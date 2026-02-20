# MiniC - 小型C语言编译器

一个从零编写的精简C语言编译器，支持C语言的核心子集。

## 特性

- **支持的功能：**
  - 基本数据类型：int, char
  - 变量声明和初始化
  - 算术运算：+, -, *, /, %
  - 比较运算：==, !=, <, >, <=, >=
  - 逻辑运算：&&, ||, !
  - 控制流：if, else, for, while
  - 函数定义和调用
  - return语句

- **架构：**
  - 词法分析器（Lexer）
  - 递归下降语法分析器（Parser）
  - 抽象语法树（AST）
  - x86汇编代码生成器

## 编译

```bash
gcc -o minic *.c
```

## 使用

```bash
./minic <source.c> > output.s
nasm -f elf32 output.s -o output.o
gcc -m32 output.o -o output
./output
```

## 项目结构

- `lexer.c/h` - 词法分析器
- `parser.c/h` - 语法分析器
- `ast.c/h` - AST定义
- `codegen.c/h` - 代码生成器
- `main.c` - 主程序
- `demo/` - 示例程序
