#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*-------------------------
        ��������
-------------------------*/
#define MAX_SYMBOLS 100   // ���ű��������
#define MAX_KEYWORDS 10   // ���ؼ�������
#define MAX_TOKEN_LEN 256 // Tokenֵ����󳤶�

/*-------------------------
        ���Ͷ���
-------------------------*/
typedef enum
{
    TOKEN_EOF = 0,     // �ļ�����
    TOKEN_IDENT = 1,   // ��ʶ��
    TOKEN_NUM = 2,     // ���֣���������
    TOKEN_KEYWORD = 3, // �ؼ���
    TOKEN_OP = 4,      // ���������չ֧�� >,>=,<,<=, &&, ||��
    TOKEN_DELIM = 5,   // �ָ���
    TOKEN_36NUM = 6,
    TOKEN_ERROR = -1 // ����
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
        ��������
-------------------------*/
// ���ű��������
void init_symbol_table(SymbolTable *table);
int manage_symbol(SymbolTable *table, const char *name);

// �ʷ��������ĺ���
void init_lexer(Lexer *lexer, FILE *input, SymbolTable *table,
                const char **keywords, int key_count);
void skip_single_line_comment(Lexer *lexer);
int skip_block_comment(Lexer *lexer);
Token handle_identifier(Lexer *lexer, char first_char, Token last_token);
Token handle_number(Lexer *lexer, char first_char);
Token handle_operator(Lexer *lexer, char first_char);
Token get_next_token(Lexer *lexer);

// �������ܺ���
void print_symbol_table(const SymbolTable *table);
void print_token(const Token *token);

/*-------------------------
        ���ű��������
-------------------------*/

// ��ʼ�����ű�
void init_symbol_table(SymbolTable *table)
{
    table->count = 0;
    memset(table->entries, 0, sizeof(table->entries));
}

// ������ű���Ŀ�����һ���ӣ�
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
        fprintf(stderr, "[����] ���ű��������޷���ӣ�%s\n", name);
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
        �ʷ��������ĺ���
-------------------------*/

// ��ʼ���ʷ�������
void init_lexer(Lexer *lexer, FILE *input, SymbolTable *table,
                const char **keywords, int key_count)
{
    lexer->input = input;
    lexer->symtab = table;
    lexer->keywords = keywords;
    lexer->keyword_count = key_count;
}

/**
 * ��������ע�ͣ�//...��
 * @param lexer �ʷ�������������
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
 * ��������ע��
 * @param lexer �ʷ�������������
 * @return �ɹ���������1�����򷵻�0
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
    return 0; // �ļ�����ǰδ�ҵ�ע�ͽ�β
}

// �����ʶ���͹ؼ���

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

    // �ؼ��ּ��
    for (int i = 0; i < lexer->keyword_count; i++)
    {
        if (strcmp(buffer, lexer->keywords[i]) == 0)
        {
            token.type = TOKEN_KEYWORD;
            strcpy(token.value, buffer);
            return token;
        }
    }

    // ���ǰһ�� token �� int �� int36����ֱ�ӽ���ʶ��Ϊ��ʶ��
    // if (last_token.type == TOKEN_KEYWORD &&
    //     (strcmp(last_token.value, "int") == 0 || strcmp(last_token.value, "int36") == 0))
    // {
    //     int sym_index = manage_symbol(lexer->symtab, buffer, 1);
    //     if (sym_index == -1)
    //     {
    //         fprintf(stderr, "[����] ���ű��������޷���� %s\n", buffer);
    //         token.type = TOKEN_ERROR;
    //         snprintf(token.value, MAX_TOKEN_LEN, "���ű�����");
    //         return token;
    //     }
    //     token.type = TOKEN_IDENT;
    //     snprintf(token.value, MAX_TOKEN_LEN, "%d", sym_index);
    //     return token;
    // }

    // // ���ű���
    // int sym_index = manage_symbol(lexer->symtab, buffer, 0);
    // // printf("%d %d", sym_index, lexer->symtab->count);
    // if (sym_index == lexer->symtab->count && first_char != '_')
    // {
    //     // ������ڷ��ű��У����´���Ϊ36������
    //     // ungetc(first_char, lexer->input); // �˻ص�һ���ַ�
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
        fprintf(stderr, "[����] ���ű��������޷���� %s\n", buffer);
        token.type = TOKEN_ERROR;
        snprintf(token.value, MAX_TOKEN_LEN, "���ű�����");
        return token;
    }
    token.type = TOKEN_IDENT;
    snprintf(token.value, MAX_TOKEN_LEN, "%d", sym_index);
    return token;

    return token;
}

//// �������֣���������
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

//     // ����Ƿ�Ϊ36������
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
//         snprintf(token.value, MAX_TOKEN_LEN, "0z(%s)", buffer); // ���0zǰ׺�Ա�ʶ��
//     }
//     else
//     {
//         token.type = TOKEN_36NUM;
//         snprintf(token.value, MAX_TOKEN_LEN, "%s", buffer);
//     }
//     return token;
// }

// �޸ĺ�� handle_number ����
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
//     { // ����С����ֱ�ӱ���
//         buffer[pos++] = ch;
//         // ������ȡ�����ַ�����ʾ����������Ϣ
//         while ((ch = fgetc(lexer->input)) != EOF && (isdigit(ch) || ch == '.'))
//         {
//             if (pos < MAX_TOKEN_LEN - 1)
//                 buffer[pos++] = ch;
//         }
//         ungetc(ch, lexer->input);
//         buffer[pos] = '\0';
//         token.type = TOKEN_ERROR;
//         snprintf(token.value, MAX_TOKEN_LEN, "�ݲ�֧�ָ���������: %s", buffer);
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
//         { // ����С����ֱ�ӱ���
//             buffer[pos++] = ch;
//             // ������ȡ�����ַ�����ʾ����������Ϣ
//             while ((ch = fgetc(lexer->input)) != EOF && (isdigit(ch) || ch == '.'))
//             {
//                 if (pos < MAX_TOKEN_LEN - 1)
//                     buffer[pos++] = ch;
//             }
//             ungetc(ch, lexer->input);
//             buffer[pos] = '\0';
//             token.type = TOKEN_ERROR;
//             snprintf(token.value, MAX_TOKEN_LEN, "�ݲ�֧�ָ���������: %s", buffer);
//             return token;
//         }
//         else
//         {
//             ungetc(ch, lexer->input);
//             break;
//         }
//     }
//     printf("After reading digits: %s\n", buffer);
//     // // ��ȡ���ֲ���
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

//     // �������ַ��Ƿ�Ϊ��ĸ a-z �� A-Z
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

//     // ����Ƿ�Ϊ0z(...)��ʽ��36������
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
//                 snprintf(token.value, MAX_TOKEN_LEN, "��Ч��36���Ƹ�ʽ: 0z(%s", buffer);
//                 return token;
//             }
//         }
//         else
//         {
//             ungetc(ch, lexer->input);
//         }
//     }

//     // ����Ƿ�Ϊ36������
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

    // ������ʽ36���Ƹ�ʽ 0z(...)
    if (first_char == '0' && pos == 1)
    {
        // ���Զ�ȡ�ڶ����ַ�
        ch = fgetc(lexer->input);
        if (ch == 'z' && pos < MAX_TOKEN_LEN - 1)
        {
            buffer[pos++] = ch; // ��ǰ buffer = "0z"

            // ��ȡ������
            ch = fgetc(lexer->input);
            if (ch == '(')
            {
                // ��ȡ��������
                while ((ch = fgetc(lexer->input)) != EOF && ch != ')')
                {
                    if (isalnum(ch) && pos < MAX_TOKEN_LEN - 1)
                    {
                        buffer[pos++] = ch;
                    }
                    else if (ch == '.' || ch == '_')
                    { // ��ֹ�������
                        buffer[pos++] = ch;
                        buffer[pos] = '\0';
                        token.type = TOKEN_ERROR;
                        snprintf(token.value, MAX_TOKEN_LEN, "��Ч�ַ�: %s", buffer);
                        return token;
                    }
                    else
                    {
                        ungetc(ch, lexer->input);
                        break;
                    }
                }

                // ����Ƿ�ɹ��պ�
                if (ch == ')')
                {
                    buffer[pos] = '\0';
                    token.type = TOKEN_36NUM;
                    snprintf(token.value, MAX_TOKEN_LEN, "0z(%s)", buffer + 2); // ����"0z"
                    return token;
                }
                else
                {
                    buffer[pos] = '\0';
                    token.type = TOKEN_ERROR;
                    snprintf(token.value, MAX_TOKEN_LEN, "δ�պϵ�36����: %s", buffer);
                    return token;
                }
            }
            else
            {
                ungetc(ch, lexer->input);
                // 0z��û�����ţ�����ͨ36���ƴ���
            }
        }
        else
        {
            ungetc(ch, lexer->input);
        }
    }
    // printf("After reading digits: %s\n", buffer);
    // ������ͨ����/��ĸ����ʽ36���ƣ�
    while ((ch = fgetc(lexer->input)) != EOF)
    {
        // printf("%c ", ch);
        // ���С���㣨�������ȼ�飩
        if (ch == '.')
        {
            buffer[pos++] = ch;
            // ��ȡ����С������
            while ((ch = fgetc(lexer->input)) != EOF && (isdigit(ch) || ch == '.'))
            {
                if (pos < MAX_TOKEN_LEN - 1)
                    buffer[pos++] = ch;
            }
            ungetc(ch, lexer->input);
            buffer[pos] = '\0';
            token.type = TOKEN_ERROR;
            snprintf(token.value, MAX_TOKEN_LEN, "�ݲ�֧�ָ�����: %s", buffer);
            return token;
        }
        // ���Ϸ��ַ�
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
    // �ж�����
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

// ����������ͷָ�������չ֧��>,>=,<,<=, &&, ||��
Token handle_operator(Lexer *lexer, char first_char)
{
    Token token;
    char ch;

    switch (first_char)
    {
    // ����ָ���
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

    // ����ע�ͺͳ��������
    case '/':
    {
        ch = fgetc(lexer->input);
        if (ch == '/')
        { // ����ע��
            skip_single_line_comment(lexer);
            return get_next_token(lexer);
        }
        else if (ch == '*')
        { // ��ע��
            if (!skip_block_comment(lexer))
            {
                token.type = TOKEN_ERROR;
                strcpy(token.value, "δ�պϵĿ�ע��");
            }
            else
            {
                return get_next_token(lexer);
            }
        }
        else
        { // ��ͨ���������
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
        { // ���� &&
            strcpy(token.value, "&&");
            token.type = TOKEN_OP;
        }
        else
        { // ���� &
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
        { // ���� ||
            strcpy(token.value, "||");
            token.type = TOKEN_OP;
        }
        else
        { // ���� | ��������
            ungetc(ch, lexer->input);
            token.type = TOKEN_ERROR;
            snprintf(token.value, MAX_TOKEN_LEN, "δ֪�����: |");
        }
        break;
    }

    case '!':
    {
        char next_ch = fgetc(lexer->input);
        if (next_ch == '=')
        { // ���� !=
            strcpy(token.value, "!=");
            token.type = TOKEN_OP;
        }
        else
        { // �������߼��� !
            ungetc(next_ch, lexer->input);
            strcpy(token.value, "!");
            token.type = TOKEN_OP;
        }
        break;
    }

    // ����Ⱥ���������
    case '=':
    {
        ch = fgetc(lexer->input);
        if (ch == '=')
        { // ˫�Ⱥ� ==
            strcpy(token.value, "==");
            token.type = TOKEN_OP;
        }
        else
        { // ���Ⱥ� =
            ungetc(ch, lexer->input);
            token.value[0] = first_char;
            token.value[1] = '\0';
            token.type = TOKEN_OP;
        }
        break;
    }

    // ������ں���������
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

    // ����С�ں���������
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

    // �������������
    case '+':
    case '-':
    case '*':
        token.type = TOKEN_OP;
        token.value[0] = first_char;
        token.value[1] = '\0';
        break;

    // ����С������Ϊ����
    case '.':
        token.type = TOKEN_ERROR;
        snprintf(token.value, MAX_TOKEN_LEN, "��Ч�ַ�: .");
        break;

    default:
        token.type = TOKEN_ERROR;
        snprintf(token.value, MAX_TOKEN_LEN, "δ֪�ַ�: %c", first_char);
    }
    return token;
}

// ��ȡ��һ��Token�����ķַ�������
Token get_next_token(Lexer *lexer)
{
    Token token;
    char ch;
    static Token last_token = {TOKEN_EOF, ""};

    // �����հ��ַ�
    while ((ch = fgetc(lexer->input)) != EOF)
    {
        if (!isspace(ch))
            break;
    }

    // �����ļ�����
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
    { // �����������ֿ�ͷ������

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
        �������ܺ���
-------------------------*/
void print_symbol_table(const SymbolTable *table)
{
    printf("\n��ʶ�������ݣ���%d�:\n", table->count);
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
        ������
-------------------------*/
int main()
{
    // ��ʼ��ϵͳ���
    SymbolTable symtab;
    init_symbol_table(&symtab);

    const char *keywords[] = {"if", "else", "while", "int", "return", "main", "int36", "printf",
                              "void", "for"};

    Lexer lexer;
    FILE *input = fopen("d:\\20250226candid\\compilers\\test4.txt", "r");

    if (!input)
    {
        perror("[����] �ļ���ʧ��");
        return EXIT_FAILURE;
    }
    init_lexer(&lexer, input, &symtab, keywords, 10);

    // ִ�дʷ�����
    printf("��ʼ�ʷ�����...\n");
    Token token;
    do
    {
        token = get_next_token(&lexer);
        print_token(&token);
    } while (token.type != TOKEN_EOF);

    // ��ӡ���ű�
    print_symbol_table(&symtab);
    fclose(input);
    return EXIT_SUCCESS;
}
