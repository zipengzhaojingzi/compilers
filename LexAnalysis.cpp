#include <iostream>  
#include <fstream>  
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <vector>  
#include <iomanip>  
#include "LexAnalysis.h"  
  
using namespace std;  
  
int leftSmall = 0;//��С����  
int rightSmall = 0;//��С����  
int leftMiddle = 0;//��������  
int rightMiddle = 0;//��������  
int leftBig = 0;//�������  
int rightBig = 0;//�Ҵ�����  
int lineBra[6][1000] = {0};//���ź������Ķ�Ӧ��ϵ����һά��������6������  
int static_iden_number = 0;//ģ���־���ĵ�ַ������  
//Token�ڵ�  
  
  
NormalNode * normalHead;//�׽��  
  
//����ڵ�  
struct ErrorNode  
{  
    char content[30];//��������  
    char describe[30];//��������  
    int type;  
    int line;//��������  
    ErrorNode * next;//��һ���ڵ�  
};  
  
ErrorNode * errorHead;//�׽ڵ�  
  
//��־���ڵ�  
struct IdentiferNode  
{  
    char content[30];//����  
    char describe[30];//����  
    int type;//�ֱ���  
    int addr;//��ڵ�ַ  
    int line;//��������  
    IdentiferNode * next;//��һ���ڵ�  
};  
IdentiferNode * idenHead;//�׽ڵ�  
  
vector<pair<const char *,int> > keyMap;  
vector<pair<const char *,int> > operMap;  
vector<pair<const char *,int> > limitMap;  
  
  
  
//��ʼ��C���ԵĹؼ��ֵļ���  
void initKeyMapping()   
{  
    keyMap.clear();  
    keyMap.push_back(make_pair("auto",AUTO));  
    keyMap.push_back(make_pair("break",BREAK));  
    keyMap.push_back(make_pair("case",CASE));  
    keyMap.push_back(make_pair("char",CHAR));  
    keyMap.push_back(make_pair("const",CONST));  
    keyMap.push_back(make_pair("continue",CONTINUE));  
    keyMap.push_back(make_pair("default",DEFAULT));  
    keyMap.push_back(make_pair("do",DO));  
    keyMap.push_back(make_pair("double",DOUBLE));  
    keyMap.push_back(make_pair("else",ELSE));  
    keyMap.push_back(make_pair("enum",ENUM));  
    keyMap.push_back(make_pair("extern",EXTERN));  
    keyMap.push_back(make_pair("float",FLOAT));  
    keyMap.push_back(make_pair("for",FOR));  
    keyMap.push_back(make_pair("goto",GOTO));  
    keyMap.push_back(make_pair("if",IF));  
    keyMap.push_back(make_pair("int",INT));  
    keyMap.push_back(make_pair("long",LONG));  
    keyMap.push_back(make_pair("register",REGISTER));  
    keyMap.push_back(make_pair("return",RETURN));  
    keyMap.push_back(make_pair("short",SHORT));  
    keyMap.push_back(make_pair("signed",SIGNED));  
    keyMap.push_back(make_pair("sizeof",SIZEOF));  
    keyMap.push_back(make_pair("static",STATIC));  
    keyMap.push_back(make_pair("struct",STRUCT));  
    keyMap.push_back(make_pair("switch",SWITCH));  
    keyMap.push_back(make_pair("typedef",TYPEDEF));  
    keyMap.push_back(make_pair("union",UNION));  
    keyMap.push_back(make_pair("unsigned",UNSIGNED));  
    keyMap.push_back(make_pair("void",VOID));  
    keyMap.push_back(make_pair("volatile",VOLATILE));  
    keyMap.push_back(make_pair("while",WHILE));  
  
    keyMap.push_back(make_pair("describe",DESCRIBE));  
    keyMap.push_back(make_pair("type",TYPE));  
    keyMap.push_back(make_pair("string",STRING));  
    keyMap.push_back(make_pair("digit",DIGIT));  
    // keyMap.push_back(make_pair("struct", STRUCT));  // ��� struct �ؼ���

}  
void initOperMapping()  
{  
    operMap.clear();  
    operMap.push_back(make_pair("!",NOT));  
    operMap.push_back(make_pair("&",BYTE_AND));  
    operMap.push_back(make_pair("~",COMPLEMENT));  
    operMap.push_back(make_pair("^",BYTE_XOR));  
    operMap.push_back(make_pair("*",MUL));  
    operMap.push_back(make_pair("/",DIV));  
    operMap.push_back(make_pair("%",MOD));  
    operMap.push_back(make_pair("+",ADD));  
    operMap.push_back(make_pair("-",SUB));  
    operMap.push_back(make_pair("<",LES_THAN));  
    operMap.push_back(make_pair(">",GRT_THAN));  
    operMap.push_back(make_pair("=",ASG));  
    operMap.push_back(make_pair("->",ARROW));  
    operMap.push_back(make_pair("++",SELF_ADD));  
    operMap.push_back(make_pair("--",SELF_SUB));  
    operMap.push_back(make_pair("<<",LEFT_MOVE));  
    operMap.push_back(make_pair(">>",RIGHT_MOVE));  
    operMap.push_back(make_pair("<=",LES_EQUAL));  
    operMap.push_back(make_pair(">=",GRT_EQUAL));  
    operMap.push_back(make_pair("==",EQUAL));  
    operMap.push_back(make_pair("!=",NOT_EQUAL));  
    operMap.push_back(make_pair("&&",AND));  
    operMap.push_back(make_pair("||",OR));  
    operMap.push_back(make_pair("+=",COMPLETE_ADD));  
    operMap.push_back(make_pair("-=",COMPLETE_SUB));  
    operMap.push_back(make_pair("*=",COMPLETE_MUL));  
    operMap.push_back(make_pair("/=",COMPLETE_DIV));  
    operMap.push_back(make_pair("^=",COMPLETE_BYTE_XOR));  
    operMap.push_back(make_pair("&=",COMPLETE_BYTE_AND));  
    operMap.push_back(make_pair("~=",COMPLETE_COMPLEMENT));  
    operMap.push_back(make_pair("%=",COMPLETE_MOD));  
    operMap.push_back(make_pair("|",BYTE_OR));  
     operMap.push_back(make_pair(".", DOT));  // ��ӳ�Ա���ʲ����� .

}  
void initLimitMapping()  
{  
    limitMap.clear();  
    limitMap.push_back(make_pair("(",LEFT_BRA));  
    limitMap.push_back(make_pair(")",RIGHT_BRA));  
    limitMap.push_back(make_pair("[",LEFT_INDEX));  
    limitMap.push_back(make_pair("]",RIGHT_INDEX));  
    limitMap.push_back(make_pair("{",L_BOUNDER));  
    limitMap.push_back(make_pair("}",R_BOUNDER));  
    limitMap.push_back(make_pair(".",POINTER));  
    limitMap.push_back(make_pair("#",JING));  
    limitMap.push_back(make_pair("_",UNDER_LINE));  
    limitMap.push_back(make_pair(",",COMMA));  
    limitMap.push_back(make_pair(";",SEMI));  
    limitMap.push_back(make_pair("'",SIN_QUE));  
    limitMap.push_back(make_pair("\"",DOU_QUE));  
    //   limitMap.push_back(make_pair("{", LBRACE));  // �������
    // limitMap.push_back(make_pair("}", RBRACE));  // �Ҵ�����

}  
void initNode()  
{  
    normalHead = new NormalNode();  
    strcpy(normalHead->content,"");  
    strcpy(normalHead->describe,"");  
    normalHead->type = -1;  
    normalHead->addr = -1;  
    normalHead->line = -1;  
    normalHead->next = NULL;  
  
    errorHead = new ErrorNode();  
    strcpy(errorHead->content,"");  
    strcpy(errorHead->describe,"");  
    errorHead->line = -1;  
    errorHead->next = NULL;  
  
    idenHead = new IdentiferNode();  
    strcpy(idenHead->content,"");  
    strcpy(idenHead->describe,"");  
    idenHead->type = -1;  
    idenHead->addr = -1;  
    idenHead->line = -1;  
    idenHead->next = NULL;  
}  
  
void createNewNode(char * content,char *descirbe,int type,int addr,int line)  
{  
    NormalNode * p = normalHead;  
    NormalNode * temp = new NormalNode();  
  
    while(p->next!=NULL)  
    {  
        p = p->next;  
    }  
  
    strcpy(temp->content,content);  
    strcpy(temp->describe,descirbe);  
    temp->type = type;  
    temp->addr = addr;  
    temp->line = line;  
    temp->next = NULL;  
  
    p->next = temp;  
}  
void createNewError(char * content,char *descirbe,int type,int line)  
{  
    ErrorNode * p = errorHead;  
    ErrorNode * temp = new ErrorNode();  
  
    strcpy(temp->content,content);  
    strcpy(temp->describe,descirbe);  
    temp->type = type;  
    temp->line = line;  
    temp->next = NULL;  
    while(p->next!=NULL)  
    {  
        p = p->next;  
    }  
    p->next = temp;  
}  
//����ֵ���µı�־������ڵ�ַ  
int createNewIden(char * content,char *descirbe,int type,int addr,int line)  
{  
    IdentiferNode * p = idenHead;  
    IdentiferNode * temp = new IdentiferNode();  
    int flag = 0;  
    int addr_temp = -2;  
    while(p->next!=NULL)  
    {  
        if(strcmp(content,p->next->content) == 0)  
        {  
            flag = 1;  
            addr_temp = p->next->addr;  
        }  
        p = p->next;  
    }  
    if(flag == 0)  
    {  
        addr_temp = ++static_iden_number;//��������ģ����ڵ�ַ  
    }  
    strcpy(temp->content,content);  
    strcpy(temp->describe,descirbe);  
    temp->type = type;  
    temp->addr = addr_temp;  
    temp->line = line;  
    temp->next = NULL;  
    p->next = temp;  
    return addr_temp;  
}  
  
void printNodeLink()  
{  
    NormalNode * p = normalHead;  
    p = p->next;  
    cout<<"************************************������******************************"<<endl<<endl;  
    cout<<setw(30)<<"����"<<setw(10)<<"����"<<"\t"<<"�ֱ���"<<"\t"<<"��ַ"<<"\t"<<"�к�"<<endl;  
    while(p!=NULL)  
    {  
        if(p->type == IDENTIFER)  
        {  
            cout<<setw(30)<<p->content<<setw(10)<<p->describe<<"\t"<<p->type<<"\t"<<p->addr<<"\t"<<p->line<<endl;  
        }  
        else  
        {  
            cout<<setw(30)<<p->content<<setw(10)<<p->describe<<"\t"<<p->type<<"\t"<<"\t"<<p->line<<endl;  
        }  
        p = p->next;  
    }  
    cout<<endl<<endl;  
}  
/* 
�������ࣺ 
1.float��ʾ���� 
2.double��ʾ���� 
3.ע��û�н����� 
4.�ַ�������û�н����� 
5.�ַ�����û�н����� 
6.�Ƿ��ַ� 
7.'('û�ж�Ӧ�� 
8.Ԥ������� 
*/  
void printErrorLink()  
{  
    ErrorNode * p = errorHead;  
    p = p->next;  
    cout<<"************************************�����******************************"<<endl<<endl;  
    cout<<setw(10)<<"����"<<setw(30)<<"����"<<"\t"<<"����"<<"\t"<<"�к�"<<endl;  
    while(p!=NULL)  
    {  
        cout<<setw(10)<<p->content<<setw(30)<<p->describe<<"\t"<<p->type<<"\t"<<p->line<<endl;  
        p = p->next;  
    }  
    cout<<endl<<endl;  
}  
//��־�������ظ����֣��ݲ�����  
void printIdentLink()  
{  
    IdentiferNode * p = idenHead;  
    p = p->next;  
    cout<<"************************************��־����******************************"<<endl<<endl;  
    cout<<setw(30)<<"����"<<setw(10)<<"����"<<"\t"<<"�ֱ���"<<"\t"<<"��ַ"<<"\t"<<"�к�"<<endl;  
    while(p!=NULL)  
    {  
        cout<<setw(30)<<p->content<<setw(10)<<p->describe<<"\t"<<p->type<<"\t"<<p->addr<<"\t"<<p->line<<endl;  
        p = p->next;  
    }  
    cout<<endl<<endl;  
}  
int mystrlen(char * word)  
{  
    if(*word == '\0')  
    {  
        return 0;  
    }  
    else  
    {  
        return 1+mystrlen(word+1);  
    }  
}  
//Ԥ��������ͷ�ļ��ͺ궨��  
void preProcess(char * word,int line)  
{  
    const char * include_temp = "include";  
    const char * define_temp = "define";  
    char * p_include,*p_define;  
    int flag = 0;  
    p_include = strstr(word,include_temp);  
    if(p_include!=NULL)  
    {  
        flag = 1;  
        int i;  
        for(i=7;;)  
        {  
            if(*(p_include+i) == ' ' || *(p_include+i) == '\t')  
            {  
                i++;  
            }  
            else  
            {  
                break;  
            }  
        }  
        createNewNode(p_include+i,HEADER_DESC,HEADER,-1,line);  
    }  
    else  
    {  
        p_define = strstr(word,define_temp);  
        if(p_define!=NULL)  
        {  
            flag = 1;  
            int i;  
            for(i=7;;)  
            {  
                if(*(p_define+i) == ' ' || *(p_define+i) == '\t')  
                {  
                    i++;  
                }  
                else  
                {  
                    break;  
                }  
            }  
            createNewNode(p_define+i,CONSTANT_DESC,MACRO_VAL,-1,line);  
        }  
    }  
    if(flag == 0)  
    {  
        createNewError(word,PRE_PROCESS_ERROR,PRE_PROCESS_ERROR_NUM,line);  
    }  
}  
  
void close()  
{  
    //delete idenHead;  
    //delete errorHead;  
    //delete normalHead;  
}  
  
int seekKey(char * word)  
{  
    for(int i=0; i<keyMap.size(); i++)  
    {  
        if(strcmp(word,keyMap[i].first) == 0)  
        {  
            return i+1;  
        }  
    }  
    return IDENTIFER;  
}  
  
void scanner()  
{  
    char filename[30];  
    char ch;  
    char array[30];//���ʳ���������30  
    char * word;  
    int i;  
    int line = 1;//����  
  
  
    FILE * infile;  
    printf("������Ҫ�����﷨������C���Գ���\n");  
    scanf("%s",filename);  
    infile = fopen(filename,"r");  
    while(!infile)  
    {  
        printf("���ļ�ʧ�ܣ�\n");  
        return;  
    }  
    ch = fgetc(infile);  
    while(ch!=EOF)  
    {  
        // printf("ʶ�𵽷���: %s (%d)\n", word, type);  // �����־
   
        i = 0;  
        //����ĸ�����»��߿�ͷ,����ؼ��ֻ��߱�ʶ��  
        if((ch>='A' && ch<='Z') || (ch>='a' && ch<='z') || ch == '_')  
        {  
            while((ch>='A' && ch<='Z')||(ch>='a' && ch<='z')||(ch>='0' && ch<='9') || ch == '_')  
            {  
                array[i++] = ch;  
                ch = fgetc(infile);  
            }  
            word = new char[i+1];  
            memcpy(word,array,i);  
            word[i] = '\0';  
            int seekTemp = seekKey(word);  
            if(seekTemp!=IDENTIFER)  
            {  
                createNewNode(word,KEY_DESC,seekTemp,-1,line);  
            }  
            else  
            {  
                int addr_tmp = createNewIden(word,IDENTIFER_DESC,seekTemp,-1,line);  
                createNewNode(word,IDENTIFER_DESC,seekTemp,addr_tmp,line);  
            }  
            fseek(infile,-1L,SEEK_CUR);//������һλ  
        }  
        //�����ֿ�ͷ����������  
        else if(ch >='0' && ch<='9')  
        {  
            int flag = 0;  
            int flag2 = 0;  
            //��������  
            while(ch >='0' && ch<='9')  
            {  
                array[i++] = ch;  
                ch = fgetc(infile);  
            }  
            //����float  
            if(ch == '.')  
            {  
                flag2 = 1;  
                array[i++] = ch;  
                ch = fgetc(infile);  
                if(ch>='0' && ch<='9')  
                {  
                    while(ch>='0' && ch<='9')  
                    {  
                        array[i++] = ch;  
                        ch = fgetc(infile);  
                    }  
                }  
                else  
                {  
                    flag = 1;  
                }  
  
                //����Double  
                if(ch == 'E' || ch == 'e')  
                {  
                    array[i++] = ch;  
                    ch = fgetc(infile);  
                    if(ch == '+' || ch == '-')  
                    {  
                        array[i++] = ch;  
                        ch = fgetc(infile);  
                    }  
                    if(ch >='0' && ch<='9')  
                    {  
                        array[i++] = ch;  
                        ch = fgetc(infile);  
                    }  
                    else  
                    {  
                        flag = 2;  
                    }  
                }  
  
            }  
            word = new char[i+1];  
            memcpy(word,array,i);  
            word[i] = '\0';  
            if(flag == 1)  
            {  
                createNewError(word,FLOAT_ERROR,FLOAT_ERROR_NUM,line);  
            }  
            else if(flag == 2)  
            {  
                createNewError(word,DOUBLE_ERROR,DOUBLE_ERROR_NUM,line);  
            }  
            else  
            {  
                if(flag2 == 0)  
                {  
                    createNewNode(word,CONSTANT_DESC,INT_VAL,-1,line);  
                }  
                else  
                {  
                    createNewNode(word,CONSTANT_DESC,FLOAT_VAL,-1,line);  
                }  
            }  
            fseek(infile,-1L,SEEK_CUR);//������һλ  
        }  
        //��"/"��ͷ  
        else if(ch == '/')  
        {  
            ch = fgetc(infile);  
            //���������"/="  
            if(ch == '=')  
            {  
                createNewNode("/=",OPE_DESC,COMPLETE_DIV,-1,line);  
            }  
            //����"/**/"��ע��  
            else if(ch == '*')  
            {  
                ch =  fgetc(infile);  
                while(1)  
                {  
                    while(ch != '*')  
                    {  
                        if(ch == '\n')  
                        {  
                            line++;  
                        }  
                        ch = fgetc(infile);  
                        if(ch == EOF)  
                        {  
                            createNewError(_NULL,NOTE_ERROR,NOTE_ERROR_NUM,line);  
                            return;  
                        }  
                    }  
                    ch = fgetc(infile);  
                    if(ch == '/')  
                    {  
                        break;  
                    }  
                    if(ch == EOF)  
                    {  
                        createNewError(_NULL,NOTE_ERROR,NOTE_ERROR_NUM,line);  
                        return;  
                    }  
                }  
                createNewNode(_NULL,NOTE_DESC,NOTE1,-1,line);  
            }  
            //����"//"��ע��  
            else if(ch == '/')  
            {  
                while(ch!='\n')  
                {  
                    ch = fgetc(infile);  
                    if(ch == EOF)  
                    {  
                        createNewNode(_NULL,NOTE_DESC,NOTE2,-1,line);  
                        return;  
                    }  
                }  
                line++;  
                createNewNode(_NULL,NOTE_DESC,NOTE2,-1,line);  
                if(ch == EOF)  
                {  
                    return;  
                }  
            }  
            //�������  
            else  
            {  
                createNewNode("/",OPE_DESC,DIV,-1,line);  
            }  
        }  
        //�������ַ���  
        else if(ch == '"')  
        {  
            createNewNode("\"",CLE_OPE_DESC,DOU_QUE,-1,line);  
            ch = fgetc(infile);  
            i = 0;  
            while(ch!='"')  
            {  
                array[i++] = ch;  
                if(ch == '\n')  
                {  
                    line++;  
                }  
                ch = fgetc(infile);  
                if(ch == EOF)  
                {  
                    createNewError(_NULL,STRING_ERROR,STRING_ERROR_NUM,line);  
                    return;  
                }  
            }  
            word = new char[i+1];  
            memcpy(word,array,i);  
            word[i] = '\0';  
            createNewNode(word,CONSTANT_DESC,STRING_VAL,-1,line);  
            createNewNode("\"",CLE_OPE_DESC,DOU_QUE,-1,line);  
        }  
        //�������ַ�  
        else if(ch == '\'')  
        {  
            createNewNode("\'",CLE_OPE_DESC,SIN_QUE,-1,line);  
            ch = fgetc(infile);  
            i = 0;  
            while(ch!='\'')  
            {  
                array[i++] = ch;  
                if(ch == '\n')  
                {  
                    line++;  
                }  
                ch = fgetc(infile);  
                if(ch == EOF)  
                {  
                    createNewError(_NULL,CHARCONST_ERROR,CHARCONST_ERROR_NUM,line);  
                    return;  
                }  
            }  
            word = new char[i+1];  
            memcpy(word,array,i);  
            word[i] = '\0';  
            createNewNode(word,CONSTANT_DESC,CHAR_VAL,-1,line);  
            createNewNode("\'",CLE_OPE_DESC,SIN_QUE,-1,line);  
        }  
        else if(ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')  
        {  
            if(ch == '\n')  
            {  
                line++;  
            }  
        }  
        else  
        {  
            if(ch == EOF)  
            {  
                return;  
            }  
            //����ͷ�ļ��ͺ곣����Ԥ����  
            else if(ch == '#')  
            {  
                while(ch!='\n' && ch!=EOF)  
                {  
                    array[i++] = ch;  
                    ch = fgetc(infile);  
                }  
                word = new char[i+1];  
                memcpy(word,array,i);  
                word[i] = '\0';  
                preProcess(word,line);  
  
                fseek(infile,-1L,SEEK_CUR);//������һλ  
            }  
            //����-��ͷ�������  
            else if(ch == '-')  
            {  
                array[i++] = ch;  
                ch = fgetc(infile);  
                if(ch >='0' && ch<='9')  
                {  
                    int flag = 0;  
                    int flag2 = 0;  
                    //��������  
                    while(ch>='0' && ch<='9')  
                    {  
                        array[i++] = ch;  
                        ch = fgetc(infile);  
                    }  
                    //����float  
                    if(ch == '.')  
                    {  
                        flag2 = 1;  
                        array[i++] = ch;  
                        ch = fgetc(infile);  
                        if(ch>='0' && ch<='9')  
                        {  
                            while(ch>='0' && ch<='9')  
                            {  
                                array[i++] = ch;  
                                ch = fgetc(infile);  
                            }  
                        }  
                        else  
                        {  
                            flag = 1;  
                        }  
                        //����Double  
                        if(ch == 'E' || ch == 'e')  
                        {  
                            array[i++] = ch;  
                            ch = fgetc(infile);  
                            if(ch == '+' || ch == '-')  
                            {  
                                array[i++] = ch;  
                                ch = fgetc(infile);  
                            }  
                            if(ch >='0' && ch<='9')  
                            {  
                                array[i++] = ch;  
                                ch = fgetc(infile);  
                            }  
                            else  
                            {  
                                flag = 2;  
                            }  
                        }  
                    }  
                    word = new char[i+1];  
                    memcpy(word,array,i);  
                    word[i] = '\0';  
                    if(flag == 1)  
                    {  
                        createNewError(word,FLOAT_ERROR,FLOAT_ERROR_NUM,line);  
                    }  
                    else if(flag == 2)  
                    {  
                        createNewError(word,DOUBLE_ERROR,DOUBLE_ERROR_NUM,line);  
                    }  
                    else  
                    {  
                        if(flag2 == 0)  
                        {  
                            createNewNode(word,CONSTANT_DESC,INT_VAL,-1,line);  
                        }  
                        else  
                        {  
                            createNewNode(word,CONSTANT_DESC,FLOAT_VAL,-1,line);  
                        }  
                    }  
                    fseek(infile,-1L,SEEK_CUR);//������һλ  
                }  
                else if(ch == '>')  
                {  
                    createNewNode("->",OPE_DESC,ARROW,-1,line);  
                }  
                else if(ch == '-')  
                {  
                    createNewNode("--",OPE_DESC,SELF_SUB,-1,line);  
                }  
                else if(ch == '=')  
                {  
                    createNewNode("--",OPE_DESC,SELF_SUB,-1,line);  
                }  
                else  
                {  
                    createNewNode("-",OPE_DESC,SUB,-1,line);  
                    fseek(infile,-1L,SEEK_CUR);  
                }  
            }  
            //����+��ͷ�������  
            else if(ch == '+')  
            {  
                ch = fgetc(infile);  
                if(ch == '+')  
                {  
                    createNewNode("++",OPE_DESC,SELF_ADD,-1,line);  
                }  
                else if(ch == '=')  
                {  
                    createNewNode("+=",OPE_DESC,COMPLETE_ADD,-1,line);  
                }  
                else  
                {  
                    createNewNode("+",OPE_DESC,ADD,-1,line);  
                    fseek(infile,-1L,SEEK_CUR);  
                }  
            }  
            //����*��ͷ�������  
            else if(ch == '*')  
            {  
                ch = fgetc(infile);  
                if(ch == '=')  
                {  
                    createNewNode("*=",OPE_DESC,COMPLETE_MUL,-1,line);  
                }  
                else  
                {  
                    createNewNode("*",OPE_DESC,MUL,-1,line);  
                    fseek(infile,-1L,SEEK_CUR);  
                }  
            }  
            //����^��ͷ�������  
            else if(ch == '^')  
            {  
                ch = fgetc(infile);  
                if(ch == '=')  
                {  
                    createNewNode("^=",OPE_DESC,COMPLETE_BYTE_XOR,-1,line);  
                }  
                else  
                {  
                    createNewNode("^",OPE_DESC,BYTE_XOR,-1,line);  
                    fseek(infile,-1L,SEEK_CUR);  
                }  
            }  
            //����%��ͷ�������  
            else if(ch == '%')  
            {  
                ch = fgetc(infile);  
                if(ch == '=')  
                {  
                    createNewNode("%=",OPE_DESC,COMPLETE_MOD,-1,line);  
                }  
                else  
                {  
                    createNewNode("%",OPE_DESC,MOD,-1,line);  
                    fseek(infile,-1L,SEEK_CUR);  
                }  
            }  
            //����&��ͷ�������  
            else if(ch == '&')  
            {  
                ch = fgetc(infile);  
                if(ch == '=')  
                {  
                    createNewNode("&=",OPE_DESC,COMPLETE_BYTE_AND,-1,line);  
                }  
                else if(ch == '&')  
                {  
                    createNewNode("&&",OPE_DESC,AND,-1,line);  
                }  
                else  
                {  
                    createNewNode("&",OPE_DESC,BYTE_AND,-1,line);  
                    fseek(infile,-1L,SEEK_CUR);  
                }  
            }  
            //����~��ͷ�������  
            else if(ch == '~')  
            {  
                ch = fgetc(infile);  
                if(ch == '=')  
                {  
                    createNewNode("~=",OPE_DESC,COMPLETE_COMPLEMENT,-1,line);  
                }  
                else  
                {  
                    createNewNode("~",OPE_DESC,COMPLEMENT,-1,line);  
                    fseek(infile,-1L,SEEK_CUR);  
                }  
            }  
            //����!��ͷ�������  
            else if(ch == '!')  
            {  
                ch = fgetc(infile);  
                if(ch == '=')  
                {  
                    createNewNode("!=",OPE_DESC,NOT_EQUAL,-1,line);  
                }  
                else  
                {  
                    createNewNode("!",OPE_DESC,NOT,-1,line);  
                    fseek(infile,-1L,SEEK_CUR);  
                }  
            }  
            //����<��ͷ�������  
            else if(ch == '<')  
            {  
                ch = fgetc(infile);  
                if(ch == '<')  
                {  
                    createNewNode("<<",OPE_DESC,LEFT_MOVE,-1,line);  
                }  
                else if(ch == '=')  
                {  
                    createNewNode("<=",OPE_DESC,LES_EQUAL,-1,line);  
                }  
                else  
                {  
                    createNewNode("<",OPE_DESC,LES_THAN,-1,line);  
                    fseek(infile,-1L,SEEK_CUR);  
                }  
            }  
            //����>��ͷ�������  
            else if(ch == '>')  
            {  
                ch = fgetc(infile);  
                if(ch == '>')  
                {  
                    createNewNode(">>",OPE_DESC,RIGHT_MOVE,-1,line);  
                }  
                else if(ch == '=')  
                {  
                    createNewNode(">=",OPE_DESC,GRT_EQUAL,-1,line);  
                }  
                else  
                {  
                    createNewNode(">",OPE_DESC,GRT_THAN,-1,line);  
                    fseek(infile,-1L,SEEK_CUR);  
                }  
            }  
            //����|��ͷ�������  
            else if(ch == '|')  
            {  
                ch = fgetc(infile);  
                if(ch == '|')  
                {  
                    createNewNode("||",OPE_DESC,OR,-1,line);  
                }  
                else  
                {  
                    createNewNode("|",OPE_DESC,BYTE_OR,-1,line);  
                    fseek(infile,-1L,SEEK_CUR);  
                }  
            }  
            else if(ch == '=')  
            {  
                ch = fgetc(infile);  
                if(ch == '=')  
                {  
                    createNewNode("==",OPE_DESC,EQUAL,-1,line);  
                }  
                else  
                {  
                    createNewNode("=",OPE_DESC,ASG,-1,line);  
                    fseek(infile,-1L,SEEK_CUR);  
                }  
            }  
            else if(ch == '(')  
            {  
                leftSmall++;  
                lineBra[0][leftSmall] = line;  
                createNewNode("(",CLE_OPE_DESC,LEFT_BRA,-1,line);  
            }  
            else if(ch == ')')  
            {  
                rightSmall++;  
                lineBra[1][rightSmall] = line;  
                createNewNode(")",CLE_OPE_DESC,RIGHT_BRA,-1,line);  
            }  
            else if(ch == '[')  
            {  
                leftMiddle++;  
                lineBra[2][leftMiddle] = line;  
                createNewNode("[",CLE_OPE_DESC,LEFT_INDEX,-1,line);  
            }  
            else if(ch == ']')  
            {  
                rightMiddle++;  
                lineBra[3][rightMiddle] = line;  
                createNewNode("]",CLE_OPE_DESC,RIGHT_INDEX,-1,line);  
            }  
            else if(ch == '{')  
            {  
                leftBig++;  
                lineBra[4][leftBig] = line;  
                createNewNode("{",CLE_OPE_DESC,L_BOUNDER,-1,line);  
            }  
            else if(ch == '}')  
            {  
                rightBig++;  
                lineBra[5][rightBig] = line;  
                createNewNode("}",CLE_OPE_DESC,R_BOUNDER,-1,line);  
            }  
            else if(ch == '.')  
            {  
                createNewNode(".",CLE_OPE_DESC,DOT,-1,line);  
            }  
            else if(ch == ',')  
            {  
                createNewNode(",",CLE_OPE_DESC,COMMA,-1,line);  
            }  
            else if(ch == ';')  
            {  
                createNewNode(";",CLE_OPE_DESC,SEMI,-1,line);  
            }  
            else  
            {  
                char temp[2];  
                temp[0] = ch;  
                temp[1] = '\0';  
                createNewError(temp,CHAR_ERROR,CHAR_ERROR_NUM,line);  
            }  
        }  
        ch = fgetc(infile);  
    }  
}  
void BraMappingError()  
{  
    if(leftSmall != rightSmall)  
    {  
        int i = (leftSmall>rightSmall) ? (leftSmall-rightSmall) : (rightSmall - leftSmall);  
        bool  flag = (leftSmall>rightSmall) ? true : false;  
        if(flag)  
        {  
            while(i--)  
            {  
                createNewError(_NULL,LEFT_BRA_ERROR,LEFT_BRA_ERROR_NUM,lineBra[0][i+1]);  
            }  
        }  
        else  
        {  
            while(i--)  
            {  
                createNewError(_NULL,RIGHT_BRA_ERROR,RIGHT_BRA_ERROR_NUM,lineBra[1][i+1]);  
            }  
        }  
    }  
    if(leftMiddle != rightMiddle)  
    {  
        int i = (leftMiddle>rightMiddle) ? (leftMiddle-rightMiddle) : (rightMiddle - leftMiddle);  
        bool flag = (leftMiddle>rightMiddle) ? true : false;  
        if(flag)  
        {  
            while(i--)  
            {  
                createNewError(_NULL,LEFT_INDEX_ERROR,LEFT_INDEX_ERROR_NUM,lineBra[2][i+1]);  
            }  
        }  
        else  
        {  
            while(i--)  
            {  
                createNewError(_NULL,RIGHT_INDEX_ERROR,RIGHT_INDEX_ERROR_NUM,lineBra[3][i+1]);  
            }  
        }  
    }  
    if(leftBig != rightBig)  
    {  
        int i = (leftBig>rightBig) ? (leftBig-rightBig) : (rightBig - leftSmall);  
        bool flag = (leftBig>rightBig) ? true : false;  
        if(flag)  
        {  
            while(i--)  
            {  
                createNewError(_NULL,L_BOUNDER_ERROR,L_BOUNDER_ERROR_NUM,lineBra[4][i+1]);  
            }  
        }  
        else  
        {  
            while(i--)  
            {  
                createNewError(_NULL,R_BOUNDER_ERROR,R_BOUNDER_ERROR_NUM,lineBra[5][i+1]);  
            }  
        }  
    }  
}  
