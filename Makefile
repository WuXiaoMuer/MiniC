# MiniC 编译器 Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = miniic

# 源文件
SRCS = main.c lexer.c parser.c ast.c codegen.c
OBJS = $(SRCS:.c=.o)

# 默认目标
all: $(TARGET)

# 编译主程序
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Build complete: $(TARGET)"

# 编译目标文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 运行示例
run-hello: $(TARGET)
	./$(TARGET) demo/hello.c | tee hello.s
	nasm -f elf32 hello.s -o hello.o
	gcc -m32 hello.o -o hello
	./hello
	@echo "Exit code: $$?"

run-factorial: $(TARGET)
	./$(TARGET) demo/factorial.c | tee factorial.s
	nasm -f elf32 factorial.s -o factorial.o
	gcc -m32 factorial.o -o factorial
	./factorial
	@echo "Exit code: $$?"

run-fibonacci: $(TARGET)
	./$(TARGET) demo/fibonacci.c | tee fibonacci.s
	nasm -f elf32 fibonacci.s -o fibonacci.o
	gcc -m32 fibonacci.o -o fibonacci
	./fibonacci
	@echo "Exit code: $$?"

run-prime: $(TARGET)
	./$(TARGET) demo/prime.c | tee prime.s
	nasm -f elf32 prime.s -o prime.o
	gcc -m32 prime.o -o prime
	./prime
	@echo "Exit code: $$?"

# 运行所有示例
run-all: run-hello run-factorial run-fibonacci run-prime

# 清理生成的文件
clean:
	rm -f $(TARGET) $(OBJS)
	rm -f *.s *.o hello.exe factorial.exe fibonacci.exe prime.exe

# 清理所有生成的文件（包括示例）
clean-all: clean
	rm -f hello factorial fibonacci prime

# 安装到系统目录（可选）
install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/

# 卸载
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# 依赖关系
main.o: main.c lexer.h parser.h codegen.h ast.h
lexer.o: lexer.c lexer.h
parser.o: parser.c parser.h lexer.h ast.h
ast.o: ast.c ast.h
codegen.o: codegen.c codegen.h ast.h

.PHONY: all clean clean-all install uninstall run-hello run-factorial run-fibonacci run-prime run-all
