#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

// 初始化代码生成器
void initCodeGen(CodeGen* gen, FILE* output) {
    gen->output = output;
    gen->labelCounter = 0;
    gen->stackOffset = 0;
}

// 生成新标签
static char* newLabel(CodeGen* gen) {
    char* label = (char*)malloc(32);
    sprintf(label, ".L%d", gen->labelCounter++);
    return label;
}

// 生成表达式
static void generateExpression(CodeGen* gen, ASTNode* node);

// 生成二元运算
static void generateBinaryOp(CodeGen* gen, ASTNode* node) {
    generateExpression(gen, node->binary.left);
    fprintf(gen->output, "    push eax\n");

    generateExpression(gen, node->binary.right);
    fprintf(gen->output, "    pop ebx\n");

    switch (node->op) {
        case OP_ADD:
            fprintf(gen->output, "    add eax, ebx\n");
            break;
        case OP_SUB:
            fprintf(gen->output, "    sub eax, ebx\n");
            break;
        case OP_MUL:
            fprintf(gen->output, "    imul eax, ebx\n");
            break;
        case OP_DIV:
            fprintf(gen->output, "    cdq\n");
            fprintf(gen->output, "    idiv ebx\n");
            break;
        case OP_MOD:
            fprintf(gen->output, "    cdq\n");
            fprintf(gen->output, "    idiv ebx\n");
            fprintf(gen->output, "    mov eax, edx\n");
            break;
        case OP_EQ:
            fprintf(gen->output, "    cmp eax, ebx\n");
            fprintf(gen->output, "    mov eax, 0\n");
            fprintf(gen->output, "    sete al\n");
            break;
        case OP_NE:
            fprintf(gen->output, "    cmp eax, ebx\n");
            fprintf(gen->output, "    mov eax, 0\n");
            fprintf(gen->output, "    setne al\n");
            break;
        case OP_LT:
            fprintf(gen->output, "    cmp eax, ebx\n");
            fprintf(gen->output, "    mov eax, 0\n");
            fprintf(gen->output, "    setl al\n");
            break;
        case OP_GT:
            fprintf(gen->output, "    cmp eax, ebx\n");
            fprintf(gen->output, "    mov eax, 0\n");
            fprintf(gen->output, "    setg al\n");
            break;
        case OP_LE:
            fprintf(gen->output, "    cmp eax, ebx\n");
            fprintf(gen->output, "    mov eax, 0\n");
            fprintf(gen->output, "    setle al\n");
            break;
        case OP_GE:
            fprintf(gen->output, "    cmp eax, ebx\n");
            fprintf(gen->output, "    mov eax, 0\n");
            fprintf(gen->output, "    setge al\n");
            break;
        case OP_AND:
            fprintf(gen->output, "    and eax, ebx\n");
            break;
        case OP_OR:
            fprintf(gen->output, "    or eax, ebx\n");
            break;
        default:
            break;
    }
}

// 生成变量加载
static void generateVariable(CodeGen* gen, ASTNode* node, bool isLvalue) {
    const char* name = node->var.name;
    fprintf(gen->output, "    mov eax, [ebp-%d]\n", node->var.offset + 4);
}

// 生成表达式
static void generateExpression(CodeGen* gen, ASTNode* node) {
    switch (node->type) {
        case NODE_INT_LITERAL:
            fprintf(gen->output, "    mov eax, %d\n", node->intVal);
            break;
        case NODE_CHAR_LITERAL:
            fprintf(gen->output, "    mov eax, %d\n", (int)node->charVal);
            break;
        case NODE_VARIABLE:
            generateVariable(gen, node, false);
            break;
        case NODE_BINARY_OP:
            generateBinaryOp(gen, node);
            break;
        case NODE_ASSIGNMENT: {
            const char* name = node->var.name;
            int offset = node->var.offset;
            generateExpression(gen, node->var.init);
            fprintf(gen->output, "    mov [ebp-%d], eax\n", offset + 4);
            break;
        }
        case NODE_CALL:
            // 简化处理：暂时不支持函数调用参数传递
            fprintf(gen->output, "    call %s\n", node->var.name);
            break;
        default:
            break;
    }
}

// 生成语句
static void generateStatement(CodeGen* gen, ASTNode* node);

// 生成代码块
static void generateBlock(CodeGen* gen, ASTNode* node) {
    if (!node) return;

    // 生成声明
    ASTNode* decl = node->block.declarations;
    while (decl) {
        // 在代码块开始时已经分配了栈空间
        decl = decl->var.next;
    }

    // 生成语句
    ASTNode* stmt = node->block.statements;
    while (stmt) {
        generateStatement(gen, stmt);
        stmt = stmt->var.next;
    }
}

// 生成if语句
static void generateIf(CodeGen* gen, ASTNode* node) {
    char* elseLabel = newLabel(gen);
    char* endLabel = newLabel(gen);

    generateExpression(gen, node->ifStmt.condition);
    fprintf(gen->output, "    cmp eax, 0\n");
    fprintf(gen->output, "    je %s\n", elseLabel);

    generateBlock(gen, node->ifStmt.thenBlock);
    fprintf(gen->output, "    jmp %s\n", endLabel);

    fprintf(gen->output, "%s:\n", elseLabel);
    if (node->ifStmt.elseBlock) {
        generateBlock(gen, node->ifStmt.elseBlock);
    }

    fprintf(gen->output, "%s:\n", endLabel);

    free(elseLabel);
    free(endLabel);
}

// 生成while循环
static void generateWhile(CodeGen* gen, ASTNode* node) {
    char* startLabel = newLabel(gen);
    char* endLabel = newLabel(gen);

    fprintf(gen->output, "%s:\n", startLabel);
    generateExpression(gen, node->whileStmt.condition);
    fprintf(gen->output, "    cmp eax, 0\n");
    fprintf(gen->output, "    je %s\n", endLabel);

    generateBlock(gen, node->whileStmt.body);
    fprintf(gen->output, "    jmp %s\n", startLabel);

    fprintf(gen->output, "%s:\n", endLabel);

    free(startLabel);
    free(endLabel);
}

// 生成for循环
static void generateFor(CodeGen* gen, ASTNode* node) {
    char* startLabel = newLabel(gen);
    char* endLabel = newLabel(gen);
    char* incLabel = newLabel(gen);

    // 初始化
    if (node->forStmt.init) {
        generateStatement(gen, node->forStmt.init);
    }

    fprintf(gen->output, "%s:\n", startLabel);

    // 条件
    if (node->forStmt.condition) {
        generateExpression(gen, node->forStmt.condition);
        fprintf(gen->output, "    cmp eax, 0\n");
        fprintf(gen->output, "    je %s\n", endLabel);
    }

    // 循环体
    generateBlock(gen, node->forStmt.body);
    fprintf(gen->output, "    jmp %s\n", incLabel);

    // 增量
    fprintf(gen->output, "%s:\n", incLabel);
    if (node->forStmt.increment) {
        generateExpression(gen, node->forStmt.increment);
    }
    fprintf(gen->output, "    jmp %s\n", startLabel);

    fprintf(gen->output, "%s:\n", endLabel);

    free(startLabel);
    free(endLabel);
    free(incLabel);
}

// 生成return语句
static void generateReturn(CodeGen* gen, ASTNode* node) {
    if (node->returnStmt.expr) {
        generateExpression(gen, node->returnStmt.expr);
    }
    fprintf(gen->output, "    leave\n");
    fprintf(gen->output, "    ret\n");
}

// 生成语句
static void generateStatement(CodeGen* gen, ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case NODE_ASSIGNMENT:
            generateExpression(gen, node);
            break;
        case NODE_IF:
            generateIf(gen, node);
            break;
        case NODE_WHILE:
            generateWhile(gen, node);
            break;
        case NODE_FOR:
            generateFor(gen, node);
            break;
        case NODE_RETURN:
            generateReturn(gen, node);
            break;
        case NODE_BLOCK:
            generateBlock(gen, node);
            break;
        case NODE_CALL:
            generateExpression(gen, node);
            break;
        default:
            if (node->type == NODE_INT_LITERAL ||
                node->type == NODE_CHAR_LITERAL ||
                node->type == NODE_VARIABLE ||
                node->type == NODE_BINARY_OP) {
                generateExpression(gen, node);
            }
            break;
    }
}

// 计算栈大小
static int calculateStackSize(ASTNode* node) {
    int size = 0;
    if (node->type == NODE_BLOCK && node->block.declarations) {
        ASTNode* decl = node->block.declarations;
        while (decl) {
            size += 4;
            decl = decl->var.next;
        }
    }
    return size;
}

// 生成函数
static void generateFunction(CodeGen* gen, ASTNode* node) {
    fprintf(gen->output, "\n");
    fprintf(gen->output, "global %s\n", node->func.name);
    fprintf(gen->output, "%s:\n", node->func.name);
    fprintf(gen->output, "    push ebp\n");
    fprintf(gen->output, "    mov ebp, esp\n");

    // 计算栈空间
    int stackSize = calculateStackSize(node->func.body);
    if (stackSize > 0) {
        fprintf(gen->output, "    sub esp, %d\n", stackSize);
    }

    generateBlock(gen, node->func.body);

    if (node->dataType != TYPE_VOID) {
        fprintf(gen->output, "    leave\n");
        fprintf(gen->output, "    ret\n");
    }
}

// 生成程序
void generateCode(CodeGen* gen, ASTNode* node) {
    // 文件头
    fprintf(gen->output, "; MiniC Generated Assembly\n");
    fprintf(gen->output, "section .text\n");

    ASTNode* func = node->program.functions;
    while (func) {
        generateFunction(gen, func);
        func = func->var.next;
    }

    // 数据段
    fprintf(gen->output, "\nsection .data\n");
}
