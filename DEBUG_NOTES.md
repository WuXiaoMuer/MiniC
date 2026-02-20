# 段错误修复 - 调试步骤

## 添加的调试输出

### 1. 每个主要步骤都添加状态打印
```c
printf("Initializing lexer...\n");
printf("=== Lexical Analysis ===\n");
printf("Reinitializing lexer for parsing...\n");
printf("Initializing parser...\n");
printf("Starting parse...\n");
printf("Parse returned, checking result...\n");
```

### 2. 添加 fflush(stdout)
确保输出立即显示，帮助定位崩溃位置。

### 3. 代码生成阶段的安全检查
```c
generateFunction: 检查 node->type 和 node->func.name
generateCode: 检查 node->type
calculateStackSize: 检查 node 是否为 NULL
```

### 4. 禁用 AST 打印
临时禁用 `printAST()` 以排除它导致段错误的可能性。

## 预期输出

正常情况下应该看到：
```
Initializing lexer...
=== Lexical Analysis ===
Token: ...
Total tokens: XX

Reinitializing lexer for parsing...
Initializing parser...
=== Parsing ===
Starting parse...
Parse returned, checking result...
Parsing successful!

=== Abstract Syntax Tree ===
AST pointer: 0x555555XXXXXXXX
AST type: 0
Skipping AST print for debugging...

=== Code Generation ===
Initializing code generator...
Generating code...
Assembly code generated!
Cleaning up...
Done!
```

## 如果仍然崩溃

根据最后一条输出消息，可以定位问题阶段：
- **"Total tokens: XX" 之后崩溃** → 解析器问题
- **"Starting parse..." 之后崩溃** → parse() 函数内部
- **"Parse returned, checking result..." 之后崩溃** → AST 节点访问错误
- **"Generating code..." 之后崩溃** → 代码生成器问题

## 下一步

提交后查看 GitHub Actions 日志，看最后一条消息是什么，这将帮助我们定位问题。
