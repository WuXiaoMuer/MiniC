# MiniC 编译器 - 构建指南

## 系统要求

### Linux/Unix
- GCC 编译器
- NASM 汇编器
- Make (可选)

### Windows
- 可以使用以下任一方式：
  - WSL (Windows Subsystem for Linux)
  - MinGW-w64
  - Cygwin
  - 直接在Linux虚拟机中编译

## 在 Linux/WSL 中构建

### 1. 安装依赖

Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install gcc nasm make
```

Arch Linux:
```bash
sudo pacman -S gcc nasm make
```

### 2. 编译 MiniC 编译器

```bash
cd claw
gcc -o miniic *.c
```

### 3. 编译和运行示例程序

```bash
# 使用构建脚本
chmod +x build.sh
./build.sh demo/hello.c

# 或者手动编译
./miniic demo/hello.c > hello.s
nasm -f elf32 hello.s -o hello.o
gcc -m32 hello.o -o hello
./hello
```

## 编译后的示例

由于MiniC暂不支持标准I/O函数，所有示例都通过返回值输出结果：

### hello.c
简单的赋值和返回
```bash
./miniic demo/hello.c > hello.s
nasm -f elf32 hello.s -o hello.o
gcc -m32 hello.o -o hello
./hello
echo $?  # 查看 exit code，应该是 42
```

### factorial.c
递归计算阶乘
```bash
./miniic demo/factorial.c > factorial.s
nasm -f elf32 factorial.s -o factorial.o
gcc -m32 factorial.o -o factorial
./factorial
echo $?  # 输出: 120 (5!)
```

### fibonacci.c
循环计算斐波那契数列
```bash
./miniic demo/fibonacci.c > fibonacci.s
nasm -f elf32 fibonacci.s -o fibonacci.o
gcc -m32 fibonacci.o -o fibonacci
./fibonacci
echo $?  # 输出: 89 (第10个斐波那契数)
```

### prime.c
判断素数
```bash
./miniic demo/prime.c > prime.s
nasm -f elf32 prime.s -o prime.o
gcc -m32 prime.o -o prime
./prime
echo $?  # 输出: 1 (17是素数)
```

## 架构说明

### 代码流程

1. **词法分析器 (lexer.c)**
   - 将源代码分解为token
   - 识别关键字、标识符、运算符、分隔符
   - 支持单行和多行注释

2. **语法分析器 (parser.c)**
   - 使用递归下降解析技术
   - 构建抽象语法树(AST)
   - 支持函数定义、控制流、表达式

3. **代码生成器 (codegen.c)**
   - 遍历AST，生成x86-32位汇编代码
   - 使用栈帧管理局部变量
   - 生成条件跳转和循环标签

### 支持的语言特性

- **基本类型**: int, char, void
- **运算符**:
  - 算术: +, -, *, /, %
  - 比较: ==, !=, <, >, <=, >=
  - 逻辑: &&, ||, !
- **控制结构**:
  - if/else 条件语句
  - while 循环
  - for 循环
- **函数**: 定义、调用、参数、返回值
- **变量**: 声明、初始化、赋值

### 限制

- 不支持：
  - 指针
  - 数组
  - 结构体
  - 预处理指令(#include, #define等)
  - 标准库函数(printf, scanf等)
  - 全局变量
  - typedef
  - 枚举
  - switch语句
  - do-while循环
- 函数调用参数传递未完全实现
- 错误处理较为基础

## 扩展建议

如果想要扩展 MiniC，可以考虑：

1. 添加标准I/O支持
2. 实现数组类型
3. 添加指针支持
4. 支持预处理指令
5. 改进错误信息
6. 添加优化阶段
7. 生成更高效的汇编代码

## 故障排除

### 编译错误
- 确保GCC版本支持C99标准
- 检查是否有语法错误

### 链接错误
- 检查NASM版本是否支持elf32格式
- 确保使用-m32标志进行32位链接

### 运行时错误
- 检查栈溢出
- 验证变量作用域
- 确认函数返回值类型匹配
