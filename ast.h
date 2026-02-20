#ifndef AST_H
#define AST_H

// 节点类型枚举
typedef enum {
    NODE_PROGRAM,
    NODE_FUNCTION,
    NODE_PARAM_LIST,
    NODE_PARAM,
    NODE_BLOCK,
    NODE_INT_LITERAL,
    NODE_CHAR_LITERAL,
    NODE_VARIABLE,
    NODE_BINARY_OP,
    NODE_UNARY_OP,
    NODE_ASSIGNMENT,
    NODE_IF,
    NODE_WHILE,
    NODE_FOR,
    NODE_RETURN,
    NODE_CALL,
    NODE_DECLARATION,
    NODE_EMPTY
} NodeType;

// 运算符类型
typedef enum {
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD,
    OP_EQ, OP_NE, OP_LT, OP_GT, OP_LE, OP_GE,
    OP_AND, OP_OR, OP_NOT,
    OP_ASSIGN
} Operator;

// 数据类型
typedef enum {
    TYPE_INT,
    TYPE_CHAR,
    TYPE_VOID
} DataType;

// AST节点结构
typedef struct ASTNode {
    NodeType type;
    DataType dataType;
    union {
        int intVal;
        char charVal;
        Operator op;
        struct {
            struct ASTNode* left;
            struct ASTNode* right;
        } binary;
        struct {
            struct ASTNode* expr;
            struct ASTNode* next;
        } unary;
        struct {
            char* name;
            struct ASTNode* init;
            struct ASTNode* next;  // 用于参数列表和声明列表
        } var;
        struct {
            char* name;
            struct ASTNode* params;
            struct ASTNode* body;
        } func;
        struct {
            struct ASTNode* condition;
            struct ASTNode* thenBlock;
            struct ASTNode* elseBlock;
        } ifStmt;
        struct {
            struct ASTNode* condition;
            struct ASTNode* body;
        } whileStmt;
        struct {
            struct ASTNode* init;
            struct ASTNode* condition;
            struct ASTNode* increment;
            struct ASTNode* body;
        } forStmt;
        struct {
            struct ASTNode* expr;
        } returnStmt;
        struct {
            struct ASTNode* funcName;
            struct ASTNode* args;
        } call;
        struct {
            struct ASTNode* declarations;
            struct ASTNode* statements;
        } block;
        struct {
            struct ASTNode* functions;
        } program;
    };
} ASTNode;

// AST节点创建函数
ASTNode* createProgram(ASTNode* functions);
ASTNode* createFunction(char* name, DataType returnType, ASTNode* params, ASTNode* body);
ASTNode* createParam(char* name, DataType type);
ASTNode* createBlock(ASTNode* declarations, ASTNode* statements);
ASTNode* createIntLiteral(int value);
ASTNode* createCharLiteral(char value);
ASTNode* createVariable(char* name);
ASTNode* createBinaryOp(Operator op, ASTNode* left, ASTNode* right);
ASTNode* createUnaryOp(Operator op, ASTNode* expr);
ASTNode* createAssignment(char* name, ASTNode* expr);
ASTNode* createIf(ASTNode* condition, ASTNode* thenBlock, ASTNode* elseBlock);
ASTNode* createWhile(ASTNode* condition, ASTNode* body);
ASTNode* createFor(ASTNode* init, ASTNode* condition, ASTNode* increment, ASTNode* body);
ASTNode* createReturn(ASTNode* expr);
ASTNode* createCall(char* name, ASTNode* args);

// 释放AST
void freeAST(ASTNode* node);

#endif
