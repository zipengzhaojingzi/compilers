// ���ļ�ͷ�����
#include <vector>
#include <string>
#include <stack>
#include <unordered_map>
using namespace std;
#include "lexanalysis.h"

// �����������������ǰ�����Щǰ������
void handleAssignment(TreeNode *node, SymbolTable &symTable);
void handleArithmetic(TreeNode *node, SymbolTable &symTable);
void handleIfStatement(TreeNode *node, SymbolTable &symTable);
void handleWhileStatement(TreeNode *node, SymbolTable &symTable);
void handleFunctionDeclaration(TreeNode *node, SymbolTable &symTable);
void handleFunctionCall(TreeNode *node, SymbolTable &symTable);

// �﷨���ڵ�����
enum NodeType
{
    TERMINAL_NODE,    // �ս���ڵ�
    NONTERMINAL_NODE, // ���ս���ڵ�
    ABSTRACT_NODE     // �����﷨�ڵ�
};

// �﷨�������еķ��ս����ʹ���µı�ŷ�Χ��
enum SyntaxNodeType {
    // ������������2000+��
    VARIABLE_DECLARATION = 2000,
    FUNCTION_DECLARATION = 2001,
    
    // ���ṹ��2100+��
    ASSIGNMENT = 2100,
    IF_STATEMENT = 2101,
    WHILE_STATEMENT = 2102,
    
    // ���ʽ��2200+��
    ADD_EXPRESSION = 2200,
    FUNCTION_CALL = 2201,
    
    // ������أ�2300+��
    STRUCT_DEFINITION = 2300
};

// // �﷨���ڵ�ṹ
// struct TreeNode
// {
//     NodeType type;               // �ڵ�����
//     int symbol;                  // ���ű���
//     string lexeme;               // ����ֵ���ս�����У�
//     string value;                // ����ֵ
//     vector<TreeNode *> children; // �ӽڵ�
//     int line;                    // Դ�����к�

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
        int terminal_symbol;   // ʹ��lexanalysis.h���ֱ��루��type=TERMINAL_NODEʱ��
        int syntax_symbol;     // ʹ��SyntaxNodeType����type=NONTERMINAL_NODEʱ��
    } symbol;
    
    NodeType type;
    string lexeme;
    vector<TreeNode*> children;
    int line;
    
    // ���캯������
    TreeNode(NodeType t, int sym, const string& lex = "", int ln = 0)
        : type(t), lexeme(lex), line(ln) {
        if(t == TERMINAL_NODE) terminal_symbol = sym;
        else syntax_symbol = sym;
    }
};


// �﷨�����ڵ�
TreeNode *syntaxTreeRoot = nullptr;

// // ���������ĳ����﷨��
// TreeNode *buildAST(TreeNode *parseTreeNode)
// {
//     if (!parseTreeNode)
//         return nullptr;

//     // �ս��ֱ�Ӵ���AST�ڵ�
//     if (parseTreeNode->type == TERMINAL_NODE)
//     {
//         return new TreeNode(ABSTRACT_NODE, parseTreeNode->symbol,
//                             parseTreeNode->lexeme, parseTreeNode->line);
//     }

//     // ���ս�����ݾ������ת��
//     TreeNode *astNode = nullptr;
//     switch (parseTreeNode->symbol)
//     {
//     // ��ֵ���: ֻ��Ҫ������ʶ���ͱ��ʽ
//     case ASSIGNMENT:
//         if (parseTreeNode->children.size() >= 3)
//         {
//             astNode = new TreeNode(ABSTRACT_NODE, ASSIGNMENT);
//             astNode->children.push_back(buildAST(parseTreeNode->children[0])); // ID
//             astNode->children.push_back(buildAST(parseTreeNode->children[2])); // Expr
//             astNode->line = parseTreeNode->line;
//         }
//         break;

//     // ���ʽ: ֻ��Ҫ�����������Ͳ�����
//     case ADD_EXPRESSION:
//         if (parseTreeNode->children.size() >= 3)
//         {
//             astNode = new TreeNode(ABSTRACT_NODE, ADD_EXPRESSION);
//             astNode->children.push_back(buildAST(parseTreeNode->children[0])); // Left
//             astNode->children.push_back(buildAST(parseTreeNode->children[2])); // Right
//             astNode->line = parseTreeNode->line;
//         }
//         break;

//     // ��������: �����������Ͳ����б�
//     case FUNCTION_CALL:
//         if (parseTreeNode->children.size() >= 3)
//         {
//             astNode = new TreeNode(ABSTRACT_NODE, FUNCTION_CALL);
//             astNode->children.push_back(buildAST(parseTreeNode->children[0])); // FuncName

//             // �����б�
//             TreeNode *params = new TreeNode(ABSTRACT_NODE, PARAM_LIST);
//             for (int i = 2; i < parseTreeNode->children.size() - 1; i++)
//             {
//                 params->children.push_back(buildAST(parseTreeNode->children[i]));
//             }
//             astNode->children.push_back(params);
//             astNode->line = parseTreeNode->line;
//         }
//         break;

//     // Ĭ�����: ���������ӽڵ�
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

// // �ڷ�����ɺ����
// TreeNode *astRoot = buildAST(syntaxTreeRoot);
// delete syntaxTreeRoot;    // �ͷ�ԭʼ�﷨��
// syntaxTreeRoot = astRoot; // ʹ��AST��Ϊ���ڵ�

// temperory
//  ���ű�����������
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
        enterScope(); // ȫ��������
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
            return false; // �ظ�����
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
        return nullptr; // δ�ҵ�
    }
};

// ��Ԫʽ����
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

// ����ϵͳ��������
string getTypeName(TreeNode *typeNode)
{
    if (!typeNode)
        return "unknown";

    // ��������
    if (typeNode->symbol == INT)
        return "int";
    if (typeNode->symbol == FLOAT)
        return "float";
    if (typeNode->symbol == CHAR)
        return "char";
    if (typeNode->symbol == VOID)
        return "void";

    // ָ������
    if (typeNode->symbol == POINTER)
    {
        if (typeNode->children.size() > 0)
        {
            return getTypeName(typeNode->children[0]) + "*";
        }
        return "void*";
    }

    // �ṹ������
    if (typeNode->symbol == STRUCT_DEF || typeNode->symbol == STRUCT_TYPE)
    {
        if (typeNode->children.size() > 1 && typeNode->children[1]->symbol == IDENTIFIER)
        {
            return "struct " + typeNode->children[1]->lexeme;
        }
        return "struct";
    }

    // ��������
    if (typeNode->symbol == ARRAY_TYPE)
    {
        string baseType = "unknown";
        string arraySize = "";

        // ��ȡ��������
        if (typeNode->children.size() > 0)
        {
            baseType = getTypeName(typeNode->children[0]);
        }

        // ��ȡ�����С
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

    return typeNode->lexeme; // �����Զ���������
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

    // ��ʶ�� - �ӷ��ű��ȡ����
    if (exprNode->symbol == IDENTIFIER)
    {
        Symbol *sym = symTable.findSymbol(exprNode->lexeme);
        return sym ? sym->type : "unknown";
    }

    // ������ - �ƶ�����
    if (exprNode->symbol == INTEGER_LITERAL)
        return "int";
    if (exprNode->symbol == FLOAT_LITERAL)
        return "float";
    if (exprNode->symbol == CHARACTER_LITERAL)
        return "char";
    if (exprNode->symbol == STRING_LITERAL)
        return "char*";

    // ��Ա���ʱ��ʽ
    if (exprNode->symbol == MEMBER_ACCESS)
    {
        if (exprNode->children.size() >= 2)
        {
            // ��ȡ�ṹ������
            string structType = getExpressionType(exprNode->children[0], symTable);

            // ���ҽṹ�嶨��
            StructInfo *structInfo = symTable.findStruct(structType);
            if (!structInfo)
                return "unknown";

            // ��ȡ��Ա����
            string memberName = exprNode->children[1]->lexeme;

            // ���ҳ�Ա����
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

    // �������
    if (exprNode->symbol == ARRAY_ACCESS)
    {
        string arrayType = getExpressionType(exprNode->children[0], symTable);

        // ȥ������ά����Ϣ����ȡԪ������
        size_t pos = arrayType.find('[');
        if (pos != string::npos)
        {
            return arrayType.substr(0, pos);
        }
        return "unknown";
    }

    // �������� - ��ȡ��������
    if (exprNode->symbol == FUNCTION_CALL)
    {
        if (exprNode->children.size() > 0)
        {
            Symbol *sym = symTable.findSymbol(exprNode->children[0]->lexeme);
            return sym ? sym->type : "unknown";
        }
        return "void";
    }

    // ��Ԫ���� - ��������
    if (exprNode->symbol == BINARY_EXPRESSION)
    {
        if (exprNode->children.size() >= 2)
        {
            string leftType = getExpressionType(exprNode->children[0], symTable);
            string rightType = getExpressionType(exprNode->children[2], symTable);

            // ������������
            if (leftType == "double" || rightType == "double")
                return "double";
            if (leftType == "float" || rightType == "float")
                return "float";
            if (leftType == "long" || rightType == "long")
                return "long";
            if (leftType == "int" || rightType == "int")
                return "int";

            return leftType; // Ĭ�Ϸ��������������
        }
    }

    // һԪ����
    if (exprNode->symbol == UNARY_EXPRESSION)
    {
        if (exprNode->children.size() > 1)
        {
            return getExpressionType(exprNode->children[1], symTable);
        }
    }

    // ��ֵ���ʽ
    if (exprNode->symbol == ASSIGNMENT)
    {
        if (exprNode->children.size() > 0)
        {
            return getExpressionType(exprNode->children[0], symTable);
        }
    }

    // Ĭ�Ϸ��ص�һ���ӽڵ������
    if (!exprNode->children.empty())
    {
        return getExpressionType(exprNode->children[0], symTable);
    }

    return "unknown";
}

// ��ȡ��ʶ�������ͣ�ͨ�����ű�
string getType(const string &identifier, SymbolTable &symTable)
{
    Symbol *sym = symTable.findSymbol(identifier);
    return sym ? sym->type : "unknown";
}

// ������
void error(const string &message, int line)
{
    cerr << "������� [�� " << line << "]: " << message << endl;
    // �����ռ����д�������������˳�
}

// �ṹ����Ϣ
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

// ��ǿ���ű���
class SymbolTable
{
    vector<unordered_map<string, Symbol>> scopes;
    unordered_map<string, StructInfo> structs;
    int currentScope = 0;

public:
    SymbolTable()
    {
        enterScope(); // ȫ��������
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
            return false; // �ظ�����
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
        return nullptr; // δ�ҵ�
    }

    // ��ӽṹ�嶨��
    bool addStruct(const string &name, const vector<StructMember> &members)
    {
        if (structs.find(name) != structs.end())
        {
            return false; // �ظ�����
        }
        structs[name] = {name, members};
        return true;
    }

    // ���ҽṹ�嶨��
    StructInfo *findStruct(const string &name)
    {
        auto it = structs.find(name);
        if (it != structs.end())
        {
            return &(it->second);
        }
        return nullptr;
    }

    // ����ṹ�嶨��
    void handleStructDefinition(TreeNode *node)
    {
        if (node->symbol != STRUCT_DEF || node->children.size() < 4)
            return;

        // ��ȡ�ṹ������
        string structName = "";
        if (node->children[1]->symbol == IDENTIFIER)
        {
            structName = node->children[1]->lexeme;
        }

        // ������Ա�б�
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

        // ��ӵ��ṹ���
        if (!structName.empty())
        {
            addStruct(structName, members);
        }
    }
};

// ���������������ǿ�棩
void handleDeclaration(TreeNode *node, SymbolTable &symTable)
{
    if (node->children.size() < 2)
        return;

    // ��ȡ���ͽڵ�
    TreeNode *typeNode = node->children[0];
    string type = getTypeName(typeNode);

    // �����������б�
    for (int i = 1; i < node->children.size(); i++)
    {
        TreeNode *decl = node->children[i];

        if (decl->symbol == VARIABLE_DECLARATOR)
        {
            // ��������
            string name = decl->children[0]->lexeme;

            // ������������
            string fullType = type;
            if (decl->children.size() > 1 && decl->children[1]->symbol == ARRAY_SIZE)
            {
                fullType += "[" + decl->children[1]->children[0]->lexeme + "]";
            }

            if (!symTable.addSymbol(name, fullType))
            {
                error("�ظ�����ı���: " + name, node->line);
            }

            // �����ʼ��
            if (decl->children.size() > 2 && decl->children[2]->symbol == '=')
            {
                semanticAnalysis(decl->children[3], symTable);
                emit("=", decl->children[3]->value, "", name);
            }
        }
        else if (decl->symbol == FUNCTION_DECLARATOR)
        {
            // ��������
            string name = decl->children[0]->lexeme;
            string returnType = type;

            // ������������
            if (!symTable.addSymbol(name, returnType))
            {
                error("�ظ�����ĺ���: " + name, node->line);
            }

            // ���뺯��������
            symTable.enterScope();

            // �������
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

            // ��������
            if (decl->children.size() > 2 && decl->children[2]->symbol == FUNCTION_BODY)
            {
                semanticAnalysis(decl->children[2], symTable);
            }

            // �˳�����������
            symTable.exitScope();
        }
    }
}

// ����ṹ�嶨��
void handleStructDefinition(TreeNode *node, SymbolTable &symTable)
{
    symTable.handleStructDefinition(node);
}

// ��ǿ�������������
void semanticAnalysis(TreeNode *node, SymbolTable &symTable)
{
    if (!node)
        return;

    // ���ݽڵ����ʹ���
    switch (node->symbol)
    {
    case STRUCT_DEF:
        handleStructDefinition(node, symTable);
        break;
    case VARIABLE_DECLARATION:
        handleDeclaration(node, symTable);
        break;
    case FUNCTION_DECLARATION:
        handleDeclaration(node, symTable); // ������������
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
        // �ݹ鴦���ӽڵ�
        for (TreeNode *child : node->children)
        {
            semanticAnalysis(child, symTable);
        }
    }
}

// �����Ա����
void handleMemberAccess(TreeNode *node, SymbolTable &symTable)
{
    if (node->children.size() < 2)
        return;

    // ����������ʽ
    semanticAnalysis(node->children[0], symTable);
    string object = node->children[0]->value;

    // ��ȡ��Ա����
    string memberName = node->children[1]->lexeme;

    // ��ȡ��������
    string objectType = getExpressionType(node->children[0], symTable);

    // ���ҽṹ�嶨��
    StructInfo *structInfo = symTable.findStruct(objectType);
    if (!structInfo)
    {
        error("δ���������: " + objectType, node->line);
        return;
    }

    // ���ҳ�Աƫ��
    int offset = 0;
    bool found = false;
    for (auto &member : structInfo->members)
    {
        if (member.name == memberName)
        {
            found = true;
            break;
        }
        offset++; // �򻯴���ʵ����Ҫ�����ֽ�ƫ��
    }

    if (!found)
    {
        error("�ṹ�� " + objectType + " û�г�Ա " + memberName, node->line);
        return;
    }

    // ���ɳ�Ա���ʴ���
    string temp = newTemp();
    emit("member", object, to_string(offset), temp);
    node->value = temp;
}

// ����������
void handleFunctionCall(TreeNode *node, SymbolTable &symTable)
{
    if (node->children.size() < 1)
        return;

    string funcName = node->children[0]->lexeme;

    // ��麯���Ƿ����
    Symbol *funcSym = symTable.findSymbol(funcName);
    if (!funcSym)
    {
        error("δ����ĺ���: " + funcName, node->line);
        return;
    }

    // �������
    vector<string> args;
    if (node->children.size() > 1 && node->children[1]->symbol == ARGUMENT_LIST)
    {
        for (TreeNode *arg : node->children[1]->children)
        {
            semanticAnalysis(arg, symTable);
            args.push_back(arg->value);
        }
    }

    // ���ɺ������ô���
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

// �������������
// void semanticAnalysis(TreeNode *node, SymbolTable &symTable)
// {
//     if (!node)
//         return;

//     // ���ݽڵ����ʹ���
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
//         // �ݹ鴦���ӽڵ�
//         for (TreeNode *child : node->children)
//         {
//             semanticAnalysis(child, symTable);
//         }
//     }
// }

// // �����������
// void handleDeclaration(TreeNode *node, SymbolTable &symTable)
// {
//     if (node->children.size() < 2)
//         return;

//     string type = getTypeName(node->children[0]);
//     string name = node->children[1]->lexeme;

//     if (!symTable.addSymbol(name, type))
//     {
//         error("�ظ�����ı���: " + name, node->line);
//     }

//     // ����г�ʼ�����ʽ
//     if (node->children.size() > 3 && node->children[2]->symbol == '=')
//     {
//         semanticAnalysis(node->children[3], symTable);
//         emit("=", node->children[3]->value, "", name);
//     }
// }

// ����ֵ���
void handleAssignment(TreeNode *node, SymbolTable &symTable)
{
    if (node->children.size() < 3)
        return;

    string name = node->children[0]->lexeme;
    Symbol *sym = symTable.findSymbol(name);

    if (!sym)
    {
        error("δ����ı���: " + name, node->line);
        return;
    }

    semanticAnalysis(node->children[1], symTable);
    string exprValue = node->children[1]->value;

    // ���ͼ��
    if (sym->type != getExpressionType(node->children[1], symTable))
    {
        error("���Ͳ�ƥ��: " + sym->type + " �� " + getExpressionType(node->children[1], symTable), node->line);
    }

    emit("=", exprValue, "", name);
    node->value = name;
}

// ������������
void handleArithmetic(TreeNode *node, SymbolTable &symTable)
{
    if (node->children.size() < 3)
        return;

    semanticAnalysis(node->children[0], symTable);
    semanticAnalysis(node->children[2], symTable);

    string left = node->children[0]->value;
    string right = node->children[2]->value;
    string op = getOperator(node->children[1]);

    // ���ͼ��
    if (getType(left, symTable) != getType(right, symTable))
    {
        error("���������Ͳ�ƥ��", node->line);
    }

    string temp = newTemp();
    emit(op, left, right, temp);
    node->value = temp;
}

// ����if���
void handleIfStatement(TreeNode *node, SymbolTable &symTable)
{
    if (node->children.size() < 5)
        return;

    // �������ʽ
    semanticAnalysis(node->children[2], symTable);
    string cond = node->children[2]->value;

    string elseLabel = newLabel();
    string endLabel = newLabel();

    // ������ת
    emit("jz", cond, "", elseLabel);

    // then��
    symTable.enterScope();
    semanticAnalysis(node->children[4], symTable);
    symTable.exitScope();

    // ����else��
    emit("jmp", "", "", endLabel);

    // else��
    emit("label", elseLabel, "", "");
    if (node->children.size() > 6)
    {
        symTable.enterScope();
        semanticAnalysis(node->children[6], symTable);
        symTable.exitScope();
    }

    // ������ǩ
    emit("label", endLabel, "", "");
}