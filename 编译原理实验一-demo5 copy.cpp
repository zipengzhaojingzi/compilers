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
    TOKEN_36NUM = 6,
    TOKEN_ERROR = -1 // 错误
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
Token handle_identifier(Lexer *lexer, char first_char, Token last_token);
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
int manage_symbol(SymbolTable *table, const char *name, int increment_count)
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
    // return table->count++;
    int index = table->count;

    if (increment_count)
    {
        table->count++;
    }
    return index;
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
 * 跳过多行注释
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

Token handle_identifier(Lexer *lexer, char first_char, Token last_token)
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

    // 如果前一个 token 是 int 或 int36，则直接将其识别为标识符
    // if (last_token.type == TOKEN_KEYWORD &&
    //     (strcmp(last_token.value, "int") == 0 || strcmp(last_token.value, "int36") == 0))
    // {
    //     int sym_index = manage_symbol(lexer->symtab, buffer, 1);
    //     if (sym_index == -1)
    //     {
    //         fprintf(stderr, "[错误] 符号表已满，无法添加 %s\n", buffer);
    //         token.type = TOKEN_ERROR;
    //         snprintf(token.value, MAX_TOKEN_LEN, "符号表已满");
    //         return token;
    //     }
    //     token.type = TOKEN_IDENT;
    //     snprintf(token.value, MAX_TOKEN_LEN, "%d", sym_index);
    //     return token;
    // }

    // // 符号表检查
    // int sym_index = manage_symbol(lexer->symtab, buffer, 0);
    // // printf("%d %d", sym_index, lexer->symtab->count);
    // if (sym_index == lexer->symtab->count && first_char != '_')
    // {
    //     // 如果不在符号表中，重新处理为36进制数
    //     // ungetc(first_char, lexer->input); // 退回第一个字符
    //     // token = handle_number(lexer, first_char);
    //     token.type = TOKEN_36NUM;
    //     snprintf(token.value, MAX_TOKEN_LEN, "%s", buffer);
    //     return token;
    // }
    // else
    // {
    //     token.type = TOKEN_IDENT;
    //     snprintf(token.value, MAX_TOKEN_LEN, "%d", sym_index);
    // }
    int sym_index = manage_symbol(lexer->symtab, buffer, 1);
    if (sym_index == -1)
    {
        fprintf(stderr, "[错误] 符号表已满，无法添加 %s\n", buffer);
        token.type = TOKEN_ERROR;
        snprintf(token.value, MAX_TOKEN_LEN, "符号表已满");
        return token;
    }
    token.type = TOKEN_IDENT;
    snprintf(token.value, MAX_TOKEN_LEN, "%d", sym_index);
    return token;

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

// Token handle_36_number(Lexer *lexer, char first_char)
// {
//     Token token;
//     char buffer[MAX_TOKEN_LEN] = {first_char};
//     int pos = 1;
//     char ch;

//     while ((ch = fgetc(lexer->input)) != EOF)
//     {
//         if (isdigit(ch) || (ch >= 'g' && ch <= 'z') || (ch >= 'G' && ch <= 'Z') || ch == '(' || ch == ')')
//         {
//             if (pos < MAX_TOKEN_LEN - 1)
//                 buffer[pos++] = ch;
//         }
//         else
//         {
//             ungetc(ch, lexer->input);
//             // token.type = TOKEN_ERROR;
//             // return token;
//             break;
//         }
//     }
//     buffer[pos] = '\0';

//     // 检查是否为36进制数
//     int is_hex = 0;
//     if (buffer[0] == '0' && buffer[1] == 'z')
//         is_hex = 1;
//     else
//     {
//         for (int i = 1; i < pos; i++)
//         {
//             if ((buffer[i] >= 'h' && buffer[i] <= 'z') || (buffer[i] >= 'H' && buffer[i] <= 'Z'))
//             {
//                 is_hex = 2;
//                 break;
//             }
//         }
//     }

//     if (is_hex == 0)
//     {
//         ungetc(ch, lexer->input);
//         token.type = TOKEN_ERROR;
//         return token;
//     }
//     else if (is_hex == 2)
//     {
//         token.type = TOKEN_36NUM;
//         snprintf(token.value, MAX_TOKEN_LEN, "0z(%s)", buffer); // 添加0z前缀以便识别
//     }
//     else
//     {
//         token.type = TOKEN_36NUM;
//         snprintf(token.value, MAX_TOKEN_LEN, "%s", buffer);
//     }
//     return token;
// }

// 修改后的 handle_number 函数
// Token handle_number(Lexer *lexer, char first_char)
// {
// Token token;
// char buffer[MAX_TOKEN_LEN] = {first_char};
// int pos = 1;
// char ch;

// while ((ch = fgetc(lexer->input)) != EOF)
// {
//     if (isdigit(ch))
//     {
//         if (pos < MAX_TOKEN_LEN - 1)
//             buffer[pos++] = ch;
//     }
//     else if (ch == '.')
//     { // 遇到小数点直接报错
//         buffer[pos++] = ch;
//         // 继续读取后续字符以显示完整错误信息
//         while ((ch = fgetc(lexer->input)) != EOF && (isdigit(ch) || ch == '.'))
//         {
//             if (pos < MAX_TOKEN_LEN - 1)
//                 buffer[pos++] = ch;
//         }
//         ungetc(ch, lexer->input);
//         buffer[pos] = '\0';
//         token.type = TOKEN_ERROR;
//         snprintf(token.value, MAX_TOKEN_LEN, "暂不支持浮点数类型: %s", buffer);
//         return token;
//     }
//     else
//     {
//         ungetc(ch, lexer->input);
//         break;
//     }
// }
// buffer[pos] = '\0';
// token.type = TOKEN_NUM;
// strcpy(token.value, buffer);
// return token;
// }

// Token handle_number(Lexer *lexer, char first_char)
// {
//     Token token;
//     char buffer[MAX_TOKEN_LEN] = {first_char};
//     int pos = 1;
//     char ch;
//     printf("Initial buffer: %s\n", buffer);

//     while ((ch = fgetc(lexer->input)) != EOF)
//     {
//         if (isdigit(ch))
//         {
//             if (pos < MAX_TOKEN_LEN - 1)
//                 buffer[pos++] = ch;
//         }
//         else if (ch == '.')
//         { // 遇到小数点直接报错
//             buffer[pos++] = ch;
//             // 继续读取后续字符以显示完整错误信息
//             while ((ch = fgetc(lexer->input)) != EOF && (isdigit(ch) || ch == '.'))
//             {
//                 if (pos < MAX_TOKEN_LEN - 1)
//                     buffer[pos++] = ch;
//             }
//             ungetc(ch, lexer->input);
//             buffer[pos] = '\0';
//             token.type = TOKEN_ERROR;
//             snprintf(token.value, MAX_TOKEN_LEN, "暂不支持浮点数类型: %s", buffer);
//             return token;
//         }
//         else
//         {
//             ungetc(ch, lexer->input);
//             break;
//         }
//     }
//     printf("After reading digits: %s\n", buffer);
//     // // 读取数字部分
//     // while ((ch = fgetc(lexer->input)) != EOF)
//     // {
//     //     if (isdigit(ch))
//     //     {
//     //         if (pos < MAX_TOKEN_LEN - 1)
//     //             buffer[pos++] = ch;
//     //     }
//     //     else
//     //     {
//     //         ungetc(ch, lexer->input);
//     //         break;
//     //     }
//     // }

//     // 检查后续字符是否为字母 a-z 或 A-Z
//     while ((ch = fgetc(lexer->input)) != EOF)
//     {
//         if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || isdigit(ch))
//         {
//             if (pos < MAX_TOKEN_LEN - 1)
//                 buffer[pos++] = ch;
//         }
//         else
//         {
//             ungetc(ch, lexer->input);
//             break;
//         }
//     }
//     buffer[pos] = '\0';
//     printf("After reading letters: %s\n", buffer);

//     // 检查是否为0z(...)格式的36进制数
//     if (first_char == '0' && pos == 2 && buffer[1] == 'z')
//     {
//         ch = fgetc(lexer->input);
//         if (ch == '(')
//         {
//             pos = 0;
//             while ((ch = fgetc(lexer->input)) != EOF && ch != ')')
//             {
//                 if (isdigit(ch) || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
//                 {
//                     if (pos < MAX_TOKEN_LEN - 1)
//                         buffer[pos++] = ch;
//                 }
//                 else
//                 {
//                     ungetc(ch, lexer->input);
//                     break;
//                 }
//             }
//             buffer[pos] = '\0';

//             if (ch == ')')
//             {
//                 token.type = TOKEN_36NUM;
//                 snprintf(token.value, MAX_TOKEN_LEN, "0z(%s)", buffer);
//                 return token;
//             }
//             else
//             {
//                 token.type = TOKEN_ERROR;
//                 snprintf(token.value, MAX_TOKEN_LEN, "无效的36进制格式: 0z(%s", buffer);
//                 return token;
//             }
//         }
//         else
//         {
//             ungetc(ch, lexer->input);
//         }
//     }

//     // 检查是否为36进制数
//     int is_hex = 0;
//     for (int i = 1; i <= pos; i++)
//     {
//         if ((buffer[i] >= 'a' && buffer[i] <= 'z') || (buffer[i] >= 'A' && buffer[i] <= 'Z'))
//         {
//             is_hex = 1;
//             break;
//         }
//     }

//     if (is_hex)
//     {
//         token.type = TOKEN_36NUM;
//         snprintf(token.value, MAX_TOKEN_LEN, "%s", buffer);
//     }
//     else
//     {
//         token.type = TOKEN_NUM;
//         strcpy(token.value, buffer);
//     }
//     return token;
// }

Token handle_number(Lexer *lexer, char first_char)
{
    Token token;
    char buffer[MAX_TOKEN_LEN] = {first_char};
    int pos = 1;
    char ch;

    // 处理显式36进制格式 0z(...)
    if (first_char == '0' && pos == 1)
    {
        // 尝试读取第二个字符
        ch = fgetc(lexer->input);
        if (ch == 'z' && pos < MAX_TOKEN_LEN - 1)
        {
            buffer[pos++] = ch; // 当前 buffer = "0z"

            // 读取左括号
            ch = fgetc(lexer->input);
            if (ch == '(')
            {
                // 读取括号内容
                while ((ch = fgetc(lexer->input)) != EOF && ch != ')')
                {
                    if (isalnum(ch) && pos < MAX_TOKEN_LEN - 1)
                    {
                        buffer[pos++] = ch;
                    }
                    else if (ch == '.' || ch == '_')
                    { // 禁止特殊符号
                        buffer[pos++] = ch;
                        buffer[pos] = '\0';
                        token.type = TOKEN_ERROR;
                        snprintf(token.value, MAX_TOKEN_LEN, "无效字符: %s", buffer);
                        return token;
                    }
                    else
                    {
                        ungetc(ch, lexer->input);
                        break;
                    }
                }

                // 检查是否成功闭合
                if (ch == ')')
                {
                    buffer[pos] = '\0';
                    token.type = TOKEN_36NUM;
                    snprintf(token.value, MAX_TOKEN_LEN, "0z(%s)", buffer + 2); // 跳过"0z"
                    return token;
                }
                else
                {
                    buffer[pos] = '\0';
                    token.type = TOKEN_ERROR;
                    snprintf(token.value, MAX_TOKEN_LEN, "未闭合的36进制: %s", buffer);
                    return token;
                }
            }
            else
            {
                ungetc(ch, lexer->input);
                // 0z后没有括号，按普通36进制处理
            }
        }
        else
        {
            ungetc(ch, lexer->input);
        }
    }
    // printf("After reading digits: %s\n", buffer);
    // 处理普通数字/字母（隐式36进制）
    while ((ch = fgetc(lexer->input)) != EOF)
    {
        // printf("%c ", ch);
        // 检查小数点（必须优先检查）
        if (ch == '.')
        {
            buffer[pos++] = ch;
            // 读取后续小数部分
            while ((ch = fgetc(lexer->input)) != EOF && (isdigit(ch) || ch == '.'))
            {
                if (pos < MAX_TOKEN_LEN - 1)
                    buffer[pos++] = ch;
            }
            ungetc(ch, lexer->input);
            buffer[pos] = '\0';
            token.type = TOKEN_ERROR;
            snprintf(token.value, MAX_TOKEN_LEN, "暂不支持浮点数: %s", buffer);
            return token;
        }
        // 检查合法字符
        else if (isdigit(ch) || (isalpha(ch) && pos < MAX_TOKEN_LEN - 1))
        {
            buffer[pos++] = ch;
        }
        else
        {
            ungetc(ch, lexer->input);
            break;
        }
    }
    buffer[pos] = '\0';
    // printf("After reading letters: %s\n", buffer);
    // 判断类型
    int is_hex = 0;
    for (int i = 0; i < pos; i++)
    {
        if (isalpha(buffer[i]))
        {
            is_hex = 1;
            break;
        }
    }

    if (is_hex)
    {
        token.type = TOKEN_36NUM;
        snprintf(token.value, MAX_TOKEN_LEN, "%s", buffer);
    }
    else
    {
        token.type = TOKEN_NUM;
        strcpy(token.value, buffer);
    }

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
    case '%':
        token.type = TOKEN_OP;
        token.value[0] = first_char;
        token.value[1] = '\0';
        break;

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
    static Token last_token = {TOKEN_EOF, ""};

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

    if (isalpha(ch) || ch == '_')
    {

        token = handle_identifier(lexer, ch, last_token);
    }

    else if (isdigit(ch))
    { // 仅处理以数字开头的整数

        token = handle_number(lexer, ch);
    }
    else
    {
        token = handle_operator(lexer, ch);
    }
    last_token = token;
    return token;
}

/*-------------------------
        辅助功能函数
-------------------------*/
void print_symbol_table(const SymbolTable *table)
{
    printf("\n标识符表内容（共%d项）:\n", table->count);
    for (int i = 0; i < table->count; i++)
    {
        printf("  [%d] %s\n", i, table->entries[i].name);
    }
}

void print_token(const Token *token)
{
    const char *type_names[] = {
        "EOF", "IDENT", "NUM", "KEYWORD",
        "OP", "DELIM", "NUM36", "ERROR"};

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

    const char *keywords[] = {"if", "else", "while", "int", "return", "main", "int36", "printf",
                              "void", "for"};

    Lexer lexer;
    FILE *input = fopen("d:\\20250226candid\\compilers\\test4.txt", "r");

    if (!input)
    {
        perror("[错误] 文件打开失败");
        return EXIT_FAILURE;
    }
    init_lexer(&lexer, input, &symtab, keywords, 10);

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
