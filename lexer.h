#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdbool.h>

// Token类型
typedef enum {
    TOKEN_EOF,
    TOKEN_INT,
    TOKEN_CHAR,
    TOKEN_VOID,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_FOR,
    TOKEN_RETURN,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_CHAR_LITERAL,
    TOKEN_STRING,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_MODULO,
    TOKEN_ASSIGN,
    TOKEN_EQ,
    TOKEN_NE,
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_LE,
    TOKEN_GE,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_UNKNOWN
} TokenType;

// Token结构
typedef struct {
    TokenType type;
    char* lexeme;
    int line;
    int column;
} Token;

// 词法分析器状态
typedef struct {
    const char* source;
    int position;
    int line;
    int column;
} Lexer;

// 词法分析器函数
void initLexer(Lexer* lexer, const char* source);
Token getNextToken(Lexer* lexer);
Token peekToken(Lexer* lexer, int lookahead);
void printToken(Token token);
void freeToken(Token token);

#endif
