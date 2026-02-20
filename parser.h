#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

// 语法分析器状态
typedef struct {
    Lexer* lexer;
    Token currentToken;
    Token nextToken;
} Parser;

// 符号表条目
typedef struct Symbol {
    char* name;
    DataType type;
    int offset;
    int size;
    struct Symbol* next;
} Symbol;

// 符号表
typedef struct {
    Symbol* head;
    int stackOffset;
} SymbolTable;

// 语法分析器函数
void initParser(Parser* parser, Lexer* lexer);
ASTNode* parse(Parser* parser);
void freeParser(Parser* parser);

// 符号表函数
void initSymbolTable(SymbolTable* table);
void addSymbol(SymbolTable* table, char* name, DataType type);
Symbol* findSymbol(SymbolTable* table, char* name);
void freeSymbolTable(SymbolTable* table);

#endif
