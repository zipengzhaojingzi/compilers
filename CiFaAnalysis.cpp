#include<bits/stdc++.h>
using namespace std;

string resvWords_dir[33] = {   //保留字数组
    "int", "long", "short", "float", "double", "char","unsigned", "signed", "const", "void", "volatile",
    "enum", "struct", "union", "if", "else", "goto", "switch", "case", "do", "while", "for", "int36",
    "continue", "break", "return", "default", "typedef", "auto", "register","extern","static", "sizeof"
};

string bound_dir[11] = { "(", ")", "{" ,"}", "[", "]", ";", ",", ".", "\"", "\'" };  //界符数组

string sig_dir[26] = {  //运算符数组
    "+", "-", "*", "/", "<", "<=", ">", ">=", "=", "==","!=", "^", "#", "&",
    "&&", "|", "||", "%", "~", "<<", ">>", "\\", "\?", ":", "!","++"
};

string identifier[512] = {};    //标识符数组，顺序记录所有识别到的标识符
int identifier_flag = 0;    //标识符数组
string identifier_only[512] = {};    //标识符数组，每个标识符只记录一次
int identifier_only_flag = 0;  //标识符数组指针
string identifier_int36[512] = {};  //int36类型标识符数组
int identifier_int36_flag = 0;	//INT36类型标识符数组指针
map<string, int> resvWordMap;    //用于给每个保留字标号

/*以'1'标识符，'2'界符，'3'运算符，'4'保留字，'5'常量为在, '0'为不在完成下面函数*/
//6为INT36

/*判断字符串是否是界符*/
int isBound(string s) {
    for (int i = 0; i < sizeof(bound_dir) / sizeof(bound_dir[0]) - 2; i++) {   //忽略单双引号，后续单独判断
        if (s == bound_dir[i]) {
            return 1;
        }
    }
    return 0;
}

/*判断字符串是否是运算符*/
int isOP(string s) {
    for (int i = 0; i < sizeof(sig_dir) / sizeof(sig_dir[0]); i++) {   //忽略单双引号，后续单独判断
        if (s == sig_dir[i]) {
            return 1;
        }
    }
    return 0;
}



/*判断字符串是否在identifier_only数组中*/
int isExist_identifier_only(string s) {
    for (int i = 0; i < identifier_only_flag; i++)
    {
        if (s == identifier_only[i])
            return 1;
    }
    return 0;
}

/*判断字符串是否在identifier_only数组中*/
int isExist_identifier(string s) {
    for (int i = 0; i < identifier_flag; i++)
    {
        if (s == identifier[i])
            return 1;
    }
    return 0;
}

/*判断字符串是否在identifier_int36数组中*/
int isExist_int36(string s) {
    for (int i = 0; i < identifier_int36_flag; i++) {
        if (s == identifier_int36[i]) {
            return 1;
        }
    }
    return 0;
}

/*过滤代码, 删除代码中的注释*/
string FilterCode(string& inputStr)
{
    string tmp;
    int count = 0;
    int len = inputStr.length();
    for (int i = 0; i <= len - 2; i++) {
        // 单行注释过滤
        if (inputStr[i] == '/' && inputStr[i + 1] == '/') {
            while (inputStr[i] != '\n') i++;  //直到注释行结束（换行）
        }
        //多行注释过滤
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
    if (resvWordMap.count(s) == 0) { //不存在该保留字
        return -1;
    }
    else
        return resvWordMap[s];
}

int Scanner(string inputStr, string& token, int& pos, ofstream& out_fp, int& identifier_syn)
{
    int flag;
    char tmp = inputStr[pos];
    if (pos == inputStr.length()) {   //main函数中结束扫描条件
        return 0;
    }
    while (tmp == ' ') {   //处理首字符遇到空格的情况
        pos++;
        tmp = inputStr[pos];
    }
    token = ""; //初始化扫描到的字串

    //以 _ 开头的字串是标识符
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
    //首先判断该字符串在 = 的左边或右边，若在左边则是标识符，若在右边；
    //查int36标识符数组是否存在该字符串，若存在，则是标识符，若不存在，则是int36常量，需要注意，若不存在且为数组，也识别为数标识符
    //以大小写字母开头的字串可能是保留字或标识符
    else if (islower(inputStr[pos]) || isupper(inputStr[pos])) {
        int tmp_pos = pos;
        string tmp_token;

        while ((islower(inputStr[tmp_pos]) || isupper(inputStr[tmp_pos]) || isdigit(inputStr[tmp_pos]) || inputStr[tmp_pos] == '_')) {
            tmp_token += inputStr[tmp_pos];
            tmp_pos++;
        }
        if (SearchRecvWord(tmp_token) != -1) {   //搜索保留字
            //遇到int36关键字，修改int36标记，表示下一个标识符会加入int36标识符数组
            if (tmp_token == "int36") {
                identifier_syn = 2; //表示下一个标识符需要同时记录到identifier_int36数组中
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
        //例子 int36 a[10] = a[9] + u7;
        //例子 a == u7
        //不是关键字就是标识符或int36数值
        else {
            //如果identifier_syn=0，先查identifier_only数组，若存在为标识符，若不存在为int36数值
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
    /*当以数字开头的字符串*/
    else if (isdigit(inputStr[pos])) {
        int tmp_pos = pos;
        string tmp_token = "";
        flag = 5;   //默认是常量
        //逐个接收字符
        while (isdigit(inputStr[tmp_pos]) || islower(inputStr[tmp_pos]) || isupper(inputStr[tmp_pos])) {
            tmp_token += inputStr[tmp_pos];
            ++tmp_pos;
            // 若出现一个字母，表示int36数值
            if (islower(inputStr[tmp_pos]) || isupper(inputStr[tmp_pos])) {
                flag = 6;
            }
        }

        //若前面的字符全是数字，即flag=5，且接收到点号，识别为浮点数
        if (inputStr[tmp_pos] == '.' && flag == 5) {
            tmp_token += inputStr[tmp_pos];
            ++tmp_pos;
            //若为浮点数，点号后面一定要接数字
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

        //若全为数字，查看最近的标识符类别，判断数字为常量或int36
        if (isExist_int36(identifier[identifier_flag - 1])) {
            flag = 6;
        }
    }

    /*以界符开头的字串，那必然是界符呀*/
    /*string (1, inputStr[pos])是创建一个单个字符inputStr[pos]的string类型对象*/
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

    /*单独识别双引号，用于判断字串是否属于char[]类型的数据*/
    else if (inputStr[pos] == '\"') {

        //输出界符 flag = 2(未实现)
        token = inputStr[pos];
        cout << "界符" << '(' << 2 << ',' << token << ')' << endl;
        out_fp << "界符" << '(' << 2 << ',' << token << ')' << endl;
        token = "";
        pos++;
        while (1) {
            if (inputStr[pos] == '\"') {   //遇到第二个引号
                // 输出常量 flag = 6（未实现）
                break;
            }
            token += inputStr[pos];
            pos++;
        }
        cout << "常量" << '(' << 5 << ',' << token << ')' << endl;
        out_fp << "常量" << '(' << 5 << ',' << token << ')' << endl;
        token = inputStr[pos];
        flag = 2;
        pos++;
    }

    /*单独识别单引号，用于判断字串是否属于char类型的数据*/
    else if (inputStr[pos] == '\'') {
        token = inputStr[pos];
        cout << "界符" << '(' << 2 << ',' << token << ')' << endl;
        out_fp << "界符" << '(' << 2 << ',' << token << ')' << endl;
        pos++;

        token = inputStr[pos];
        cout << "常量" << '(' << 5 << ',' << token << ')' << endl;
        out_fp << "常量" << '(' << 5 << ',' << token << ')' << endl;
        pos++;

        if (inputStr[pos] != '\'') {   //遇到第二个引号
            printf("char content error\n");
            exit(1);

        }
        //只有一个引号 
        else {
            token = inputStr[pos];
            pos++;
            flag = 2;
        }

    }

    /*识别运算符*/
    else if (isOP(string(1, inputStr[pos]))) {
        flag = 3;
        token += inputStr[pos];
        pos++;
        if (!(inputStr[pos - 1] == '=')) {
            identifier_syn = 1;
        }
        //可能出现由两个字符组成的运算符的情况
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
  // 打开文件
    ifstream in_fp("test.txt");
    ofstream out_fp("res.txt");
    std::string result; // 新增结果收集变量

    if (!in_fp.is_open()) {
        return "read_file_error";
    }
    if (!out_fp.is_open()) {
        return "write_file_error";
    }

    for (int i = 0; i < 31; i++) {
        resvWordMap[resvWords_dir[i]] = i + 1;  //为31个保留字赋值1-32
    }

    string inputStr, token; //读入源代码    当前字串
    char tmp;   //当前字符
    while (!in_fp.eof()) {
        tmp = in_fp.get();
        inputStr += tmp;
    }
    in_fp.close();

    //去除注释以及转义字符
    inputStr = FilterCode(inputStr);

    int flag = -1;
    int pos = 0;    //扫描器扫描位置pos

    // 字符串为标识符：1，当遇到int36关键字时，将syn设置为1，当对标识符加入int36标识符数组后，重置为0
    int identifier_syn = 0;

    while (flag != 0) {
        flag = Scanner(inputStr, token, pos, out_fp, identifier_syn);
        //标识符
        if (flag == 1) {
            cout << "标识符" << '(' << 1 << ',' << token << ')' << endl;
            out_fp << "标识符" << '(' << 1 << ',' << token << ')' << endl;
        }
        //界符
        else if (flag == 2) {
            cout << "界符" << '(' << 2 << ',' << token << ')' << endl;
            out_fp << "界符" << '(' << 2 << ',' << token << ')' << endl;
        }
        //运算符
        else if (flag == 3) {
            cout << "运算符" << '(' << 3 << ',' << token << ')' << endl;
            out_fp << "运算符" << '(' << 3 << ',' << token << ')' << endl;
        }
        //保留字
        else if (flag == 4) {
            cout << "保留字" << '(' << 4 << ',' << token << ')' << endl;
            out_fp << "保留字" << '(' << 4 << ',' << token << ')' << endl;

        }
        //常量
        else if (flag == 5) {
            cout << "常量" << '(' << 5 << ',' << token << ')' << endl;
            out_fp << "常量" << '(' << 5 << ',' << token << ')' << endl;
        }
        else if (flag == 6) {
            cout << "INT36" << '(' << 6 << ',' << token << ')' << endl;
            out_fp << "INT36" << '(' << 6 << ',' << token << ')' << endl;
        }
        else if (flag == 0) {
            cout << "complete" << endl;
        }
        else {    //报错
            cout << "error" << endl;
            exit(1);
        }
    }
out_fp.flush();  // 强制刷新缓冲区
out_fp.close();   // 先关闭输出流

ifstream in("res.txt");  // 重新打开读取
stringstream buffer;
buffer << in.rdbuf();
result = buffer.str();
in.close();

    return result; // 返回实际分析结果
}

// int main() {
//     string res = AnalyzeCiFaStr();
//     if (res.empty()) {
//         cerr << "ERROR: Empty analysis result" << endl;
//     }
//     cout << "LEX OUTPUT:\n" << res << endl;
//     return 0;
// }
