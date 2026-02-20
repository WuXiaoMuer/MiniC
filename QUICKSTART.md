# MiniC 快速入门

## 1. 项目简介

MiniC是一个完整的小型C语言编译器实现，包含：
- 词法分析器
- 语法分析器（递归下降）
- 抽象语法树（AST）
- x86汇编代码生成器
- 多个示例程序

## 2. 文件说明

### 源代码（9个文件）
- `main.c` - 主程序
- `lexer.c/h` - 词法分析器
- `parser.c/h` - 语法分析器
- `ast.c/h` - 抽象语法树
- `codegen.c/h` - 代码生成器

### 示例程序（4个）
- `demo/hello.c` - 基础示例
- `demo/factorial.c` - 递归
- `demo/fibonacci.c` - 循环
- `demo/prime.c` - 条件判断

### 文档（5个）
- `README.md` - 项目概述
- `ARCHITECTURE.md` - 架构详解
- `BUILD.md` - 构建指南
- `EXAMPLES.md` - 示例说明
- `TODO.md` - 改进方向

### 构建脚本（3个）
- `Makefile` - Linux构建
- `build.sh` - Linux自动编译
- `build.bat` - Windows脚本

## 3. 快速开始（推荐使用WSL）

### 步骤1：准备环境
如果你在Windows上，建议使用WSL：

1. 打开PowerShell（管理员）
2. 运行：`wsl --install`
3. 重启电脑，按照提示设置Ubuntu
4. 在WSL终端中安装依赖：
   ```bash
   sudo apt-get update
   sudo apt-get install gcc nasm make
   ```

### 步骤2：编译编译器
```bash
cd /mnt/c/Users/20969/Desktop/claw
make
```
或者：
```bash
gcc -o miniic *.c
```

### 步骤3：编译并运行示例
```bash
# 编译hello示例
./miniic demo/hello.c > hello.s

# 汇编
nasm -f elf32 hello.s -o hello.o

# 链接
gcc -m32 hello.o -o hello

# 运行
./hello
echo $?  # 查看结果：42
```

### 更简单的方式（使用Makefile）
```bash
make run-hello        # 运行hello
make run-factorial    # 运行阶乘
make run-fibonacci    # 运行斐波那契
make run-prime        # 运行素数判断
make run-all          # 运行所有示例
```

## 4. 查看编译过程

编译器会输出详细的编译过程：

```bash
./miniic demo/factorial.c
```

你会看到：
```
MiniC Compiler - v1.0
Compiling: demo/factorial.c

=== Lexical Analysis ===
Token: INT             int                  (line 1, col 1)
...

=== Parsing ===
Parsing successful!

=== Abstract Syntax Tree ===
Program
  Function: factorial
  ...

=== Code Generation ===
Assembly code generated!
```

生成的汇编代码会输出到标准输出，可以重定向到文件查看：
```bash
./miniic demo/factorial.c > factorial.s
cat factorial.s  # 查看汇编代码
```

## 5. 理解MiniC的限制

**注意**：MiniC是一个教学性质的编译器，有以下限制：

### 不支持的功能
- ❌ 标准I/O函数（无printf、scanf）
- ❌ 数组、指针、结构体
- ❌ 全局变量
- ❌ 预处理指令
- ❌ 标准库

### 如何查看结果
由于没有printf，所有结果通过程序退出码显示：
```bash
./your_program
echo $?  # 查看返回值
```

## 6. 自己写程序

创建一个C文件，符合MiniC支持的语法：

```c
// mytest.c
int add(int x, int y) {
    return x + y;
}

int main() {
    int result;
    result = add(10, 20);
    return result;
}
```

编译运行：
```bash
./miniic mytest.c > mytest.s
nasm -f elf32 mytest.s -o mytest.o
gcc -m32 mytest.o -o mytest
./mytest
echo $?  # 结果: 30
```

## 7. 常见问题

### Q: 程序运行没有输出
A: MiniC没有I/O支持，结果在退出码中，用`echo $?`查看

### Q: 如何查看调试信息
A: 编译器输出包含词法分析、AST等信息，仔细阅读即可

### Q: 编译错误怎么办
A: 检查你的代码语法，确保只使用MiniC支持的功能

### Q: Windows上没有gcc怎么办
A: 建议安装WSL，或者下载MinGW-w64

## 8. 学习路径

### 初学者
1. 运行所有示例程序
2. 看生成的汇编代码
3. 修改示例，观察变化
4. 阅读`ARCHITECTURE.md`

### 进阶学习者
1. 阅读`README.md`和`BUILD.md`
2. 研究源代码（从lexer.c开始）
3. 尝试添加新功能（参考`TODO.md`）
4. 阅读相关书籍（龙书）

### 练习建议
1. 添加一个新的运算符（如位运算&）
2. 改进错误处理
3. 实现简单的函数调用参数
4. 支持全局变量声明

## 9. 下一步

- 📖 阅读完整文档：`BUILD.md`, `EXAMPLES.md`
- 🏗️ 了解架构：`ARCHITECTURE.md`
- 🔧 改进项目：参考`TODO.md`
- 📚 学习资源：龙书《Compilers》

## 10. 获取帮助

如果你遇到问题：
1. 检查`BUILD.md`的故障排除部分
2. 确保环境依赖正确安装
3. 仔细阅读错误信息
4. 从简单示例开始，逐步增加复杂度

---

**祝你学习愉快！** 🦞
