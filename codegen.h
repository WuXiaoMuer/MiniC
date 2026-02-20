#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"

// 代码生成器状态
typedef struct {
    FILE* output;
    int labelCounter;
    int stackOffset;
} CodeGen;

// 初始化代码生成器
void initCodeGen(CodeGen* gen, FILE* output);

// 生成代码
void generateCode(CodeGen* gen, ASTNode* node);

#endif
