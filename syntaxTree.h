// 在文件头部添加
#include <vector>
#include <string>
#include <stack>
#include <unordered_map>
using namespace std;
#include "lexanalysis.h"

// 在语义分析函数声明前添加这些前置声明
void handleAssignment(TreeNode *node, SymbolTable &symTable);
void handleArithmetic(TreeNode *node, SymbolTable &symTable);
void handleIfStatement(TreeNode *node, SymbolTable &symTable);
void handleWhileStatement(TreeNode *node, SymbolTable &symTable);
void handleFunctionDeclaration(TreeNode *node, SymbolTable &symTable);
void handleFunctionCall(TreeNode *node, SymbolTable &symTable);

// 语法树节点类型
enum NodeType
{
    TERMINAL_NODE,    // 终结符节点
    NONTERMINAL_NODE, // 非终结符节点
    ABSTRACT_NODE     // 抽象语法节点
};

// 语法分析特有的非终结符（使用新的编号范围）
enum SyntaxNodeType {
    // 变量与声明（2000+）
    VARIABLE_DECLARATION = 2000,
    FUNCTION_DECLARATION = 2001,
    
    // 语句结构（2100+）
    ASSIGNMENT = 2100,
    IF_STATEMENT = 2101,
    WHILE_STATEMENT = 2102,
    
    // 表达式（2200+）
    ADD_EXPRESSION = 2200,
    FUNCTION_CALL = 2201,
    
    // 类型相关（2300+）
    STRUCT_DEFINITION = 2300
};

// // 语法树节点结构
// struct TreeNode
// {
//     NodeType type;               // 节点类型
//     int symbol;                  // 符号编码
//     string lexeme;               // 词素值（终结符才有）
//     string value;                // 语义值
//     vector<TreeNode *> children; // 子节点
//     int line;                    // 源代码行号

//     TreeNode(NodeType t, int sym, const string &lex = "", int ln = 0)
//         : type(t), symbol(sym), lexeme(lex), line(ln) {}

//     ~TreeNode()
//     {
//         for (auto child : children)
//         {
//             delete child;
//         }
//     }
// };


struct TreeNode {
    union {
        int terminal_symbol;   // 使用lexanalysis.h的种别码（当type=TERMINAL_NODE时）
        int syntax_symbol;     // 使用SyntaxNodeType（当type=NONTERMINAL_NODE时）
    } symbol;
    
    NodeType type;
    string lexeme;
    vector<TreeNode*> children;
    int line;
    
    // 构造函数重载
    TreeNode(NodeType t, int sym, const string& lex = "", int ln = 0)
        : type(t), lexeme(lex), line(ln) {
        if(t == TERMINAL_NODE) terminal_symbol = sym;
        else syntax_symbol = sym;
    }
};


// 语法树根节点
TreeNode *syntaxTreeRoot = nullptr;

// // 构建更简洁的抽象语法树
// TreeNode *buildAST(TreeNode *parseTreeNode)
// {
//     if (!parseTreeNode)
//         return nullptr;

//     // 终结符直接创建AST节点
//     if (parseTreeNode->type == TERMINAL_NODE)
//     {
//         return new TreeNode(ABSTRACT_NODE, parseTreeNode->symbol,
//                             parseTreeNode->lexeme, parseTreeNode->line);
//     }

//     // 非终结符根据具体规则转换
//     TreeNode *astNode = nullptr;
//     switch (parseTreeNode->symbol)
//     {
//     // 赋值语句: 只需要保留标识符和表达式
//     case ASSIGNMENT:
//         if (parseTreeNode->children.size() >= 3)
//         {
//             astNode = new TreeNode(ABSTRACT_NODE, ASSIGNMENT);
//             astNode->children.push_back(buildAST(parseTreeNode->children[0])); // ID
//             astNode->children.push_back(buildAST(parseTreeNode->children[2])); // Expr
//             astNode->line = parseTreeNode->line;
//         }
//         break;

//     // 表达式: 只需要保留操作符和操作数
//     case ADD_EXPRESSION:
//         if (parseTreeNode->children.size() >= 3)
//         {
//             astNode = new TreeNode(ABSTRACT_NODE, ADD_EXPRESSION);
//             astNode->children.push_back(buildAST(parseTreeNode->children[0])); // Left
//             astNode->children.push_back(buildAST(parseTreeNode->children[2])); // Right
//             astNode->line = parseTreeNode->line;
//         }
//         break;

//     // 函数调用: 保留函数名和参数列表
//     case FUNCTION_CALL:
//         if (parseTreeNode->children.size() >= 3)
//         {
//             astNode = new TreeNode(ABSTRACT_NODE, FUNCTION_CALL);
//             astNode->children.push_back(buildAST(parseTreeNode->children[0])); // FuncName

//             // 参数列表
//             TreeNode *params = new TreeNode(ABSTRACT_NODE, PARAM_LIST);
//             for (int i = 2; i < parseTreeNode->children.size() - 1; i++)
//             {
//                 params->children.push_back(buildAST(parseTreeNode->children[i]));
//             }
//             astNode->children.push_back(params);
//             astNode->line = parseTreeNode->line;
//         }
//         break;

//     // 默认情况: 保留所有子节点
//     default:
//         astNode = new TreeNode(ABSTRACT_NODE, parseTreeNode->symbol);
//         for (TreeNode *child : parseTreeNode->children)
//         {
//             astNode->children.push_back(buildAST(child));
//         }
//         astNode->line = parseTreeNode->line;
//     }

//     return astNode;
// }

// // 在分析完成后调用
// TreeNode *astRoot = buildAST(syntaxTreeRoot);
// delete syntaxTreeRoot;    // 释放原始语法树
// syntaxTreeRoot = astRoot; // 使用AST作为根节点

// temperory
//  符号表和作用域管理
struct Symbol
{
    string name;
    string type;
    bool isInitialized;
    int scopeLevel;
};

class SymbolTable
{
    vector<unordered_map<string, Symbol>> scopes;
    int currentScope = 0;

public:
    SymbolTable()
    {
        enterScope(); // 全局作用域
    }

    void enterScope()
    {
        scopes.push_back(unordered_map<string, Symbol>());
        currentScope++;
    }

    void exitScope()
    {
        if (scopes.size() > 1)
        {
            scopes.pop_back();
            currentScope--;
        }
    }

    bool addSymbol(const string &name, const string &type)
    {
        if (scopes.back().find(name) != scopes.back().end())
        {
            return false; // 重复定义
        }
        scopes.back()[name] = {name, type, false, currentScope};
        return true;
    }

    Symbol *findSymbol(const string &name)
    {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
        {
            auto entry = it->find(name);
            if (entry != it->end())
            {
                return &(entry->second);
            }
        }
        return nullptr; // 未找到
    }
};

// 四元式生成
vector<Quadruple> quadList;
int tempCount = 0;
int labelCount = 0;

string newTemp()
{
    return "t" + to_string(++tempCount);
}

string newLabel()
{
    return "L" + to_string(++labelCount);
}

void emit(const string &op, const string &arg1, const string &arg2, const string &result)
{
    quadList.push_back({op, arg1, arg2, result});
}

// 类型系统辅助函数
string getTypeName(TreeNode *typeNode)
{
    if (!typeNode)
        return "unknown";

    // 基本类型
    if (typeNode->symbol == INT)
        return "int";
    if (typeNode->symbol == FLOAT)
        return "float";
    if (typeNode->symbol == CHAR)
        return "char";
    if (typeNode->symbol == VOID)
        return "void";

    // 指针类型
    if (typeNode->symbol == POINTER)
    {
        if (typeNode->children.size() > 0)
        {
            return getTypeName(typeNode->children[0]) + "*";
        }
        return "void*";
    }

    // 结构体类型
    if (typeNode->symbol == STRUCT_DEF || typeNode->symbol == STRUCT_TYPE)
    {
        if (typeNode->children.size() > 1 && typeNode->children[1]->symbol == IDENTIFIER)
        {
            return "struct " + typeNode->children[1]->lexeme;
        }
        return "struct";
    }

    // 数组类型
    if (typeNode->symbol == ARRAY_TYPE)
    {
        string baseType = "unknown";
        string arraySize = "";

        // 获取基本类型
        if (typeNode->children.size() > 0)
        {
            baseType = getTypeName(typeNode->children[0]);
        }

        // 获取数组大小
        if (typeNode->children.size() > 1 &&
            node->children[1]->symbol == ARRAY_SIZE &&
            node->children[1]->children.size() > 0)
        {
            arraySize = "[" + node->children[1]->children[0]->lexeme + "]";
        }
        else
        {
            arraySize = "[]";
        }

        return baseType + arraySize;
    }

    return typeNode->lexeme; // 对于自定义类型名
}

string getOperator(TreeNode *opNode)
{
    if (!opNode)
        return "?";

    switch (opNode->symbol)
    {
    case '+':
        return "add";
    case '-':
        return "sub";
    case '*':
        return "mul";
    case '/':
        return "div";
    case '%':
        return "mod";
    case '<':
        return "lt";
    case '>':
        return "gt";
    case LE:
        return "le"; // <=
    case GE:
        return "ge"; // >=
    case EQ:
        return "eq"; // ==
    case NE:
        return "ne"; // !=
    case AND:
        return "and"; // &&
    case OR:
        return "or"; // ||
    default:
        return opNode->lexeme;
    }
}

string getExpressionType(TreeNode *exprNode, SymbolTable &symTable)
{
    if (!exprNode)
        return "void";

    // 标识符 - 从符号表获取类型
    if (exprNode->symbol == IDENTIFIER)
    {
        Symbol *sym = symTable.findSymbol(exprNode->lexeme);
        return sym ? sym->type : "unknown";
    }

    // 字面量 - 推断类型
    if (exprNode->symbol == INTEGER_LITERAL)
        return "int";
    if (exprNode->symbol == FLOAT_LITERAL)
        return "float";
    if (exprNode->symbol == CHARACTER_LITERAL)
        return "char";
    if (exprNode->symbol == STRING_LITERAL)
        return "char*";

    // 成员访问表达式
    if (exprNode->symbol == MEMBER_ACCESS)
    {
        if (exprNode->children.size() >= 2)
        {
            // 获取结构体类型
            string structType = getExpressionType(exprNode->children[0], symTable);

            // 查找结构体定义
            StructInfo *structInfo = symTable.findStruct(structType);
            if (!structInfo)
                return "unknown";

            // 获取成员名称
            string memberName = exprNode->children[1]->lexeme;

            // 查找成员类型
            for (auto &member : structInfo->members)
            {
                if (member.name == memberName)
                {
                    return member.type;
                }
            }
        }
        return "unknown";
    }

    // 数组访问
    if (exprNode->symbol == ARRAY_ACCESS)
    {
        string arrayType = getExpressionType(exprNode->children[0], symTable);

        // 去掉数组维度信息，获取元素类型
        size_t pos = arrayType.find('[');
        if (pos != string::npos)
        {
            return arrayType.substr(0, pos);
        }
        return "unknown";
    }

    // 函数调用 - 获取返回类型
    if (exprNode->symbol == FUNCTION_CALL)
    {
        if (exprNode->children.size() > 0)
        {
            Symbol *sym = symTable.findSymbol(exprNode->children[0]->lexeme);
            return sym ? sym->type : "unknown";
        }
        return "void";
    }

    // 二元操作 - 类型提升
    if (exprNode->symbol == BINARY_EXPRESSION)
    {
        if (exprNode->children.size() >= 2)
        {
            string leftType = getExpressionType(exprNode->children[0], symTable);
            string rightType = getExpressionType(exprNode->children[2], symTable);

            // 类型提升规则
            if (leftType == "double" || rightType == "double")
                return "double";
            if (leftType == "float" || rightType == "float")
                return "float";
            if (leftType == "long" || rightType == "long")
                return "long";
            if (leftType == "int" || rightType == "int")
                return "int";

            return leftType; // 默认返回左操作数类型
        }
    }

    // 一元操作
    if (exprNode->symbol == UNARY_EXPRESSION)
    {
        if (exprNode->children.size() > 1)
        {
            return getExpressionType(exprNode->children[1], symTable);
        }
    }

    // 赋值表达式
    if (exprNode->symbol == ASSIGNMENT)
    {
        if (exprNode->children.size() > 0)
        {
            return getExpressionType(exprNode->children[0], symTable);
        }
    }

    // 默认返回第一个子节点的类型
    if (!exprNode->children.empty())
    {
        return getExpressionType(exprNode->children[0], symTable);
    }

    return "unknown";
}

// 获取标识符的类型（通过符号表）
string getType(const string &identifier, SymbolTable &symTable)
{
    Symbol *sym = symTable.findSymbol(identifier);
    return sym ? sym->type : "unknown";
}

// 错误处理
void error(const string &message, int line)
{
    cerr << "语义错误 [行 " << line << "]: " << message << endl;
    // 可以收集所有错误而不是立即退出
}

// 结构体信息
struct StructMember
{
    string name;
    string type;
};

struct StructInfo
{
    string name;
    vector<StructMember> members;
};

// 增强符号表类
class SymbolTable
{
    vector<unordered_map<string, Symbol>> scopes;
    unordered_map<string, StructInfo> structs;
    int currentScope = 0;

public:
    SymbolTable()
    {
        enterScope(); // 全局作用域
    }

    void enterScope()
    {
        scopes.push_back(unordered_map<string, Symbol>());
        currentScope++;
    }

    void exitScope()
    {
        if (scopes.size() > 1)
        {
            scopes.pop_back();
            currentScope--;
        }
    }

    bool addSymbol(const string &name, const string &type)
    {
        if (scopes.back().find(name) != scopes.back().end())
        {
            return false; // 重复定义
        }
        scopes.back()[name] = {name, type, false, currentScope};
        return true;
    }

    Symbol *findSymbol(const string &name)
    {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
        {
            auto entry = it->find(name);
            if (entry != it->end())
            {
                return &(entry->second);
            }
        }
        return nullptr; // 未找到
    }

    // 添加结构体定义
    bool addStruct(const string &name, const vector<StructMember> &members)
    {
        if (structs.find(name) != structs.end())
        {
            return false; // 重复定义
        }
        structs[name] = {name, members};
        return true;
    }

    // 查找结构体定义
    StructInfo *findStruct(const string &name)
    {
        auto it = structs.find(name);
        if (it != structs.end())
        {
            return &(it->second);
        }
        return nullptr;
    }

    // 处理结构体定义
    void handleStructDefinition(TreeNode *node)
    {
        if (node->symbol != STRUCT_DEF || node->children.size() < 4)
            return;

        // 获取结构体名称
        string structName = "";
        if (node->children[1]->symbol == IDENTIFIER)
        {
            structName = node->children[1]->lexeme;
        }

        // 解析成员列表
        vector<StructMember> members;
        TreeNode *memberList = node->children[3];
        for (TreeNode *member : memberList->children)
        {
            if (member->symbol == MEMBER_DECLARATION && member->children.size() >= 2)
            {
                string type = getTypeName(member->children[0]);
                string name = member->children[1]->lexeme;
                members.push_back({name, type});
            }
        }

        // 添加到结构体表
        if (!structName.empty())
        {
            addStruct(structName, members);
        }
    }
};

// 处理变量声明（增强版）
void handleDeclaration(TreeNode *node, SymbolTable &symTable)
{
    if (node->children.size() < 2)
        return;

    // 获取类型节点
    TreeNode *typeNode = node->children[0];
    string type = getTypeName(typeNode);

    // 处理声明符列表
    for (int i = 1; i < node->children.size(); i++)
    {
        TreeNode *decl = node->children[i];

        if (decl->symbol == VARIABLE_DECLARATOR)
        {
            // 变量声明
            string name = decl->children[0]->lexeme;

            // 处理数组声明
            string fullType = type;
            if (decl->children.size() > 1 && decl->children[1]->symbol == ARRAY_SIZE)
            {
                fullType += "[" + decl->children[1]->children[0]->lexeme + "]";
            }

            if (!symTable.addSymbol(name, fullType))
            {
                error("重复定义的变量: " + name, node->line);
            }

            // 处理初始化
            if (decl->children.size() > 2 && decl->children[2]->symbol == '=')
            {
                semanticAnalysis(decl->children[3], symTable);
                emit("=", decl->children[3]->value, "", name);
            }
        }
        else if (decl->symbol == FUNCTION_DECLARATOR)
        {
            // 函数声明
            string name = decl->children[0]->lexeme;
            string returnType = type;

            // 创建函数符号
            if (!symTable.addSymbol(name, returnType))
            {
                error("重复定义的函数: " + name, node->line);
            }

            // 进入函数作用域
            symTable.enterScope();

            // 处理参数
            if (decl->children.size() > 1 && decl->children[1]->symbol == PARAMETER_LIST)
            {
                for (TreeNode *param : decl->children[1]->children)
                {
                    if (param->symbol == PARAMETER && param->children.size() >= 2)
                    {
                        string paramType = getTypeName(param->children[0]);
                        string paramName = param->children[1]->lexeme;
                        symTable.addSymbol(paramName, paramType);
                    }
                }
            }

            // 处理函数体
            if (decl->children.size() > 2 && decl->children[2]->symbol == FUNCTION_BODY)
            {
                semanticAnalysis(decl->children[2], symTable);
            }

            // 退出函数作用域
            symTable.exitScope();
        }
    }
}

// 处理结构体定义
void handleStructDefinition(TreeNode *node, SymbolTable &symTable)
{
    symTable.handleStructDefinition(node);
}

// 增强语义分析主函数
void semanticAnalysis(TreeNode *node, SymbolTable &symTable)
{
    if (!node)
        return;

    // 根据节点类型处理
    switch (node->symbol)
    {
    case STRUCT_DEF:
        handleStructDefinition(node, symTable);
        break;
    case VARIABLE_DECLARATION:
        handleDeclaration(node, symTable);
        break;
    case FUNCTION_DECLARATION:
        handleDeclaration(node, symTable); // 复用声明处理
        break;
    case ASSIGNMENT:
        handleAssignment(node, symTable);
        break;
    case ADD_EXPRESSION:
    case SUB_EXPRESSION:
    case MUL_EXPRESSION:
    case DIV_EXPRESSION:
        handleArithmetic(node, symTable);
        break;
    case IF_STATEMENT:
        handleIfStatement(node, symTable);
        break;
    case WHILE_STATEMENT:
        handleWhileStatement(node, symTable);
        break;
    case FUNCTION_CALL:
        handleFunctionCall(node, symTable);
        break;
    case MEMBER_ACCESS:
        handleMemberAccess(node, symTable);
        break;
    default:
        // 递归处理子节点
        for (TreeNode *child : node->children)
        {
            semanticAnalysis(child, symTable);
        }
    }
}

// 处理成员访问
void handleMemberAccess(TreeNode *node, SymbolTable &symTable)
{
    if (node->children.size() < 2)
        return;

    // 分析对象表达式
    semanticAnalysis(node->children[0], symTable);
    string object = node->children[0]->value;

    // 获取成员名称
    string memberName = node->children[1]->lexeme;

    // 获取对象类型
    string objectType = getExpressionType(node->children[0], symTable);

    // 查找结构体定义
    StructInfo *structInfo = symTable.findStruct(objectType);
    if (!structInfo)
    {
        error("未定义的类型: " + objectType, node->line);
        return;
    }

    // 查找成员偏移
    int offset = 0;
    bool found = false;
    for (auto &member : structInfo->members)
    {
        if (member.name == memberName)
        {
            found = true;
            break;
        }
        offset++; // 简化处理，实际需要计算字节偏移
    }

    if (!found)
    {
        error("结构体 " + objectType + " 没有成员 " + memberName, node->line);
        return;
    }

    // 生成成员访问代码
    string temp = newTemp();
    emit("member", object, to_string(offset), temp);
    node->value = temp;
}

// 处理函数调用
void handleFunctionCall(TreeNode *node, SymbolTable &symTable)
{
    if (node->children.size() < 1)
        return;

    string funcName = node->children[0]->lexeme;

    // 检查函数是否存在
    Symbol *funcSym = symTable.findSymbol(funcName);
    if (!funcSym)
    {
        error("未定义的函数: " + funcName, node->line);
        return;
    }

    // 处理参数
    vector<string> args;
    if (node->children.size() > 1 && node->children[1]->symbol == ARGUMENT_LIST)
    {
        for (TreeNode *arg : node->children[1]->children)
        {
            semanticAnalysis(arg, symTable);
            args.push_back(arg->value);
        }
    }

    // 生成函数调用代码
    if (funcSym->type != "void")
    {
        string result = newTemp();
        emit("call", funcName, to_string(args.size()), result);
        for (string arg : args)
        {
            emit("param", arg, "", "");
        }
        node->value = result;
    }
    else
    {
        emit("call", funcName, to_string(args.size()), "");
        for (string arg : args)
        {
            emit("param", arg, "", "");
        }
        node->value = "";
    }
}

// 语义分析主函数
// void semanticAnalysis(TreeNode *node, SymbolTable &symTable)
// {
//     if (!node)
//         return;

//     // 根据节点类型处理
//     switch (node->symbol)
//     {
//     case VARIABLE_DECLARATION:
//         handleDeclaration(node, symTable);
//         break;
//     case ASSIGNMENT:
//         handleAssignment(node, symTable);
//         break;
//     case ADD_EXPRESSION:
//         handleArithmetic(node, symTable);
//         break;
//     case IF_STATEMENT:
//         handleIfStatement(node, symTable);
//         break;
//     case WHILE_STATEMENT:
//         handleWhileStatement(node, symTable);
//         break;
//     case FUNCTION_DECLARATION:
//         handleFunctionDeclaration(node, symTable);
//         break;
//     case FUNCTION_CALL:
//         handleFunctionCall(node, symTable);
//         break;
//     default:
//         // 递归处理子节点
//         for (TreeNode *child : node->children)
//         {
//             semanticAnalysis(child, symTable);
//         }
//     }
// }

// // 处理变量声明
// void handleDeclaration(TreeNode *node, SymbolTable &symTable)
// {
//     if (node->children.size() < 2)
//         return;

//     string type = getTypeName(node->children[0]);
//     string name = node->children[1]->lexeme;

//     if (!symTable.addSymbol(name, type))
//     {
//         error("重复定义的变量: " + name, node->line);
//     }

//     // 如果有初始化表达式
//     if (node->children.size() > 3 && node->children[2]->symbol == '=')
//     {
//         semanticAnalysis(node->children[3], symTable);
//         emit("=", node->children[3]->value, "", name);
//     }
// }

// 处理赋值语句
void handleAssignment(TreeNode *node, SymbolTable &symTable)
{
    if (node->children.size() < 3)
        return;

    string name = node->children[0]->lexeme;
    Symbol *sym = symTable.findSymbol(name);

    if (!sym)
    {
        error("未定义的变量: " + name, node->line);
        return;
    }

    semanticAnalysis(node->children[1], symTable);
    string exprValue = node->children[1]->value;

    // 类型检查
    if (sym->type != getExpressionType(node->children[1], symTable))
    {
        error("类型不匹配: " + sym->type + " 和 " + getExpressionType(node->children[1], symTable), node->line);
    }

    emit("=", exprValue, "", name);
    node->value = name;
}

// 处理算术运算
void handleArithmetic(TreeNode *node, SymbolTable &symTable)
{
    if (node->children.size() < 3)
        return;

    semanticAnalysis(node->children[0], symTable);
    semanticAnalysis(node->children[2], symTable);

    string left = node->children[0]->value;
    string right = node->children[2]->value;
    string op = getOperator(node->children[1]);

    // 类型检查
    if (getType(left, symTable) != getType(right, symTable))
    {
        error("操作数类型不匹配", node->line);
    }

    string temp = newTemp();
    emit(op, left, right, temp);
    node->value = temp;
}

// 处理if语句
void handleIfStatement(TreeNode *node, SymbolTable &symTable)
{
    if (node->children.size() < 5)
        return;

    // 条件表达式
    semanticAnalysis(node->children[2], symTable);
    string cond = node->children[2]->value;

    string elseLabel = newLabel();
    string endLabel = newLabel();

    // 条件跳转
    emit("jz", cond, "", elseLabel);

    // then块
    symTable.enterScope();
    semanticAnalysis(node->children[4], symTable);
    symTable.exitScope();

    // 跳过else块
    emit("jmp", "", "", endLabel);

    // else块
    emit("label", elseLabel, "", "");
    if (node->children.size() > 6)
    {
        symTable.enterScope();
        semanticAnalysis(node->children[6], symTable);
        symTable.exitScope();
    }

    // 结束标签
    emit("label", endLabel, "", "");
}