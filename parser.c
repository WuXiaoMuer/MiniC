#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "parser.h"

// 错误处理
static void error(const char* message) {
    fprintf(stderr, "Parser Error: %s\n", message);
    exit(1);
}

// 初始化解析器
void initParser(Parser* parser, Lexer* lexer) {
    parser->lexer = lexer;
    parser->currentToken = getNextToken(lexer);
    parser->nextToken = getNextToken(lexer);
}

// 前进到下一个token
static void advance(Parser* parser) {
    parser->currentToken = parser->nextToken;
    parser->nextToken = getNextToken(parser->lexer);
}

// 期望当前token为指定类型
static void expect(Parser* parser, TokenType type) {
    if (parser->currentToken.type != type) {
        error("Unexpected token");
    }
    advance(parser);
}

// 检查当前token类型
static bool match(Parser* parser, TokenType type) {
    if (parser->currentToken.type == type) {
        advance(parser);
        return true;
    }
    return false;
}

// 初始化符号表
void initSymbolTable(SymbolTable* table) {
    table->head = NULL;
    table->stackOffset = 0;
}

// 添加符号
void addSymbol(SymbolTable* table, char* name, DataType type) {
    Symbol* symbol = (Symbol*)malloc(sizeof(Symbol));
    symbol->name = strdup(name);
    symbol->type = type;
    symbol->offset = table->stackOffset;
    symbol->size = 4;  // int/char 都是4字节
    symbol->next = table->head;
    table->head = symbol;
    table->stackOffset += 4;
}

// 查找符号
Symbol* findSymbol(SymbolTable* table, char* name) {
    Symbol* symbol = table->head;
    while (symbol) {
        if (strcmp(symbol->name, name) == 0) {
            return symbol;
        }
        symbol = symbol->next;
    }
    return NULL;
}

// 释放符号表
void freeSymbolTable(SymbolTable* table) {
    Symbol* symbol = table->head;
    while (symbol) {
        Symbol* next = symbol->next;
        free(symbol->name);
        free(symbol);
        symbol = next;
    }
    table->head = NULL;
}

// 解析类型
static DataType parseType(Parser* parser) {
    if (match(parser, TOKEN_INT)) {
        return TYPE_INT;
    } else if (match(parser, TOKEN_CHAR)) {
        return TYPE_CHAR;
    } else if (match(parser, TOKEN_VOID)) {
        return TYPE_VOID;
    }
    error("Expected type");
    return TYPE_INT;
}

// 解析表达式（简化版）
static ASTNode* parseExpression(Parser* parser);

// 解析因子
static ASTNode* parseFactor(Parser* parser) {
    if (parser->currentToken.type == TOKEN_NUMBER) {
        int value = atoi(parser->currentToken.lexeme);
        advance(parser);
        return createIntLiteral(value);
    } else if (parser->currentToken.type == TOKEN_CHAR_LITERAL) {
        char value = parser->currentToken.lexeme[0];
        advance(parser);
        return createCharLiteral(value);
    } else if (parser->currentToken.type == TOKEN_IDENTIFIER) {
        char* name = strdup(parser->currentToken.lexeme);
        advance(parser);

        // 检查是否是函数调用
        if (match(parser, TOKEN_LPAREN)) {
            ASTNode* args = NULL;
            if (parser->currentToken.type != TOKEN_RPAREN) {
                args = parseExpression(parser);
                while (match(parser, TOKEN_COMMA)) {
                    ASTNode* arg = parseExpression(parser);
                    args->binary.right = arg;
                }
            }
            expect(parser, TOKEN_RPAREN);
            return createCall(name, args);
        }

        return createVariable(name);
    } else if (match(parser, TOKEN_LPAREN)) {
        ASTNode* expr = parseExpression(parser);
        expect(parser, TOKEN_RPAREN);
        return expr;
    } else if (match(parser, TOKEN_NOT)) {
        ASTNode* expr = parseFactor(parser);
        return createUnaryOp(OP_NOT, expr);
    }

    error("Expected factor");
    return NULL;
}

// 解析乘除表达式
static ASTNode* parseTerm(Parser* parser) {
    ASTNode* left = parseFactor(parser);

    while (parser->currentToken.type == TOKEN_MULTIPLY ||
           parser->currentToken.type == TOKEN_DIVIDE ||
           parser->currentToken.type == TOKEN_MODULO) {
        TokenType op = parser->currentToken.type;
        advance(parser);
        ASTNode* right = parseFactor(parser);

        Operator operator;
        if (op == TOKEN_MULTIPLY) operator = OP_MUL;
        else if (op == TOKEN_DIVIDE) operator = OP_DIV;
        else operator = OP_MOD;

        left = createBinaryOp(operator, left, right);
    }

    return left;
}

// 解析加减表达式
static ASTNode* parseAdditive(Parser* parser) {
    ASTNode* left = parseTerm(parser);

    while (parser->currentToken.type == TOKEN_PLUS ||
           parser->currentToken.type == TOKEN_MINUS) {
        TokenType op = parser->currentToken.type;
        advance(parser);
        ASTNode* right = parseTerm(parser);

        Operator operator = (op == TOKEN_PLUS) ? OP_ADD : OP_SUB;
        left = createBinaryOp(operator, left, right);
    }

    return left;
}

// 解析比较表达式
static ASTNode* parseComparison(Parser* parser) {
    ASTNode* left = parseAdditive(parser);

    while (parser->currentToken.type == TOKEN_LT ||
           parser->currentToken.type == TOKEN_GT ||
           parser->currentToken.type == TOKEN_LE ||
           parser->currentToken.type == TOKEN_GE ||
           parser->currentToken.type == TOKEN_EQ ||
           parser->currentToken.type == TOKEN_NE) {
        TokenType op = parser->currentToken.type;
        advance(parser);
        ASTNode* right = parseAdditive(parser);

        Operator operator;
        switch (op) {
            case TOKEN_LT: operator = OP_LT; break;
            case TOKEN_GT: operator = OP_GT; break;
            case TOKEN_LE: operator = OP_LE; break;
            case TOKEN_GE: operator = OP_GE; break;
            case TOKEN_EQ: operator = OP_EQ; break;
            case TOKEN_NE: operator = OP_NE; break;
            default: operator = OP_EQ; break;
        }

        left = createBinaryOp(operator, left, right);
    }

    return left;
}

// 解析逻辑表达式
static ASTNode* parseLogical(Parser* parser) {
    ASTNode* left = parseComparison(parser);

    while (parser->currentToken.type == TOKEN_AND ||
           parser->currentToken.type == TOKEN_OR) {
        TokenType op = parser->currentToken.type;
        advance(parser);
        ASTNode* right = parseComparison(parser);

        Operator operator = (op == TOKEN_AND) ? OP_AND : OP_OR;
        left = createBinaryOp(operator, left, right);
    }

    return left;
}

// 解析表达式
static ASTNode* parseExpression(Parser* parser) {
    return parseLogical(parser);
}

// 解析赋值
static ASTNode* parseAssignment(Parser* parser) {
    if (parser->currentToken.type == TOKEN_IDENTIFIER &&
        parser->nextToken.type == TOKEN_ASSIGN) {
        char* name = strdup(parser->currentToken.lexeme);
        advance(parser);  // identifier
        expect(parser, TOKEN_ASSIGN);  // =
        ASTNode* expr = parseExpression(parser);
        expect(parser, TOKEN_SEMICOLON);
        return createAssignment(name, expr);
    }
    return NULL;
}

// 解析声明
static ASTNode* parseDeclaration(Parser* parser, SymbolTable* table) {
    DataType type = parseType(parser);

    if (parser->currentToken.type != TOKEN_IDENTIFIER) {
        error("Expected identifier after type");
    }

    char* name = strdup(parser->currentToken.lexeme);
    addSymbol(table, name, type);
    advance(parser);

    ASTNode* init = NULL;
    if (match(parser, TOKEN_ASSIGN)) {
        init = parseExpression(parser);
    }

    expect(parser, TOKEN_SEMICOLON);

    return createAssignment(name, init);
}

// 解析语句
static ASTNode* parseStatement(Parser* parser, SymbolTable* table);

// 解析代码块
static ASTNode* parseBlock(Parser* parser, SymbolTable* table) {
    (void)table;  // 使用本地符号表
    expect(parser, TOKEN_LBRACE);

    SymbolTable localTable;
    initSymbolTable(&localTable);

    ASTNode* declarations = NULL;
    ASTNode* lastDecl = NULL;

    // 声明部分
    while (parser->currentToken.type == TOKEN_INT ||
           parser->currentToken.type == TOKEN_CHAR) {
        ASTNode* decl = parseDeclaration(parser, &localTable);
        if (!declarations) {
            declarations = decl;
            lastDecl = decl;
        } else {
            lastDecl->var.next = decl;
            lastDecl = decl;
        }
    }

    // 语句部分
    ASTNode* statements = NULL;
    ASTNode* lastStmt = NULL;

    while (parser->currentToken.type != TOKEN_RBRACE &&
           parser->currentToken.type != TOKEN_EOF) {
        ASTNode* stmt = parseStatement(parser, &localTable);
        if (stmt) {
            if (!statements) {
                statements = stmt;
                lastStmt = stmt;
            } else {
                lastStmt->var.next = stmt;
                lastStmt = stmt;
            }
        }
    }

    expect(parser, TOKEN_RBRACE);

    freeSymbolTable(&localTable);

    return createBlock(declarations, statements);
}

// 解析if语句
static ASTNode* parseIf(Parser* parser, SymbolTable* table) {
    expect(parser, TOKEN_IF);
    expect(parser, TOKEN_LPAREN);
    ASTNode* condition = parseExpression(parser);
    expect(parser, TOKEN_RPAREN);
    ASTNode* thenBlock = parseBlock(parser, table);

    ASTNode* elseBlock = NULL;
    if (match(parser, TOKEN_ELSE)) {
        elseBlock = parseBlock(parser, table);
    }

    return createIf(condition, thenBlock, elseBlock);
}

// 解析while语句
static ASTNode* parseWhile(Parser* parser, SymbolTable* table) {
    expect(parser, TOKEN_WHILE);
    expect(parser, TOKEN_LPAREN);
    ASTNode* condition = parseExpression(parser);
    expect(parser, TOKEN_RPAREN);
    ASTNode* body = parseBlock(parser, table);
    return createWhile(condition, body);
}

// 解析for语句
static ASTNode* parseFor(Parser* parser, SymbolTable* table) {
    expect(parser, TOKEN_FOR);
    expect(parser, TOKEN_LPAREN);

    ASTNode* init = NULL;
    if (parser->currentToken.type != TOKEN_SEMICOLON) {
        if (parser->currentToken.type == TOKEN_INT ||
            parser->currentToken.type == TOKEN_CHAR) {
            init = parseDeclaration(parser, table);
        } else {
            init = parseAssignment(parser);
        }
    } else {
        expect(parser, TOKEN_SEMICOLON);
    }

    ASTNode* condition = NULL;
    if (parser->currentToken.type != TOKEN_SEMICOLON) {
        condition = parseExpression(parser);
    }
    expect(parser, TOKEN_SEMICOLON);

    ASTNode* increment = NULL;
    if (parser->currentToken.type != TOKEN_RPAREN) {
        increment = parseAssignment(parser);
        // for循环中的increment是表达式，不是赋值语句
        if (increment->type == NODE_ASSIGNMENT) {
            // 取出右边表达式
            increment = increment->var.init;
        }
    }
    expect(parser, TOKEN_RPAREN);

    ASTNode* body = parseBlock(parser, table);
    return createFor(init, condition, increment, body);
}

// 解析return语句
static ASTNode* parseReturn(Parser* parser) {
    expect(parser, TOKEN_RETURN);
    ASTNode* expr = NULL;
    if (parser->currentToken.type != TOKEN_SEMICOLON) {
        expr = parseExpression(parser);
    }
    expect(parser, TOKEN_SEMICOLON);
    return createReturn(expr);
}

// 解析语句
static ASTNode* parseStatement(Parser* parser, SymbolTable* table) {
    Token token = parser->currentToken;

    if (token.type == TOKEN_IF) {
        return parseIf(parser, table);
    } else if (token.type == TOKEN_WHILE) {
        return parseWhile(parser, table);
    } else if (token.type == TOKEN_FOR) {
        return parseFor(parser, table);
    } else if (token.type == TOKEN_RETURN) {
        return parseReturn(parser);
    } else if (token.type == TOKEN_IDENTIFIER) {
        ASTNode* assign = parseAssignment(parser);
        if (assign) return assign;

        // 如果不是赋值，可能是表达式语句
        ASTNode* expr = parseExpression(parser);
        expect(parser, TOKEN_SEMICOLON);
        return expr;
    } else if (token.type == TOKEN_LBRACE) {
        return parseBlock(parser, table);
    }

    error("Expected statement");
    return NULL;
}

// 解析参数列表
static ASTNode* parseParameters(Parser* parser) {
    ASTNode* params = NULL;
    ASTNode* lastParam = NULL;

    if (parser->currentToken.type != TOKEN_RPAREN) {
        DataType type = parseType(parser);
        if (parser->currentToken.type != TOKEN_IDENTIFIER) {
            error("Expected parameter name");
        }
        char* name = strdup(parser->currentToken.lexeme);
        advance(parser);

        params = createParam(name, type);
        lastParam = params;

        while (match(parser, TOKEN_COMMA)) {
            type = parseType(parser);
            if (parser->currentToken.type != TOKEN_IDENTIFIER) {
                error("Expected parameter name");
            }
            name = strdup(parser->currentToken.lexeme);
            advance(parser);

            ASTNode* param = createParam(name, type);
            lastParam->var.next = param;
            lastParam = param;
        }
    }

    return params;
}

// 解析函数
static ASTNode* parseFunction(Parser* parser) {
    DataType returnType = parseType(parser);

    if (parser->currentToken.type != TOKEN_IDENTIFIER) {
        error("Expected function name");
    }
    char* name = strdup(parser->currentToken.lexeme);
    advance(parser);

    expect(parser, TOKEN_LPAREN);
    ASTNode* params = parseParameters(parser);
    expect(parser, TOKEN_RPAREN);

    SymbolTable table;
    initSymbolTable(&table);

    ASTNode* body = parseBlock(parser, &table);

    freeSymbolTable(&table);

    return createFunction(name, returnType, params, body);
}

// 解析程序
ASTNode* parse(Parser* parser) {
    ASTNode* functions = NULL;
    ASTNode* lastFunc = NULL;

    while (parser->currentToken.type != TOKEN_EOF) {
        ASTNode* func = parseFunction(parser);
        if (!functions) {
            functions = func;
            lastFunc = func;
        } else {
            lastFunc->var.next = func;
            lastFunc = func;
        }
    }

    return createProgram(functions);
}

// 释放解析器
void freeParser(Parser* parser) {
    // 释放token
    freeToken(parser->currentToken);
    freeToken(parser->nextToken);
}
