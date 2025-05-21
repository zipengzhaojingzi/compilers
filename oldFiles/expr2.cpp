#include <queue>
#include <stack>
#include <set>
#include <string.h>
#include <windows.h>

#include <algorithm>
// #include <D:\20250226candid\compilers\CiFaAnalysis.cpp>
#include "CiFaAnalysis.cpp"

using namespace std;

map<string, int> tokenIndex; // 索取对应字符在字符数组tokenList中的索引
vector<string> tokenList;    // 记录文法规则中所有字符， tokenList[0] = '#'
vector<string> rule;         // 文法产生规则集合
string firstSet[100];
string followSet[200];
int analyticalTable[100][100]; // 记录分析表，索引为对应的终结符和非终结符的索引

queue<char> remainSymbQueue; // 剩余字符串队列
stack<char> analyticalStack; // 分析栈

vector<string> readFromFile(const string &filename)
{
    ifstream file(filename);
    vector<string> terminals;
    string line;

    while (getline(file, line))
    {
        // 处理每行的特殊格式（去除\r字符）
        if (!line.empty() && line.back() == '\r')
        {
            line.erase(line.size() - 1);
        }

        stringstream ss(line);
        string token;

        // 按逗号分割
        while (getline(ss, token, ','))
        {
            terminals.push_back(token);
        }
    }
    return terminals;
}

// 文法输入
void input()
{
    // tokenIndex['#'] = 0;
    // tokenList[0] = '#';

    // 输入文法包含的所有终结符，并按输入序列对终结符从1开始编号
    int num = 1;
    char char_in;
    // 在文件头部的全局变量声明区域添加
    vector<string> tokenList; // 按索引顺序存储终结符字符串
    tokenIndex["#"] = 0;
    tokenList.push_back("#");

    // 读取终结符文本
    vector<string>
        terminals = readFromFile("VT.txt");
    if (terminals.empty())
    {
        cout << "read VTfile error!" << endl;
        return;
    }

    // 建立终结符索引
    for (const auto &term : terminals)
    {
        if (!term.empty() && tokenIndex.find(term) == tokenIndex.end())
        {
            tokenIndex[term] = num++;
            tokenList.push_back(term);
        }
    }

    // 最后加入@，用来替代ε
    tokenIndex["@"] = num++;
    tokenList.push_back("@");

    vector<string> nonTerminals = readFromFile("VN.txt");
    if (nonTerminals.empty())
    {
        cout << "read VNfile error!" << endl;
        return;
    }

    // 建立非终结符索引
    for (const auto &nt : nonTerminals)
    {
        if (!nt.empty() && tokenIndex.find(nt) == tokenIndex.end())
        { 
            tokenIndex[nt] = num++;
            tokenList.push_back(nt);
        }
    }

    cout << "succeed to read: ";
    for (const auto &t : tokenList)
    {
        cout << t << " ";
    }
    cout << endl;
    // cout << "请输入文法所包含的非终结符：";
    // do
    // {
    //     cin >> char_in;
    //     tokenIndex[char_in] = num;
    //     tokenList[num++] = char_in;
    // } while (cin.peek() != '\n');

    // 读取文法规则
    ifstream ingram_fp("Gramm.txt");
    if (!ingram_fp.is_open())
    {
        cout << "read Grammfile error!" << endl;
        return;
    }
    char tmp_char;
    //
    // 读取Gramm.txt的正确方式
    string line;
    while (getline(ingram_fp, line))
    {
        if (line.empty())
            continue;
        size_t arrowPos = line.find("->");
        if (arrowPos != string::npos)
        {
            // 提取左部并清理空格
            string left = line.substr(0, arrowPos);
            left.erase(remove(left.begin(), left.end(), ' '), left.end());

            string right = line.substr(arrowPos + 2);
            // 处理 | 分割
            stringstream ss(right);
            string prod;
            while (getline(ss, prod, '|'))
            {
                rule.push_back(left + "->" + prod);
            }
        }
    }

    ingram_fp.close();

    // cout << "请输入文法的文法规则（产生式，ε用@代替), 例如: S->aBc|adB:）" << endl;

    // while (cin >> str && str != "end")
    // {
    //     string tmp;
    //     tmp += str[0];
    //     for (int i = 3; i < str.size(); i++)
    //     {
    //         // 产生式左端加入
    //         if (str[i] == '|')
    //         {
    //             rule.push_back(tmp);
    //             tmp.clear();
    //             tmp += str[0];
    //         }
    //         else
    //             tmp += str[i];
    //     }
    //     // 产生式右端加入
    //     rule.push_back(tmp);
    // }

    // 数据读入测试
    // 测试读入的终结符和非终结符
    for (int i = 0; i < num; i++)
    {
        cout << "编号" << i << "字符：" << tokenList[i] << endl;
    }
    cout << endl;
    // 打印输入的文法规则（产生式）
    for (int i = 0; i < rule.size(); i++)
    {

        cout << rule[i] << endl;
    }
}

/*合并字符串a和字符串b, 并去除a b字符串中重复的元素，将合并后的字符串放到变量a中*/
void mergeStr(string &a, string b)
{
    set<char> s;
    for (int i = 0; i < a.size(); i++)
    {
        s.insert(a[i]);
    }
    for (int i = 0; i < b.size(); i++)
    {
        s.insert(b[i]);
    }
    string ret;
    for (set<char>::iterator i = s.begin(); i != s.end(); i++)
    {
        ret += *i;
    }
    a = ret;
}

/*递归搜索非终结符的FIRST集合(不获取ε, ε单独处理)*/
string getfirstSet(int charIndex, int &spaceFlag)
{
    // 若字符为空，传入的字符索引能够产生ε字符
    if (charIndex == tokenIndex["@"])
    {
        spaceFlag = 1;
    }

    // 若该字符为终结符, 返回该终结符的FIRST集
    if (charIndex < tokenIndex["@"])
    {
        return firstSet[charIndex];
    }

    // 若传入的字符索引为非终结符, 则递归搜索FIRST集合
    string ret = "";
    for (int i = 0; i < rule.size(); i++)
    {
        if (tokenIndex[rule[i][0]] == charIndex)
        {
            ret += getfirstSet(tokenIndex[rule[i][1]], spaceFlag);
        }
    }
    return ret;
}

// 初始化firstSet
void initfirstSet()
{
    // 将所有的终结符的FIRST集均初始化为本身
    for (int i = 1; i <= tokenIndex['@']; i++)
    {
        firstSet[i] += ('0' + i);
    }

    // 扫描产生式，计算非终结符的FIRST集
    for (int i = 0; i < rule.size(); i++)
    {
        int spaceFlag = 0;
        int cnt = 0;
        do
        {
            spaceFlag = 0;
            cnt++;
            // ε处理
            if (cnt == rule[i].size())
            {
                firstSet[tokenIndex[rule[i][0]]] += ('0' + tokenIndex['@']);
                break;
            }
            mergeStr(firstSet[tokenIndex[rule[i][0]]], getfirstSet(tokenIndex[rule[i][cnt]], spaceFlag));
        } while (spaceFlag);
    }

    // 打印所有字符的FIRST集
    for (int i = 1; i < strlen(tokenList); i++)
    {
        cout << "FIRST(" << tokenList[i] << ") = {";
        for (int j = 0; j < firstSet[i].size(); j++)
        {
            int index = firstSet[i][j] - '0';
            if (j != firstSet[i].size() - 1)
            {
                cout << tokenList[index] << ", ";
            }
            else
            {
                cout << tokenList[index];
            }
        }
        cout << "}" << endl;
    }
}

/*初始化followSet*/
void initfollowSet()
{
    // 默认第一个文法规则(产生式)的左部的未终结符为开始符
    mergeStr(followSet[tokenIndex[rule[0][0]]], "0"); // 开始符添加‘#’
    // 扫描所有的文法规则(产生式), 以此为基础初始化followSet
    for (int i = 0; i < rule.size(); i++)
    {
        for (int j = 1; j < rule[i].size(); j++)
        { // 遍历文法规则(产生式)右部的所有字符
            if (tokenIndex[rule[i][j]] <= tokenIndex['@'])
            { // 若字符为终结符, 则跳过(终结符的FOLLOW集为空)
                continue;
            }

            int cnt = j;
            int flag;
            do
            {
                flag = 0;
                cnt++;
                if (cnt == rule[i].size())
                { // 若该非终结符为产生式右部最后一个字符(U->xA)
                    mergeStr(followSet[tokenIndex[rule[i][j]]], followSet[tokenIndex[rule[i][0]]]);
                    break; // 都到最后一个字符了还不跳出来等啥呢？
                }
                // 若该非终结符不是最后一个字符，则为(U->xAy)
                else
                {
                    int h = firstSet[tokenIndex[rule[i][cnt + 1]]].size() - 1;
                    // 若y的first集里有空
                    if (tokenIndex[firstSet[j][h]] == tokenIndex['@'])
                    {
                        mergeStr(followSet[tokenIndex[rule[i][j]]], followSet[tokenIndex[rule[i][cnt]]]);
                    }
                }
                cnt++;
                mergeStr(followSet[tokenIndex[rule[i][j]]], getfirstSet(tokenIndex[rule[i][cnt]], flag));
            } while (flag); // 直到getfirstSet函数执行结束, 即将非终结符后面的字符串的firstSet加入followSet中
        }
    }
    // 打印所有非终结符的FOLLOW集
    for (int i = tokenIndex['@'] + 1; i < strlen(tokenList); i++)
    {
        cout << "FOLLOW(" << tokenList[i] << ") = {";
        for (int j = 0; j < followSet[i].size(); j++)
        {
            int index = followSet[i][j] - '0';
            if (j != followSet[i].size() - 1)
            {
                cout << tokenList[index] << ", ";
            }
            else
            {
                cout << tokenList[index];
            }
        }
        cout << "}" << endl;
    }
}

// 实现select集 + 初始化分析表
void initAnalyticalTable()
{
    // 初始化分析表
    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < 100; j++)
        {
            analyticalTable[i][j] = {-1};
        }
    }
    // 求出每个文法产生式的select集
    for (int i = 0; i < rule.size(); i++)
    {
        // 若文法规则(产生式)右部为ε(@)
        if (rule[i][1] == '@')
        {
            string sel = followSet[tokenIndex[rule[i][0]]]; // 把当前非终结符的follow集加入select集合
            for (int j = 0; j < sel.size(); j++)
            {
                if (analyticalTable[tokenIndex[rule[i][0]]][sel[j] - '0'] == -1)
                {
                    analyticalTable[tokenIndex[rule[i][0]]][sel[j] - '0'] = i;
                }
                else
                {
                    cout << "不是LL1文法";
                    abort();
                }
            }
        }

        // 若文法规则(产生式)的右部不为空
        // 获取产生式右部的FIRST集
        string sel;
        int flag = 0, cnt = 1;
        while (!flag)
        {
            flag = 1;
            if (cnt == rule[i].size())
            {
                break;
            }
            sel += firstSet[tokenIndex[rule[i][cnt]]];
            for (int j = 0; j < firstSet[tokenIndex[rule[i][cnt]]].size(); j++)
            {
                if (firstSet[tokenIndex[rule[i][cnt]]][j] - '0' == tokenIndex['@'])
                {
                    flag = 0;
                }
            }
            ++cnt;
        }

        for (int j = 0; j < sel.size(); j++)
        { // 判断是否含有空
            if (sel[j] - '0' != tokenIndex['@'])
            {
                analyticalTable[tokenIndex[rule[i][0]]][sel[j] - '0'] = i;
            }
            // 产生式右部的FIRST的最后一个元素是@时, 则证明右部能够==>ε。
            if (j == sel.size() - 1 && sel[j] - '0' == tokenIndex['@'])
            {
                sel += followSet[tokenIndex[rule[i][0]]];
            }
        }
    }
}

// 打印LL分析表
/*打印分析表*/
void print_AnalyticalTable()
{
    cout << "分析表:\n\t|";
    for (int i = 0; i < tokenIndex['@']; i++)
    {
        cout << tokenList[i] << "\t|";
    }
    cout << endl;
    for (int i = tokenIndex['@'] + 1; i < strlen(tokenList); i++)
    {
        cout << tokenList[i] << "\t|";
        for (int j = 0; j < tokenIndex['@']; j++)
        {
            if (analyticalTable[i][j] != -1)
            {
                cout << rule[analyticalTable[i][j]] << "\t|";
            }
            else
            {
                cout << "\t|";
            }
        }
        cout << endl;
    }
    cout << endl;
}

string getAlyStackElement()
{
    // 创建一个临时stack来存储所有元素
    stack<char> temp;
    string ret = "";
    while (!analyticalStack.empty())
    {
        ret += analyticalStack.top();
        temp.push(analyticalStack.top());
        analyticalStack.pop();
    }
    // 输出所有元素，并将它们push回原来的stack中
    while (!temp.empty())
    {
        analyticalStack.push(temp.top());
        temp.pop();
    }
    reverse(ret.begin(), ret.end());
    return ret;
}

string getRemainQueueElement()
{
    // 输出所有元素，并将它们重新push回原来的queue中
    string ret = "";
    int size = remainSymbQueue.size();
    for (int i = 0; i < size; i++)
    {
        ret += remainSymbQueue.front();
        remainSymbQueue.push(remainSymbQueue.front());
        remainSymbQueue.pop();
    }
    return ret;
}

string getProduction(int index)
{
    if (index < 0)
    {
        return "";
    }
    string ret;
    ret += rule[index][0];
    ret += "->";
    for (int i = 1; i < rule[index].size(); i++)
    {
        ret += rule[index][i];
    }
    return ret;
}

bool analyseProcess()
{
    cout << "分析栈\t"
         << "余留符号串\t"
         << "产生式\t"
         << "下一步动作消息" << endl;
    bool res = true;
    // 分析栈初始化
    analyticalStack.push('#');
    analyticalStack.push(rule[0][0]); // 向分析栈中压入初态

    while (!analyticalStack.empty() && res)
    {
        string analyseStr;   // 分析栈字符串
        string remainsStr;   // 余留字符串
        string production;   // 产生式，方便打印过程
        string message = ""; // 消息字符串

        // 获取分析栈栈顶元素
        char top_analyse = analyticalStack.top();
        analyticalStack.pop();

        // 获取余留字符串队列队首元素
        char top_remain = remainSymbQueue.front(); // 不需要弹出（考虑LL(1)文法的分析过程）

        // 如果分析栈栈顶元素是'#'
        if (top_analyse == '#')
        {
            // 若此时余留字符串长度为1,剩下一个'#'时，成功success
            analyseStr = "#";
            remainsStr = getRemainQueueElement();
            production = "";
            if (remainSymbQueue.size() == 1)
            {
                message = "成功！";
                res = true;
            }
            else
            {
                message = "文法分析完毕, 但仍有赘余元素, 分析失败！";
                res = false;
            }
        }
        // 若分析栈栈顶字符与余留字符串队首字符相同，则弹出余留字符串的队首字符
        else if (top_analyse == top_remain)
        {
            analyseStr = getAlyStackElement() + top_analyse;
            remainsStr = getRemainQueueElement();
            remainSymbQueue.pop();
            production = "";
            message += "弹出";
            message += top_analyse;
        }
        // 如果分析站的栈顶元素为非终结符, 寻找相应的产生式并将产生式右部压入分析栈中
        else if (tokenIndex[top_analyse] > tokenIndex['@'] && tokenIndex[top_analyse] < strlen(tokenList))
        {
            analyseStr = getAlyStackElement() + top_analyse;
            remainsStr = getRemainQueueElement();
            // 若分析表中存在对应的产生式
            if (analyticalTable[tokenIndex[top_analyse]][tokenIndex[top_remain]] != -1)
            {
                production = getProduction(analyticalTable[tokenIndex[top_analyse]][tokenIndex[top_remain]]);
                message += "弹出";
                message += top_analyse;
                message += ", ";
                // 逆序压栈
                for (int i = production.size() - 1; i > 2; i--)
                {
                    if (production[i] != '@')
                    {
                        analyticalStack.push(production[i]);
                    }
                    message += production[i];
                }
                message += "进栈";
            }
            // 分析表中存在对应的产生式, 获取产生式，将对应的生成符号压入分析栈中
            else
            {
                production = "";
                message = "!!!语法分析错误, 不存在相应产生式";
                res = false;
            }
        }
        // 如果分析栈栈顶元素为终结符, 且与余留符号串队列队首的字符不同, 语法分析错误！
        else if (tokenIndex[top_analyse] <= tokenIndex['@'] && tokenIndex[top_analyse] > 0)
        {
            analyseStr = getAlyStackElement() + top_analyse;
            remainsStr = getRemainQueueElement() + top_remain;
            production = "";
            message = "!!!语法分析错误";
            res = false;
        }
        else
        {
            analyseStr = getAlyStackElement() + top_analyse;
            remainsStr = getRemainQueueElement() + top_remain;
            production = "";
            message = "未知错误！！！！";
            res = false;
        }
        cout << analyseStr << "\t" << remainsStr << "\t\t" << production << "\t" << message << endl;
    }

    return res;
}

int main()
{
    SetConsoleOutputCP(CP_UTF8); // 设置控制台输出为UTF-8

    input();
    initfirstSet();
    initfollowSet();
    initfollowSet();
    initAnalyticalTable();
    print_AnalyticalTable();

    // 从词法分析器中得到预处理过的字符串
    string str;
    str = AnalyzeCiFaStr();
    cout << str << endl;
    // cout << "请输入需要分析的字符串: ";
    // cin >> str;

    // 构造需分析的余留符号串
    for (int i = 0; i < str.size(); i++)
    {
        remainSymbQueue.push(str[i]);
    }
    remainSymbQueue.push('#');

    // 语义分析过程
    bool res = analyseProcess();
    if (res)
    {
        cout << "语义串分析成功!!" << endl;
    }
    else
    {
        cout << "接收的语义串分析失败!!" << endl;
    }

    system("pause");
    return 0;
}
