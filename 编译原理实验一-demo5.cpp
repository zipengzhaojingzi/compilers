#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*-------------------------
        常量定义
-------------------------*/
#define MAX_SYMBOLS 100   // 符号表最大容量
#define MAX_KEYWORDS 10   // 最大关键字数量
#define MAX_TOKEN_LEN 256 // Token值的最大长度

/*-------------------------
        类型定义
-------------------------*/
typedef enum
{
    TOKEN_EOF = 0,     // 文件结束
    TOKEN_IDENT = 1,   // 标识符
    TOKEN_NUM = 2,     // 数字（仅整数）
    TOKEN_KEYWORD = 3, // 关键字
    TOKEN_OP = 4,      // 运算符（扩展支持 >,>=,<,<=, &&, ||）
    TOKEN_DELIM = 5,   // 分隔符
    TOKEN_ERROR = -1   // 错误
} TokenType;

typedef struct
{
    char name[MAX_TOKEN_LEN];
    int index;
} SymbolEntry;

typedef struct
{
    SymbolEntry entries[MAX_SYMBOLS];
    int count;
} SymbolTable;

typedef struct
{
    FILE *input;
    SymbolTable *symtab;
    const char **keywords;
    int keyword_count;
} Lexer;

typedef struct
{
    TokenType type;
    char value[MAX_TOKEN_LEN];
} Token;

/*-------------------------
        函数声明
-------------------------*/
// 符号表操作函数
void init_symbol_table(SymbolTable *table);
int manage_symbol(SymbolTable *table, const char *name);

// 词法分析核心函数
void init_lexer(Lexer *lexer, FILE *input, SymbolTable *table,
                const char **keywords, int key_count);
void skip_single_line_comment(Lexer *lexer);
int skip_block_comment(Lexer *lexer);
Token handle_identifier(Lexer *lexer, char first_char);
Token handle_number(Lexer *lexer, char first_char);
Token handle_operator(Lexer *lexer, char first_char);
Token get_next_token(Lexer *lexer);

// 辅助功能函数
void print_symbol_table(const SymbolTable *table);
void print_token(const Token *token);

/*-------------------------
        符号表操作函数
-------------------------*/

// 初始化符号表
void init_symbol_table(SymbolTable *table)
{
    table->count = 0;
    memset(table->entries, 0, sizeof(table->entries));
}

// 管理符号表条目（查找或添加）
int manage_symbol(SymbolTable *table, const char *name)
{
    for (int i = 0; i < table->count; i++)
    {
        if (strcmp(table->entries[i].name, name) == 0)
        {
            return i;
        }
    }

    if (table->count >= MAX_SYMBOLS)
    {
        fprintf(stderr, "[错误] 符号表已满，无法添加：%s\n", name);
        return -1;
    }

    strncpy(table->entries[table->count].name, name, MAX_TOKEN_LEN - 1);
    table->entries[table->count].index = table->count;
    return table->count++;
}

/*-------------------------
        词法分析核心函数
-------------------------*/

// 初始化词法分析器
void init_lexer(Lexer *lexer, FILE *input, SymbolTable *table,
                const char **keywords, int key_count)
{
    lexer->input = input;
    lexer->symtab = table;
    lexer->keywords = keywords;
    lexer->keyword_count = key_count;
}

/**
 * 跳过单行注释（//...）
 * @param lexer 词法分析器上下文
 */
void skip_single_line_comment(Lexer *lexer)
{
    char ch;
    while ((ch = fgetc(lexer->input)) != EOF)
    {
        if (ch == '\n')
            break;
    }
}

/**
 * 跳过多行注释（/*...* /）
 * @param lexer 词法分析器上下文
 * @return 成功跳过返回1，否则返回0
 */
int skip_block_comment(Lexer *lexer)
{
    char ch, prev = 0;
    while ((ch = fgetc(lexer->input)) != EOF)
    {
        if (prev == '*' && ch == '/')
            return 1;
        prev = ch;
    }
    return 0; // 文件结束前未找到注释结尾
}

// 处理标识符和关键字
Token handle_identifier(Lexer *lexer, char first_char)
{
    Token token;
    char buffer[MAX_TOKEN_LEN] = {first_char};
    int pos = 1;
    char ch;

    while ((ch = fgetc(lexer->input)) != EOF &&
           (isalnum(ch) || ch == '_'))
    {
        if (pos < MAX_TOKEN_LEN - 1)
            buffer[pos++] = ch;
    }
    ungetc(ch, lexer->input);
    buffer[pos] = '\0';

    // 关键字检查
    for (int i = 0; i < lexer->keyword_count; i++)
    {
        if (strcmp(buffer, lexer->keywords[i]) == 0)
        {
            token.type = TOKEN_KEYWORD;
            strcpy(token.value, buffer);
            return token;
        }
    }

    int sym_index = manage_symbol(lexer->symtab, buffer);
    if (sym_index == -1)
    {
        token.type = TOKEN_ERROR;
        strcpy(token.value, "符号表错误");
    }
    else
    {
        token.type = TOKEN_IDENT;
        snprintf(token.value, MAX_TOKEN_LEN, "%d", sym_index);
    }
    return token;
}

//// 处理数字（仅整数）
// Token handle_number(Lexer* lexer, char first_char) {
//     Token token;
//     char buffer[MAX_TOKEN_LEN] = {first_char};
//     int pos = 1;
//     char ch;
//
//     while ((ch = fgetc(lexer->input)) != EOF && isdigit(ch)) {
//         if (pos < MAX_TOKEN_LEN-1) buffer[pos++] = ch;
//     }
//     ungetc(ch, lexer->input);
//     buffer[pos] = '\0';
//
//     token.type = TOKEN_NUM;
//     strcpy(token.value, buffer);
//     return token;
// }

// 修改后的 handle_number 函数
Token handle_number(Lexer *lexer, char first_char)
{
    Token token;
    char buffer[MAX_TOKEN_LEN] = {first_char};
    int pos = 1;
    char ch;

    while ((ch = fgetc(lexer->input)) != EOF)
    {
        if (isdigit(ch))
        {
            if (pos < MAX_TOKEN_LEN - 1)
                buffer[pos++] = ch;
        }
        else if (ch == '.')
        { // 遇到小数点直接报错
            buffer[pos++] = ch;
            // 继续读取后续字符以显示完整错误信息
            while ((ch = fgetc(lexer->input)) != EOF && (isdigit(ch) || ch == '.'))
            {
                if (pos < MAX_TOKEN_LEN - 1)
                    buffer[pos++] = ch;
            }
            ungetc(ch, lexer->input);
            buffer[pos] = '\0';
            token.type = TOKEN_ERROR;
            snprintf(token.value, MAX_TOKEN_LEN, "暂不支持浮点数类型: %s", buffer);
            return token;
        }
        else
        {
            ungetc(ch, lexer->input);
            break;
        }
    }
    buffer[pos] = '\0';
    token.type = TOKEN_NUM;
    strcpy(token.value, buffer);
    return token;
}

// 处理运算符和分隔符（扩展支持>,>=,<,<=, &&, ||）
Token handle_operator(Lexer *lexer, char first_char)
{
    Token token;
    char ch;

    switch (first_char)
    {
    // 处理分隔符
    case '(':
    case ')':
    case ';':
    case ',':
    case '{':
    case '}':
        token.type = TOKEN_DELIM;
        token.value[0] = first_char;
        token.value[1] = '\0';
        break;

    // 处理注释和除法运算符
    case '/':
    {
        ch = fgetc(lexer->input);
        if (ch == '/')
        { // 单行注释
            skip_single_line_comment(lexer);
            return get_next_token(lexer);
        }
        else if (ch == '*')
        { // 块注释
            if (!skip_block_comment(lexer))
            {
                token.type = TOKEN_ERROR;
                strcpy(token.value, "未闭合的块注释");
            }
            else
            {
                return get_next_token(lexer);
            }
        }
        else
        { // 普通除法运算符
            ungetc(ch, lexer->input);
            token.type = TOKEN_OP;
            token.value[0] = first_char;
            token.value[1] = '\0';
        }
        break;
    }

    case '&':
    {
        ch = fgetc(lexer->input);
        if (ch == '&')
        { // 处理 &&
            strcpy(token.value, "&&");
            token.type = TOKEN_OP;
        }
        else
        { // 单个 &
            ungetc(ch, lexer->input);
            token.value[0] = first_char;
            token.value[1] = '\0';
            token.type = TOKEN_OP;
        }
        break;
    }

    case '|':
    {
        ch = fgetc(lexer->input);
        if (ch == '|')
        { // 处理 ||
            strcpy(token.value, "||");
            token.type = TOKEN_OP;
        }
        else
        { // 单个 | 按错误处理
            ungetc(ch, lexer->input);
            token.type = TOKEN_ERROR;
            snprintf(token.value, MAX_TOKEN_LEN, "未知运算符: |");
        }
        break;
    }

    case '!':
    {
        char next_ch = fgetc(lexer->input);
        if (next_ch == '=')
        { // 处理 !=
            strcpy(token.value, "!=");
            token.type = TOKEN_OP;
        }
        else
        { // 单独的逻辑非 !
            ungetc(next_ch, lexer->input);
            strcpy(token.value, "!");
            token.type = TOKEN_OP;
        }
        break;
    }

    // 处理等号相关运算符
    case '=':
    {
        ch = fgetc(lexer->input);
        if (ch == '=')
        { // 双等号 ==
            strcpy(token.value, "==");
            token.type = TOKEN_OP;
        }
        else
        { // 单等号 =
            ungetc(ch, lexer->input);
            token.value[0] = first_char;
            token.value[1] = '\0';
            token.type = TOKEN_OP;
        }
        break;
    }

    // 处理大于号相关运算符
    case '>':
    {
        ch = fgetc(lexer->input);
        if (ch == '=')
        { // >=
            strcpy(token.value, ">=");
        }
        else
        { // >
            ungetc(ch, lexer->input);
            strcpy(token.value, ">");
        }
        token.type = TOKEN_OP;
        break;
    }

    // 处理小于号相关运算符
    case '<':
    {
        ch = fgetc(lexer->input);
        if (ch == '=')
        { // <=
            strcpy(token.value, "<=");
        }
        else
        { // <
            ungetc(ch, lexer->input);
            strcpy(token.value, "<");
        }
        token.type = TOKEN_OP;
        break;
    }

    // 基础算术运算符
    case '+':
    case '-':
    case '*':
        token.type = TOKEN_OP;
        token.value[0] = first_char;
        token.value[1] = '\0';
        break;

    // 处理小数点作为错误
    case '.':
        token.type = TOKEN_ERROR;
        snprintf(token.value, MAX_TOKEN_LEN, "无效字符: .");
        break;

    default:
        token.type = TOKEN_ERROR;
        snprintf(token.value, MAX_TOKEN_LEN, "未知字符: %c", first_char);
    }
    return token;
}

// 获取下一个Token（核心分发函数）
Token get_next_token(Lexer *lexer)
{
    Token token;
    char ch;

    // 跳过空白字符
    while ((ch = fgetc(lexer->input)) != EOF)
    {
        if (!isspace(ch))
            break;
    }

    // 处理文件结束
    if (ch == EOF)
    {
        token.type = TOKEN_EOF;
        strcpy(token.value, "");
        return token;
    }

    // 根据首字符类型分发处理
    if (isalpha(ch) || ch == '_')
    {
        return handle_identifier(lexer, ch);
    }
    else if (isdigit(ch))
    { // 仅处理以数字开头的整数
        return handle_number(lexer, ch);
    }
    else
    {
        return handle_operator(lexer, ch);
    }
}

/*-------------------------
        辅助功能函数
-------------------------*/
void print_symbol_table(const SymbolTable *table)
{
    printf("\n符号表内容（共%d项）:\n", table->count);
    for (int i = 0; i < table->count; i++)
    {
        printf("  [%d] %s\n", i, table->entries[i].name);
    }
}

void print_token(const Token *token)
{
    const char *type_names[] = {
        "EOF", "IDENT", "NUM", "KEYWORD",
        "OP", "DELIM", "ERROR"};

    int index = (token->type == TOKEN_ERROR) ? 6 : token->type;
    printf("(%d:%-7s, '%s')\n", token->type, type_names[index], token->value);
}

/*-------------------------
        主程序
-------------------------*/
int main()
{
    // 初始化系统组件
    SymbolTable symtab;
    init_symbol_table(&symtab);

    const char *keywords[] = {"if", "else", "while", "int", "return"};

    Lexer lexer;
    FILE *input = fopen("d:\\20250226candid\\compilers\\test3.c", "r");

    if (!input)
    {
        perror("[错误] 文件打开失败");
        return EXIT_FAILURE;
    }
    init_lexer(&lexer, input, &symtab, keywords, 5);

    // 执行词法分析
    printf("开始词法分析...\n");
    Token token;
    do
    {
        token = get_next_token(&lexer);
        print_token(&token);
    } while (token.type != TOKEN_EOF);

    // 打印符号表
    print_symbol_table(&symtab);
    fclose(input);
    return EXIT_SUCCESS;
}
