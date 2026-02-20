#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "ast.h"

// 读取文件内容
static char* readFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(size + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    fclose(file);

    return buffer;
}

// 打印AST（简化版）
static void printAST(ASTNode* node, int indent) {
    if (!node) return;

    for (int i = 0; i < indent; i++) printf("  ");

    switch (node->type) {
        case NODE_PROGRAM:
            printf("Program\n");
            printAST(node->program.functions, indent + 1);
            break;
        case NODE_FUNCTION:
            printf("Function: %s\n", node->func.name ? node->func.name : "(null)");
            printf("  Params:\n");
            printAST(node->func.params, indent + 2);
            printf("  Body:\n");
            printAST(node->func.body, indent + 2);
            break;
        case NODE_BLOCK:
            printf("Block\n");
            printAST(node->block.declarations, indent + 1);
            printAST(node->block.statements, indent + 1);
            break;
        case NODE_INT_LITERAL:
            printf("Int: %d\n", node->intVal);
            break;
        case NODE_CHAR_LITERAL:
            printf("Char: '%c'\n", node->charVal);
            break;
        case NODE_VARIABLE:
            printf("Variable: %s\n", node->var.name ? node->var.name : "(null)");
            break;
        case NODE_ASSIGNMENT:
            printf("Assignment: %s\n", node->var.name ? node->var.name : "(null)");
            printAST(node->var.init, indent + 1);
            break;
        case NODE_IF:
            printf("If\n");
            printAST(node->ifStmt.condition, indent + 1);
            printAST(node->ifStmt.thenBlock, indent + 1);
            if (node->ifStmt.elseBlock) {
                printf("Else:\n");
                printAST(node->ifStmt.elseBlock, indent + 1);
            }
            break;
        case NODE_WHILE:
            printf("While\n");
            printAST(node->whileStmt.condition, indent + 1);
            printAST(node->whileStmt.body, indent + 1);
            break;
        case NODE_FOR:
            printf("For\n");
            printAST(node->forStmt.init, indent + 1);
            printAST(node->forStmt.condition, indent + 1);
            printAST(node->forStmt.increment, indent + 1);
            printAST(node->forStmt.body, indent + 1);
            break;
        case NODE_RETURN:
            printf("Return\n");
            printAST(node->returnStmt.expr, indent + 1);
            break;
        case NODE_BINARY_OP:
            printf("BinaryOp\n");
            printAST(node->binary.left, indent + 1);
            printAST(node->binary.right, indent + 1);
            break;
        default:
            if (node->var.next) {
                printAST(node->var.next, indent);
            }
            break;
    }

    if (node->var.next && node->type != NODE_ASSIGNMENT &&
        node->type != NODE_BLOCK && node->type != NODE_PROGRAM) {
        printAST(node->var.next, indent);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <source_file>\n", argv[0]);
        printf("Example: %s demo/hello.c > output.s\n", argv[0]);
        return 1;
    }

    // 读取源文件
    char* source = readFile(argv[1]);
    if (!source) {
        return 1;
    }

    printf("MiniC Compiler - v1.0\n");
    printf("Compiling: %s\n\n", argv[1]);

    // 词法分析
    Lexer lexer;
    printf("Initializing lexer...\n");
    initLexer(&lexer, source);

    printf("=== Lexical Analysis ===\n");
    int tokenCount = 0;
    Token token;
    do {
        token = getNextToken(&lexer);
        if (token.type != TOKEN_EOF && token.type != TOKEN_UNKNOWN) {
            printToken(token);
            tokenCount++;
        }
        if (token.type == TOKEN_UNKNOWN) {
            fprintf(stderr, "Error: Unknown token at line %d\n", token.line);
            free(source);
            return 1;
        }
        freeToken(token);
    } while (token.type != TOKEN_EOF);
    printf("Total tokens: %d\n\n", tokenCount);

    printf("Reinitializing lexer for parsing...\n");

    // 重新初始化lexer用于语法分析
    initLexer(&lexer, source);

    // 语法分析
    printf("Initializing parser...\n");
    Parser parser;
    initParser(&parser, &lexer);

    printf("=== Parsing ===\n");
    printf("Starting parse...\n");
    fflush(stdout);

    ASTNode* ast = parse(&parser);
    printf("Parse returned, checking result...\n");
    fflush(stdout);

    if (!ast) {
        printf("Parsing failed!\n");
        free(source);
        return 1;
    }
    printf("Parsing successful!\n\n");

    // 打印AST（用于调试）- 临时禁用以排查问题
    printf("=== Abstract Syntax Tree ===\n");
    printf("AST pointer: %p\n", (void*)ast);
    printf("AST type: %d\n", ast ? ast->type : -1);
    printf("Skipping AST print for debugging...\n\n");
    fflush(stdout);

    // 代码生成
    CodeGen codegen;
    printf("=== Code Generation ===\n");
    printf("Initializing code generator...\n");
    fflush(stdout);

    initCodeGen(&codegen, stdout);

    printf("Generating code...\n");
    fflush(stdout);

    generateCode(&codegen, ast);

    printf("Assembly code generated!\n");
    fflush(stdout);

    // 清理
    freeAST(ast);
    freeParser(&parser);
    free(source);

    return 0;
}
