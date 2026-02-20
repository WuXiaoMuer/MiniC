# MiniC 编译器架构文档

## 概述

MiniC是一个从零编写的精简C语言编译器，采用传统的多阶段编译器架构：前端（词法+语法）-> 中间表示（AST）-> 后端（代码生成）。

## 编译器流程

```
源代码 (.c)
    ↓
词法分析器 (Lexer)
    ↓
Token流
    ↓
语法分析器 (Parser)
    ↓
抽象语法树 (AST)
    ↓
代码生成器 (CodeGen)
    ↓
汇编代码 (.s)
    ↓
NASM汇编器
    ↓
目标文件 (.o)
    ↓
GCC链接器
    ↓
可执行文件
```

## 模块详解

### 1. 词法分析器 (lexer.c/h)

**职责**: 将源代码字符流转换为token流

**主要数据结构**:
```c
typedef enum {
    TOKEN_EOF, TOKEN_INT, TOKEN_CHAR, TOKEN_VOID,
    TOKEN_IF, TOKEN_ELSE, TOKEN_WHILE, TOKEN_FOR,
    TOKEN_RETURN, TOKEN_IDENTIFIER, TOKEN_NUMBER,
    TOKEN_CHAR_LITERAL, TOKEN_STRING,
    // ... 更多token类型
} TokenType;

typedef struct {
    TokenType type;
    char* lexeme;
    int line;
    int column;
} Token;
```

**核心算法**:
- 单字符lookahead用于识别多字符运算符(==, <=, >=等)
- 状态机模式识别不同token类型
- 自动跳过空白和注释

**支持的特性**:
- 单行注释 //
- 多行注释 /* */
- 数字、字符、字符串字面量
- 关键字识别

### 2. 语法分析器 (parser.c/h)

**职责**: 将token流转换为抽象语法树(AST)

**解析技术**: 递归下降解析器（自顶向下）

**文法设计**（简化版）:
```
Program      → Function*
Function     → Type Identifier '(' Parameters? ')' Block
Parameters   → (Param (',' Param)*)?
Param        → Type Identifier
Block        → '{' Declarations* Statements* '}'
Declaration  → Type Identifier ('=' Expression)? ';'
Statement    → If | While | For | Return | Assignment | Call | Block
If           → 'if' '(' Expression ')' Block ('else' Block)?
While        → 'while' '(' Expression ')' Block
For          → 'for' '(' (Declaration|Assignment)? ';' Expression? ';' Assignment? ')' Block
Expression   → LogicalExpression
Logical      → Comparison (('&&' | '||') Comparison)*
Comparison   → Additive (('<' | '>' | '<=' | '>=' | '==' | '!=') Additive)*
Additive     → Term (('+' | '-') Term)*
Term         → Factor (('*' | '/' | '%') Factor)*
Factor       → Number | CharLiteral | Identifier ('(' Arguments? ')')? | '(' Expression ')' | '!' Factor
Arguments    → (Expression (',' Expression)*)?
```

**优先级和结合性**:
1. 括号 ()
2. 一元运算符 ! (右结合)
3. 乘除模 */% (左结合)
4. 加减 +- (左结合)
5. 比较
6. 逻辑运算 && || (左结合)

**符号表**:
- 简单的链表实现
- 用于检查变量重复声明
- 计算栈偏移量

### 3. 抽象语法树 (ast.c/h)

**职责**: 表示程序结构

**节点类型**:
```c
typedef enum {
    NODE_PROGRAM,      // 程序入口
    NODE_FUNCTION,     // 函数定义
    NODE_BLOCK,        // 代码块
    NODE_INT_LITERAL,  // 整数字面量
    NODE_CHAR_LITERAL, // 字符字面量
    NODE_VARIABLE,     // 变量引用
    NODE_BINARY_OP,    // 二元运算
    NODE_UNARY_OP,     // 一元运算
    NODE_ASSIGNMENT,   // 赋值
    NODE_IF,           // if语句
    NODE_WHILE,        // while循环
    NODE_FOR,          // for循环
    NODE_RETURN,       // return语句
    NODE_CALL,         // 函数调用
    // ...
} NodeType;
```

**设计要点**:
- 使用union节省内存
- 节点链接使用var.next指针实现链表
- 支持类型信息（int/char/void）

### 4.代码生成器 (codegen.c/h)

**职责**: 遍历AST，生成x86汇编代码

**输出格式**: NASM语法，32位x86

**生成的代码结构**:

函数序言:
```asm
push ebp
mov ebp, esp
sub esp, <local_size>
```

函数结语:
```asm
leave
ret
```

**栈帧布局**:
```
高地址
+------------------+
|  参数 (n, n-1...)|
+------------------+
|  返回地址        |
+------------------+
|  旧的ebp         |  <- ebp
+------------------+
|  局部变量1       |  [ebp-4]
|  局部变量2       |  [ebp-8]
|  ...             |
+------------------+
低地址
```

**寄存器使用**:
- eax: 主累加器，保存表达式结果
- ebx: 临时存储
- esp: 栈指针
- ebp: 基址指针

**标签生成**:
- 自动编号标签 .L0, .L1, .L2...
- 用于条件跳转和循环

## 编译示例

### 输入代码
```c
int main() {
    int n;
    n = 42;
    return n;
}
```

### 生成的汇编
```asm
global main
main:
    push ebp
    mov ebp, esp
    sub esp, 4

    mov eax, 42
    mov [ebp-4], eax

    mov eax, [ebp-4]
    leave
    ret
```

## 设计决策

### 为什么选择递归下降解析器？
- 简单直观，易于理解和实现
- 手写代码，适合小型编译器
- 错误恢复相对容易
- 不需要复杂的解析器生成工具（如yacc/bison）

### 为什么输出汇编代码而不是直接生成机器码？
- 简化实现，利用NASM进行汇编
- 便于调试和代码审查
- 可以手动优化生成的汇编

### 为什么只支持32位x86？
- 调用约定简单
- 汇编语法相对统一
- 兼容性好

### 为什么不包含优化？
- 保持代码简单易读
- 优先考虑正确性而非性能
- 优化可以作为后续扩展

## 扩展方向

### 前端增强
1. 支持更多C语言特性（数组、指针、结构体）
2. 改进错误处理和恢复
3. 添加预处理阶段（宏、条件编译）
4. 支持更多标准类型

### 后端增强
1. 寄存器分配优化（不都用eax）
2. 常量折叠和传播
3. 死代码消除
4. 循环优化
5. 支持其他架构（ARM、x86-64）

### 代码质量
1. 更完善的单元测试
2. 代码覆盖率分析
3. 性能基准测试
4. 重构以提高可维护性

## 参考资源

- **编译原理**: 《Compilers: Principles, Techniques, and Tools》(龙书)
- **LLVM**: 现代编译器架构参考
- **NASM文档**: https://www.nasm.us/docs.php
- **System V ABI**: x86调用约定规范

## 许可

本项目是一个教育性质的编译器实现，欢迎学习和修改。
