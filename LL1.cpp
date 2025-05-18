#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include "CiFaAnalysis.cpp"


// 词法单元类型枚举
enum TokenType
{
    STRUCT,
    ID,
    LBRACE,
    RBRACE,
    SEMI,
    INT,
    FLOAT,
    CHAR,
    STAR,
    VOID,
    MAIN,
    LPAREN,
    RPAREN,
    LBRACKET,
    RBRACKET,
    COMMA,
    ASSIGN,
    IF,
    ELSE,
    PRINTF,
    NUMBER,
    FLOATVAL,
    STRING,
    DOT,
    LESS,
    GREATER,
    EOF_
};

// 词法单元结构体
struct Token
{
    TokenType type;
    std::string value;
    int line;
};

TokenType mapTokenType(int syn) {
    switch(syn) {
        case 1: return ID;      // 标识符
        case 2: return LBRACE;  // 界符
        case 3: return LESS;    // 运算符
        case 4: return STRUCT;  // 关键字
        // ...其他类型映射
        default: throw std::runtime_error("Unknown token");
    }
}

// 词法分析器
class Lexer
{
    // 实现词法分析的逻辑...
     std::vector<Token> tokens;
    size_t pos = 0;
public:
    explicit Lexer(const std::string& source) {
        std::string tokenStream = AnalyzeCiFaStr(); // 调用词法分析
        // 解析tokenStream字符串为Token对象（需实现解析逻辑）
        // 示例格式：STRUCT(1) ID(x) LBRACE(2)...
    }

    Token next_token() {
        if (pos < tokens.size()) {
            return tokens[pos++];
        }
        return {EOF_, "", -1}; // 返回结束标记
    }
};

// 语法分析器
class Parser
{
    Lexer &lexer;
    Token lookahead;

    void match(TokenType expected)
    {
        if (lookahead.type == expected)
        {
            lookahead = lexer.next_token();
        }
        else
        {
            throw std::runtime_error("Syntax error");
        }
    }

public:
    Parser(Lexer &l) : lexer(l)
    {
        lookahead = lexer.next_token();
    }
 void parse_main_function(); // 新增声明
void Parser::parse_main_function() {
    match(MAIN);
    match(LPAREN);
    match(RPAREN);
    match(LBRACE);
    // 解析函数体（示例）
    while (lookahead.type != RBRACE) {
        // 根据实际语法补充解析逻辑
    }
    match(RBRACE);
}
    void parse_program()
    {
        parse_struct_def();
        parse_main_function();
        match(EOF_);
    }

    void parse_struct_def()
    {
        match(STRUCT);
        match(ID);
        match(LBRACE);
        parse_member_list();
        match(RBRACE);
        match(SEMI);
    }

    void parse_member_list()
    {
        while (lookahead.type == INT || lookahead.type == FLOAT || lookahead.type == CHAR)
        {
            parse_type();
            match(ID);
            match(SEMI);
        }
    }

    // 其他解析函数实现...
      // 完整类型解析
    void parse_type() {
        switch(lookahead.type) {
            case INT: match(INT); break;
            case FLOAT: match(FLOAT); break;
            case CHAR: 
                match(CHAR);
                if (lookahead.type == STAR) {
                    match(STAR); // 处理char*类型
                }
                break;
            default:
                throw std::runtime_error("Expected type specifier");
        }
    }

    // 变量声明（含数组）
    void parse_variable_decl() {
        parse_type();
        match(ID);
        
        // 处理数组维度
        while (lookahead.type == LBRACKET) {
            match(LBRACKET);
            match(NUMBER);
            match(RBRACKET);
        }
        
        // 处理初始化
        if (lookahead.type == ASSIGN) {
            match(ASSIGN);
            parse_init_value();
        }
        match(SEMI);
    }

    // 初始化值（结构体/数组）
    void parse_init_value() {
        if (lookahead.type == LBRACE) { // 数组/结构体初始化
            match(LBRACE);
            while (lookahead.type != RBRACE) {
                parse_single_init();
                if (lookahead.type == COMMA) match(COMMA);
            }
            match(RBRACE);
        } else { // 简单赋值
            parse_expr();
        }
    }

    // if语句解析
    void parse_if_stmt() {
        match(IF);
        match(LPAREN);
        parse_condition();
        match(RPAREN);
        parse_stmt();
        
        if (lookahead.type == ELSE) {
            match(ELSE);
            parse_stmt();
        }
    }

    // 表达式解析（使用分层消除左递归）
    void parse_expr() {
        parse_logical_or();
    }

    void parse_logical_or() {
        parse_logical_and();
        while (lookahead.type == OR) {
            match(OR);
            parse_logical_and();
        }
    }

    void parse_logical_and() {
        parse_equality();
        while (lookahead.type == AND) {
            match(AND);
            parse_equality();
        }
    }

    void parse_equality() {
        parse_relational();
        while (lookahead.type == EQUAL || lookahead.type == NOTEQUAL) {
            match(lookahead.type);
            parse_relational();
        }
    }

    void parse_relational() {
        parse_additive();
        while (lookahead.type == LESS || lookahead.type == GREATER || 
               lookahead.type == LESSEQ || lookahead.type == GREATEREQ) {
            match(lookahead.type);
            parse_additive();
        }
    }

    void parse_additive() {
        parse_term();
        while (lookahead.type == PLUS || lookahead.type == MINUS) {
            match(lookahead.type);
            parse_term();
        }
    }

    void parse_term() {
        parse_factor();
        while (lookahead.type == STAR || lookahead.type == SLASH) {
            match(lookahead.type);
            parse_factor();
        }
    }

    void parse_factor() {
        switch(lookahead.type) {
            case ID: 
                match(ID);
                parse_postfix();
                break;
            case NUMBER: 
            case FLOATVAL:
            case STRING:
                match(lookahead.type);
                break;
            case LPAREN:
                match(LPAREN);
                parse_expr();
                match(RPAREN);
                break;
            default:
                throw std::runtime_error("Unexpected factor");
        }
    }

    // 处理成员访问和数组下标
    void parse_postfix() {
        while (true) {
            if (lookahead.type == DOT) {
                match(DOT);
                match(ID);
            } else if (lookahead.type == LBRACKET) {
                match(LBRACKET);
                parse_expr();
                match(RBRACKET);
            } else {
                break;
            }
        }
    }

    // 函数调用解析
    void parse_function_call() {
        match(ID);
        match(LPAREN);
        if (lookahead.type != RPAREN) {
            parse_argument_list();
        }
        match(RPAREN);
    }

    // 参数列表
    void parse_argument_list() {
        do {
            parse_expr();
            if (lookahead.type == COMMA) match(COMMA);
        } while (lookahead.type != RPAREN);
    }

    // printf语句解析
    void parse_printf_stmt() {
        match(PRINTF);
        match(LPAREN);
        match(STRING);
        if (lookahead.type == COMMA) {
            match(COMMA);
            parse_argument_list();
        }
        match(RPAREN);
        match(SEMI);
    }
};

// 预测分析表驱动版本示例（部分）
class LL1Parser
{
    std::stack<TokenType> parse_stack;
    std::unordered_map<TokenType, std::unordered_map<TokenType, std::vector<TokenType>>> parsing_table;

public:
    void init_parsing_table()
    {
        // 初始化预测分析表
        parsing_table[PROGRAM][STRUCT] = {STRUCT_DEF, MAIN_FUNC};
        // 其他产生式规则...
    }

    void parse(std::vector<Token> &tokens)
    {
        parse_stack.push(PROGRAM);
        size_t pos = 0;

        while (!parse_stack.empty())
        {
            TokenType top = parse_stack.top();
            parse_stack.pop();

            if (top == tokens[pos].type)
            {
                pos++;
            }
            else if (parsing_table.count(top) &&
                     parsing_table[top].count(tokens[pos].type))
            {
                auto &production = parsing_table[top][tokens[pos].type];
                for (auto it = production.rbegin(); it != production.rend(); ++it)
                {
                    parse_stack.push(*it);
                }
            }
            else
            {
                throw std::runtime_error("Syntax error");
            }
        }
    }
};

int main()
{
    std::string source = "..."; // 输入源代码
    Lexer lexer(source);

    // 递归下降版本
    try
    {
        Parser parser(lexer);
        parser.parse_program();
        std::cout << "Parse successful!" << std::endl;
    }
    catch (...)
    {
        std::cout << "Parse failed!" << std::endl;
    }

    return 0;
}
