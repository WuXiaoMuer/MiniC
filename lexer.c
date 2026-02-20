#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "lexer.h"

// 关键字表
typedef struct {
    const char* keyword;
    TokenType tokenType;
} Keyword;

static Keyword keywords[] = {
    {"int", TOKEN_INT},
    {"char", TOKEN_CHAR},
    {"void", TOKEN_VOID},
    {"if", TOKEN_IF},
    {"else", TOKEN_ELSE},
    {"while", TOKEN_WHILE},
    {"for", TOKEN_FOR},
    {"return", TOKEN_RETURN},
    {NULL, TOKEN_UNKNOWN}
};

// 初始化词法分析器
void initLexer(Lexer* lexer, const char* source) {
    lexer->source = source;
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
}

// 跳过空白字符
static void skipWhitespace(Lexer* lexer) {
    while (lexer->source[lexer->position] != '\0') {
        char c = lexer->source[lexer->position];
        if (c == ' ' || c == '\t' || c == '\r') {
            lexer->position++;
            lexer->column++;
        } else if (c == '\n') {
            lexer->position++;
            lexer->line++;
            lexer->column = 1;
        } else {
            break;
        }
    }
}

// 跳过注释
static void skipComment(Lexer* lexer) {
    if (lexer->source[lexer->position] == '/' &&
        lexer->source[lexer->position + 1] == '/') {
        // 单行注释
        while (lexer->source[lexer->position] != '\0' &&
               lexer->source[lexer->position] != '\n') {
            lexer->position++;
        }
        lexer->column = 1;
    } else if (lexer->source[lexer->position] == '/' &&
               lexer->source[lexer->position + 1] == '*') {
        // 多行注释
        lexer->position += 2;
        while (lexer->source[lexer->position] != '\0' &&
               !(lexer->source[lexer->position] == '*' &&
                 lexer->source[lexer->position + 1] == '/')) {
            if (lexer->source[lexer->position] == '\n') {
                lexer->line++;
                lexer->column = 1;
            } else {
                lexer->column++;
            }
            lexer->position++;
        }
        lexer->position += 2;
        lexer->column += 2;
    }
}

// 检查是否为关键字
static TokenType checkKeyword(const char* lexeme) {
    for (int i = 0; keywords[i].keyword != NULL; i++) {
        if (strcmp(lexeme, keywords[i].keyword) == 0) {
            return keywords[i].tokenType;
        }
    }
    return TOKEN_IDENTIFIER;
}

// 获取下一个token
Token getNextToken(Lexer* lexer) {
    skipWhitespace(lexer);

    while (lexer->source[lexer->position] == '/' &&
           (lexer->source[lexer->position + 1] == '/' ||
            lexer->source[lexer->position + 1] == '*')) {
        skipComment(lexer);
        skipWhitespace(lexer);
    }

    int startLine = lexer->line;
    int startCol = lexer->column;

    if (lexer->source[lexer->position] == '\0') {
        Token token = {TOKEN_EOF, NULL, lexer->line, lexer->column};
        return token;
    }

    char c = lexer->source[lexer->position];
    char lexeme[256];
    int lexemeLen = 0;

    // 数字
    if (isdigit(c)) {
        while (isdigit(lexer->source[lexer->position])) {
            lexeme[lexemeLen++] = lexer->source[lexer->position++];
            lexer->column++;
        }
        lexeme[lexemeLen] = '\0';

        Token token;
        token.type = TOKEN_NUMBER;
        token.lexeme = strdup(lexeme);
        token.line = startLine;
        token.column = startCol;
        return token;
    }

    // 标识符或关键字
    if (isalpha(c) || c == '_') {
        while (isalnum(lexer->source[lexer->position]) ||
               lexer->source[lexer->position] == '_') {
            lexeme[lexemeLen++] = lexer->source[lexer->position++];
            lexer->column++;
        }
        lexeme[lexemeLen] = '\0';

        Token token;
        token.type = checkKeyword(lexeme);
        token.lexeme = strdup(lexeme);
        token.line = startLine;
        token.column = startCol;
        return token;
    }

    // 字符字面量
    if (c == '\'') {
        lexer->position++;
        lexer->column++;

        if (lexer->source[lexer->position] != '\'') {
            lexeme[lexemeLen++] = lexer->source[lexer->position++];
            lexer->column++;
        }

        if (lexer->source[lexer->position] == '\'') {
            lexer->position++;
            lexer->column++;
        }

        lexeme[lexemeLen] = '\0';

        Token token;
        token.type = TOKEN_CHAR_LITERAL;
        token.lexeme = strdup(lexeme);
        token.line = startLine;
        token.column = startCol;
        return token;
    }

    // 字符串字面量（简化处理）
    if (c == '"') {
        lexer->position++;
        lexer->column++;

        while (lexer->source[lexer->position] != '"' &&
               lexer->source[lexer->position] != '\0') {
            lexeme[lexemeLen++] = lexer->source[lexer->position++];
            lexer->column++;
        }

        if (lexer->source[lexer->position] == '"') {
            lexer->position++;
            lexer->column++;
        }

        lexeme[lexemeLen] = '\0';

        Token token;
        token.type = TOKEN_STRING;
        token.lexeme = strdup(lexeme);
        token.line = startLine;
        token.column = startCol;
        return token;
    }

    // 运算符和分隔符
    lexer->position++;
    lexer->column++;

    switch (c) {
        case '+': {
            Token token = {TOKEN_PLUS, NULL, startLine, startCol};
            return token;
        }
        case '-': {
            Token token = {TOKEN_MINUS, NULL, startLine, startCol};
            return token;
        }
        case '*': {
            Token token = {TOKEN_MULTIPLY, NULL, startLine, startCol};
            return token;
        }
        case '/': {
            Token token = {TOKEN_DIVIDE, NULL, startLine, startCol};
            return token;
        }
        case '%': {
            Token token = {TOKEN_MODULO, NULL, startLine, startCol};
            return token;
        }
        case '(': {
            Token token = {TOKEN_LPAREN, NULL, startLine, startCol};
            return token;
        }
        case ')': {
            Token token = {TOKEN_RPAREN, NULL, startLine, startCol};
            return token;
        }
        case '{': {
            Token token = {TOKEN_LBRACE, NULL, startLine, startCol};
            return token;
        }
        case '}': {
            Token token = {TOKEN_RBRACE, NULL, startLine, startCol};
            return token;
        }
        case ';': {
            Token token = {TOKEN_SEMICOLON, NULL, startLine, startCol};
            return token;
        }
        case ',': {
            Token token = {TOKEN_COMMA, NULL, startLine, startCol};
            return token;
        }
        case '=':
            if (lexer->source[lexer->position] == '=') {
                lexer->position++;
                lexer->column++;
                Token token = {TOKEN_EQ, NULL, startLine, startCol};
                return token;
            }
            return (Token){TOKEN_ASSIGN, NULL, startLine, startCol};
        case '!':
            if (lexer->source[lexer->position] == '=') {
                lexer->position++;
                lexer->column++;
                Token token = {TOKEN_NE, NULL, startLine, startCol};
                return token;
            }
            return (Token){TOKEN_NOT, NULL, startLine, startCol};
        case '<':
            if (lexer->source[lexer->position] == '=') {
                lexer->position++;
                lexer->column++;
                Token token = {TOKEN_LE, NULL, startLine, startCol};
                return token;
            }
            return (Token){TOKEN_LT, NULL, startLine, startCol};
        case '>':
            if (lexer->source[lexer->position] == '=') {
                lexer->position++;
                lexer->column++;
                Token token = {TOKEN_GE, NULL, startLine, startCol};
                return token;
            }
            return (Token){TOKEN_GT, NULL, startLine, startCol};
        case '&':
            if (lexer->source[lexer->position] == '&') {
                lexer->position++;
                lexer->column++;
                Token token = {TOKEN_AND, NULL, startLine, startCol};
                return token;
            }
            break;
        case '|':
            if (lexer->source[lexer->position] == '|') {
                lexer->position++;
                lexer->column++;
                Token token = {TOKEN_OR, NULL, startLine, startCol};
                return token;
            }
            break;
        default:
            break;
    }

    Token token = {TOKEN_UNKNOWN, NULL, startLine, startCol};
    return token;
}

// 预读token
Token peekToken(Lexer* lexer, int lookahead) {
    Lexer tempLexer = *lexer;
    Token token = {TOKEN_UNKNOWN, NULL, 0, 0};

    for (int i = 0; i <= lookahead; i++) {
        token = getNextToken(&tempLexer);
    }

    return token;
}

// 释放token
void freeToken(Token token) {
    if (token.lexeme) {
        free(token.lexeme);
    }
}

// 打印token
void printToken(Token token) {
    const char* typeNames[] = {
        "EOF", "INT", "CHAR", "VOID", "IF", "ELSE", "WHILE", "FOR", "RETURN",
        "IDENTIFIER", "NUMBER", "CHAR_LITERAL", "STRING",
        "PLUS", "MINUS", "MULTIPLY", "DIVIDE", "MODULO", "ASSIGN",
        "EQ", "NE", "LT", "GT", "LE", "GE",
        "AND", "OR", "NOT",
        "LPAREN", "RPAREN", "LBRACE", "RBRACE", "SEMICOLON", "COMMA",
        "UNKNOWN"
    };

    if (token.lexeme) {
        printf("Token: %-15s %-20s (line %d, col %d)\n",
               typeNames[token.type], token.lexeme, token.line, token.column);
    } else {
        printf("Token: %-15s %20s (line %d, col %d)\n",
               typeNames[token.type], "", token.line, token.column);
    }
}
