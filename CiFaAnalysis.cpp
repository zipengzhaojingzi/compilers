#include<bits/stdc++.h>
using namespace std;

string resvWords_dir[33] = {   //����������
    "int", "long", "short", "float", "double", "char","unsigned", "signed", "const", "void", "volatile",
    "enum", "struct", "union", "if", "else", "goto", "switch", "case", "do", "while", "for", "int36",
    "continue", "break", "return", "default", "typedef", "auto", "register","extern","static", "sizeof"
};

string bound_dir[11] = { "(", ")", "{" ,"}", "[", "]", ";", ",", ".", "\"", "\'" };  //�������

string sig_dir[26] = {  //���������
    "+", "-", "*", "/", "<", "<=", ">", ">=", "=", "==","!=", "^", "#", "&",
    "&&", "|", "||", "%", "~", "<<", ">>", "\\", "\?", ":", "!","++"
};

string identifier[512] = {};    //��ʶ�����飬˳���¼����ʶ�𵽵ı�ʶ��
int identifier_flag = 0;    //��ʶ������
string identifier_only[512] = {};    //��ʶ�����飬ÿ����ʶ��ֻ��¼һ��
int identifier_only_flag = 0;  //��ʶ������ָ��
string identifier_int36[512] = {};  //int36���ͱ�ʶ������
int identifier_int36_flag = 0;	//INT36���ͱ�ʶ������ָ��
map<string, int> resvWordMap;    //���ڸ�ÿ�������ֱ��

/*��'1'��ʶ����'2'�����'3'�������'4'�����֣�'5'����Ϊ��, '0'Ϊ����������溯��*/
//6ΪINT36

/*�ж��ַ����Ƿ��ǽ��*/
int isBound(string s) {
    for (int i = 0; i < sizeof(bound_dir) / sizeof(bound_dir[0]) - 2; i++) {   //���Ե�˫���ţ����������ж�
        if (s == bound_dir[i]) {
            return 1;
        }
    }
    return 0;
}

/*�ж��ַ����Ƿ��������*/
int isOP(string s) {
    for (int i = 0; i < sizeof(sig_dir) / sizeof(sig_dir[0]); i++) {   //���Ե�˫���ţ����������ж�
        if (s == sig_dir[i]) {
            return 1;
        }
    }
    return 0;
}



/*�ж��ַ����Ƿ���identifier_only������*/
int isExist_identifier_only(string s) {
    for (int i = 0; i < identifier_only_flag; i++)
    {
        if (s == identifier_only[i])
            return 1;
    }
    return 0;
}

/*�ж��ַ����Ƿ���identifier_only������*/
int isExist_identifier(string s) {
    for (int i = 0; i < identifier_flag; i++)
    {
        if (s == identifier[i])
            return 1;
    }
    return 0;
}

/*�ж��ַ����Ƿ���identifier_int36������*/
int isExist_int36(string s) {
    for (int i = 0; i < identifier_int36_flag; i++) {
        if (s == identifier_int36[i]) {
            return 1;
        }
    }
    return 0;
}

/*���˴���, ɾ�������е�ע��*/
string FilterCode(string& inputStr)
{
    string tmp;
    int count = 0;
    int len = inputStr.length();
    for (int i = 0; i <= len - 2; i++) {
        // ����ע�͹���
        if (inputStr[i] == '/' && inputStr[i + 1] == '/') {
            while (inputStr[i] != '\n') i++;  //ֱ��ע���н��������У�
        }
        //����ע�͹���
        if (inputStr[i] == '/' && inputStr[i + 1] == '*') {
            i += 2;
            while (inputStr[i] != '*' || inputStr[i + 1] != '/') {
                i++;
                if (i == len - 2) {
                    cout << "code comments error" << endl;
                    exit(1);
                }
            }
            i += 2;
        }
        if (inputStr[i] != '\n' && inputStr[i] != '\t' && inputStr[i] != '\v' && inputStr[i] != '\r') {
            tmp += inputStr[i];
        }
    }
    return tmp;
}

int SearchRecvWord(string s)
{
    if (resvWordMap.count(s) == 0) { //�����ڸñ�����
        return -1;
    }
    else
        return resvWordMap[s];
}

int Scanner(string inputStr, string& token, int& pos, ofstream& out_fp, int& identifier_syn)
{
    int flag;
    char tmp = inputStr[pos];
    if (pos == inputStr.length()) {   //main�����н���ɨ������
        return 0;
    }
    while (tmp == ' ') {   //�������ַ������ո�����
        pos++;
        tmp = inputStr[pos];
    }
    token = ""; //��ʼ��ɨ�赽���ִ�

    //�� _ ��ͷ���ִ��Ǳ�ʶ��
    if (tmp == '_') {
        token += inputStr[pos];
        pos++;
        if (inputStr[pos] == ' ')
            flag = 1;
        else {
            while (islower(inputStr[pos]) || isupper(inputStr[pos]) || isdigit(inputStr[pos]) || (inputStr[pos] == '_')) {
                token += inputStr[pos];
                pos++;
            }
            flag = 1;
        }
    }
    //�����жϸ��ַ����� = ����߻��ұߣ�����������Ǳ�ʶ���������ұߣ�
    //��int36��ʶ�������Ƿ���ڸ��ַ����������ڣ����Ǳ�ʶ�����������ڣ�����int36��������Ҫע�⣬����������Ϊ���飬Ҳʶ��Ϊ����ʶ��
    //�Դ�Сд��ĸ��ͷ���ִ������Ǳ����ֻ��ʶ��
    else if (islower(inputStr[pos]) || isupper(inputStr[pos])) {
        int tmp_pos = pos;
        string tmp_token;

        while ((islower(inputStr[tmp_pos]) || isupper(inputStr[tmp_pos]) || isdigit(inputStr[tmp_pos]) || inputStr[tmp_pos] == '_')) {
            tmp_token += inputStr[tmp_pos];
            tmp_pos++;
        }
        if (SearchRecvWord(tmp_token) != -1) {   //����������
            //����int36�ؼ��֣��޸�int36��ǣ���ʾ��һ����ʶ�������int36��ʶ������
            if (tmp_token == "int36") {
                identifier_syn = 2; //��ʾ��һ����ʶ����Ҫͬʱ��¼��identifier_int36������
            }
            if (tmp_token == "int") {
                identifier_syn = 1;
            }
            if (tmp_token == "float") {
                identifier_syn = 1;
            }
            if (tmp_token == "double") {
                identifier_syn = 1;
            }
            if (tmp_token == "long") {
                identifier_syn = 1;
            }
            if (tmp_token == "char") {
                identifier_syn = 1;
            }
            pos = tmp_pos;
            token = tmp_token;
            flag = 4;
        }
        //���� int36 a[10] = a[9] + u7;
        //���� a == u7
        //���ǹؼ��־��Ǳ�ʶ����int36��ֵ
        else {
            //���identifier_syn=0���Ȳ�identifier_only���飬������Ϊ��ʶ������������Ϊint36��ֵ
            if (identifier_syn == 0) {
                pos = tmp_pos;
                token = tmp_token;
                if (isExist_identifier_only(token)) {
                    flag = 1;
                }
                else {
                    flag = 6;
                }
            }
            else if (identifier_syn == 1) {
                pos = tmp_pos;
                token = tmp_token;
                flag = 1;
                identifier[identifier_flag] = token;
                identifier_flag++;
                if (!isExist_identifier_only(token)) {
                    identifier_only[identifier_only_flag] = token;
                    identifier_only_flag++;
                }
                identifier_syn = 0;
            }
            else if (identifier_syn == 2) {
                pos = tmp_pos;
                token = tmp_token;
                flag = 1;
                identifier[identifier_flag] = token;
                identifier_flag++;
                if (!isExist_int36(token)) {
                    identifier_int36[identifier_int36_flag] = token;
                    identifier_int36_flag++;
                }
                if (!isExist_identifier_only(token)) {
                    identifier_only[identifier_only_flag] = token;
                    identifier_only_flag++;
                }
                identifier_syn = 0;
            }
        }
    }
    /*�������ֿ�ͷ���ַ���*/
    else if (isdigit(inputStr[pos])) {
        int tmp_pos = pos;
        string tmp_token = "";
        flag = 5;   //Ĭ���ǳ���
        //��������ַ�
        while (isdigit(inputStr[tmp_pos]) || islower(inputStr[tmp_pos]) || isupper(inputStr[tmp_pos])) {
            tmp_token += inputStr[tmp_pos];
            ++tmp_pos;
            // ������һ����ĸ����ʾint36��ֵ
            if (islower(inputStr[tmp_pos]) || isupper(inputStr[tmp_pos])) {
                flag = 6;
            }
        }

        //��ǰ����ַ�ȫ�����֣���flag=5���ҽ��յ���ţ�ʶ��Ϊ������
        if (inputStr[tmp_pos] == '.' && flag == 5) {
            tmp_token += inputStr[tmp_pos];
            ++tmp_pos;
            //��Ϊ����������ź���һ��Ҫ������
            if (!isdigit(inputStr[tmp_pos])) {
                printf("digit error\n");
                exit(1);
            }
            while (isdigit(inputStr[tmp_pos])) {
                tmp_token += inputStr[tmp_pos];
                ++tmp_pos;
            }
        }
        token = tmp_token;
        pos = tmp_pos;

        //��ȫΪ���֣��鿴����ı�ʶ������ж�����Ϊ������int36
        if (isExist_int36(identifier[identifier_flag - 1])) {
            flag = 6;
        }
    }

    /*�Խ����ͷ���ִ����Ǳ�Ȼ�ǽ��ѽ*/
    /*string (1, inputStr[pos])�Ǵ���һ�������ַ�inputStr[pos]��string���Ͷ���*/
    else if (isBound(string(1, inputStr[pos]))) {
        if (inputStr[pos] == ',') {
            if (isExist_int36(identifier[identifier_flag - 1])) {
                identifier_syn = 2;
            }
            else {
                identifier_syn = 1;
            }

        }
        flag = 2;
        token += inputStr[pos];
        pos++;
    }

    /*����ʶ��˫���ţ������ж��ִ��Ƿ�����char[]���͵�����*/
    else if (inputStr[pos] == '\"') {

        //������ flag = 2(δʵ��)
        token = inputStr[pos];
        cout << "���" << '(' << 2 << ',' << token << ')' << endl;
        out_fp << "���" << '(' << 2 << ',' << token << ')' << endl;
        token = "";
        pos++;
        while (1) {
            if (inputStr[pos] == '\"') {   //�����ڶ�������
                // ������� flag = 6��δʵ�֣�
                break;
            }
            token += inputStr[pos];
            pos++;
        }
        cout << "����" << '(' << 5 << ',' << token << ')' << endl;
        out_fp << "����" << '(' << 5 << ',' << token << ')' << endl;
        token = inputStr[pos];
        flag = 2;
        pos++;
    }

    /*����ʶ�����ţ������ж��ִ��Ƿ�����char���͵�����*/
    else if (inputStr[pos] == '\'') {
        token = inputStr[pos];
        cout << "���" << '(' << 2 << ',' << token << ')' << endl;
        out_fp << "���" << '(' << 2 << ',' << token << ')' << endl;
        pos++;

        token = inputStr[pos];
        cout << "����" << '(' << 5 << ',' << token << ')' << endl;
        out_fp << "����" << '(' << 5 << ',' << token << ')' << endl;
        pos++;

        if (inputStr[pos] != '\'') {   //�����ڶ�������
            printf("char content error\n");
            exit(1);

        }
        //ֻ��һ������ 
        else {
            token = inputStr[pos];
            pos++;
            flag = 2;
        }

    }

    /*ʶ�������*/
    else if (isOP(string(1, inputStr[pos]))) {
        flag = 3;
        token += inputStr[pos];
        pos++;
        if (!(inputStr[pos - 1] == '=')) {
            identifier_syn = 1;
        }
        //���ܳ����������ַ���ɵ�����������
        if (inputStr[pos - 1] == '=') {
            if (inputStr[pos] == '=') {
                token += '=';
                pos++;
            }
        }
        else if (inputStr[pos - 1] == '<') {
            if (inputStr[pos] == '<') {
                token += '<';
                pos++;
                if (inputStr[pos] == '=') {
                    token += '=';
                    pos++;
                }
            }
            else if (inputStr[pos] == '=') {
                token += '=';
                pos++;
            }
        }
        else if (inputStr[pos - 1] == '>') {
            if (inputStr[pos] == '>') {
                token += '>';
                pos++;
                if (inputStr[pos] == '=') {
                    token += '=';
                    pos++;
                }
            }
            else if (inputStr[pos] == '=') {
                token += '=';
                pos++;
            }
        }
        else if (inputStr[pos - 1] == '|') {
            if (inputStr[pos] == '|') {
                token += '|';
                pos++;
            }
            else if (inputStr[pos] == '=') {
                token += '=';
                pos++;
            }
        }
        else if (inputStr[pos - 1] == '!') {
            if (inputStr[pos] == '=') {
                token += '=';
                pos++;
            }
        }
        else if (inputStr[pos - 1] == '&') {
            if (inputStr[pos] == '&') {
                token += '&';
                pos++;
            }
            else if (inputStr[pos] == '=') {
                token += '=';
                pos++;
            }
        }
        else if (inputStr[pos - 1] == '+') {
            if (inputStr[pos] == '=') {
                token += '=';
                pos++;
            }
            else if (inputStr[pos] == '+') {
                token += '+';
                pos++;
            }
        }
        else if (inputStr[pos - 1] == '-') {
            if (inputStr[pos] == '=') {
                token += '=';
                pos++;
            }
            else if (inputStr[pos] == '-') {
                token += '-';
                pos++;
            }
        }
        else if (inputStr[pos - 1] == '*') {
            if (inputStr[pos] == '=') {
                token += '=';
                pos++;
            }
        }
        else if (inputStr[pos - 1] == '/') {
            if (inputStr[pos] == '=') {
                token += '=';
                pos++;
            }
        }
        else if (inputStr[pos - 1] == '%') {
            if (inputStr[pos] == '=') {
                token += '=';
                pos++;
            }
        }
        else if (inputStr[pos - 1] == '^') {
            if (inputStr[pos] == '=') {
                token += '=';
                pos++;
            }
        }
    }
    else {
        flag = -1;
    }

    return flag;
}

std::string AnalyzeCiFaStr()
{
  // ���ļ�
    ifstream in_fp("test.txt");
    ofstream out_fp("res.txt");
    std::string result; // ��������ռ�����

    if (!in_fp.is_open()) {
        return "read_file_error";
    }
    if (!out_fp.is_open()) {
        return "write_file_error";
    }

    for (int i = 0; i < 31; i++) {
        resvWordMap[resvWords_dir[i]] = i + 1;  //Ϊ31�������ָ�ֵ1-32
    }

    string inputStr, token; //����Դ����    ��ǰ�ִ�
    char tmp;   //��ǰ�ַ�
    while (!in_fp.eof()) {
        tmp = in_fp.get();
        inputStr += tmp;
    }
    in_fp.close();

    //ȥ��ע���Լ�ת���ַ�
    inputStr = FilterCode(inputStr);

    int flag = -1;
    int pos = 0;    //ɨ����ɨ��λ��pos

    // �ַ���Ϊ��ʶ����1��������int36�ؼ���ʱ����syn����Ϊ1�����Ա�ʶ������int36��ʶ�����������Ϊ0
    int identifier_syn = 0;

    while (flag != 0) {
        flag = Scanner(inputStr, token, pos, out_fp, identifier_syn);
        //��ʶ��
        if (flag == 1) {
            cout << "��ʶ��" << '(' << 1 << ',' << token << ')' << endl;
            out_fp << "��ʶ��" << '(' << 1 << ',' << token << ')' << endl;
        }
        //���
        else if (flag == 2) {
            cout << "���" << '(' << 2 << ',' << token << ')' << endl;
            out_fp << "���" << '(' << 2 << ',' << token << ')' << endl;
        }
        //�����
        else if (flag == 3) {
            cout << "�����" << '(' << 3 << ',' << token << ')' << endl;
            out_fp << "�����" << '(' << 3 << ',' << token << ')' << endl;
        }
        //������
        else if (flag == 4) {
            cout << "������" << '(' << 4 << ',' << token << ')' << endl;
            out_fp << "������" << '(' << 4 << ',' << token << ')' << endl;

        }
        //����
        else if (flag == 5) {
            cout << "����" << '(' << 5 << ',' << token << ')' << endl;
            out_fp << "����" << '(' << 5 << ',' << token << ')' << endl;
        }
        else if (flag == 6) {
            cout << "INT36" << '(' << 6 << ',' << token << ')' << endl;
            out_fp << "INT36" << '(' << 6 << ',' << token << ')' << endl;
        }
        else if (flag == 0) {
            cout << "complete" << endl;
        }
        else {    //����
            cout << "error" << endl;
            exit(1);
        }
    }
out_fp.flush();  // ǿ��ˢ�»�����
out_fp.close();   // �ȹر������

ifstream in("res.txt");  // ���´򿪶�ȡ
stringstream buffer;
buffer << in.rdbuf();
result = buffer.str();
in.close();

    return result; // ����ʵ�ʷ������
}

// int main() {
//     string res = AnalyzeCiFaStr();
//     if (res.empty()) {
//         cerr << "ERROR: Empty analysis result" << endl;
//     }
//     cout << "LEX OUTPUT:\n" << res << endl;
//     return 0;
// }
