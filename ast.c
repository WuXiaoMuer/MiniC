#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"

// 创建程序节点
ASTNode* createProgram(ASTNode* functions) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_PROGRAM;
    node->program.functions = functions;
    return node;
}

// 创建函数节点
ASTNode* createFunction(char* name, DataType returnType, ASTNode* params, ASTNode* body) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_FUNCTION;
    node->dataType = returnType;
    node->func.name = strdup(name);
    node->func.params = params;
    node->func.body = body;
    return node;
}

// 创建参数节点
ASTNode* createParam(char* name, DataType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_PARAM;
    node->dataType = type;
    node->var.name = strdup(name);
    node->var.next = NULL;
    node->var.init = NULL;
    return node;
}

// 创建代码块节点
ASTNode* createBlock(ASTNode* declarations, ASTNode* statements) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_BLOCK;
    node->block.declarations = declarations;
    node->block.statements = statements;
    return node;
}

// 创建整数字面量
ASTNode* createIntLiteral(int value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_INT_LITERAL;
    node->dataType = TYPE_INT;
    node->intVal = value;
    return node;
}

// 创建字符字面量
ASTNode* createCharLiteral(char value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_CHAR_LITERAL;
    node->dataType = TYPE_CHAR;
    node->charVal = value;
    return node;
}

// 创建变量引用
ASTNode* createVariable(char* name) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_VARIABLE;
    node->var.name = strdup(name);
    node->var.next = NULL;
    node->var.init = NULL;
    return node;
}

// 创建二元运算
ASTNode* createBinaryOp(Operator op, ASTNode* left, ASTNode* right) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_BINARY_OP;
    node->op = op;
    node->binary.left = left;
    node->binary.right = right;
    return node;
}

// 创建一元运算
ASTNode* createUnaryOp(Operator op, ASTNode* expr) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_UNARY_OP;
    node->op = op;
    node->unary.expr = expr;
    node->unary.next = NULL;
    return node;
}

// 创建赋值语句
ASTNode* createAssignment(char* name, ASTNode* expr) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_ASSIGNMENT;
    node->var.name = strdup(name);
    node->var.init = expr;
    node->var.next = NULL;
    return node;
}

// 创建if语句
ASTNode* createIf(ASTNode* condition, ASTNode* thenBlock, ASTNode* elseBlock) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_IF;
    node->ifStmt.condition = condition;
    node->ifStmt.thenBlock = thenBlock;
    node->ifStmt.elseBlock = elseBlock;
    return node;
}

// 创建while循环
ASTNode* createWhile(ASTNode* condition, ASTNode* body) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_WHILE;
    node->whileStmt.condition = condition;
    node->whileStmt.body = body;
    return node;
}

// 创建for循环
ASTNode* createFor(ASTNode* init, ASTNode* condition, ASTNode* increment, ASTNode* body) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_FOR;
    node->forStmt.init = init;
    node->forStmt.condition = condition;
    node->forStmt.increment = increment;
    node->forStmt.body = body;
    return node;
}

// 创建return语句
ASTNode* createReturn(ASTNode* expr) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_RETURN;
    node->returnStmt.expr = expr;
    return node;
}

// 创建函数调用
ASTNode* createCall(char* name, ASTNode* args) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_CALL;
    node->var.name = strdup(name);
    node->var.init = NULL;
    node->var.next = NULL;
    node->call.funcName = createVariable(name);
    node->call.args = args;
    return node;
}

// 释放AST
void freeAST(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case NODE_PROGRAM:
            freeAST(node->program.functions);
            break;
        case NODE_FUNCTION:
            free(node->func.name);
            freeAST(node->func.params);
            freeAST(node->func.body);
            break;
        case NODE_PARAM:
            free(node->var.name);
            freeAST(node->var.next);
            break;
        case NODE_BLOCK:
            freeAST(node->block.declarations);
            freeAST(node->block.statements);
            break;
        case NODE_BINARY_OP:
            freeAST(node->binary.left);
            freeAST(node->binary.right);
            break;
        case NODE_UNARY_OP:
            freeAST(node->unary.expr);
            break;
        case NODE_ASSIGNMENT:
            free(node->var.name);
            freeAST(node->var.init);
            break;
        case NODE_IF:
            freeAST(node->ifStmt.condition);
            freeAST(node->ifStmt.thenBlock);
            freeAST(node->ifStmt.elseBlock);
            break;
        case NODE_WHILE:
            freeAST(node->whileStmt.condition);
            freeAST(node->whileStmt.body);
            break;
        case NODE_FOR:
            freeAST(node->forStmt.init);
            freeAST(node->forStmt.condition);
            freeAST(node->forStmt.increment);
            freeAST(node->forStmt.body);
            break;
        case NODE_RETURN:
            freeAST(node->returnStmt.expr);
            break;
        case NODE_CALL:
            free(node->var.name);
            freeAST(node->call.funcName);
            freeAST(node->call.args);
            break;
        case NODE_VARIABLE:
            free(node->var.name);
            break;
        case NODE_DECLARATION:
            free(node->var.name);
            freeAST(node->var.init);
            freeAST(node->var.next);
            break;
        default:
            break;
    }
    free(node);
}
