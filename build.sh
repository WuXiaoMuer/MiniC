#!/bin/bash

# MiniC 编译器构建和编译脚本

# 编译翻译器
echo "Building MiniiC compiler..."
gcc -o miniic *.c

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build successful!"
echo ""

# 编译和运行示例程序
if [ $# -eq 0 ]; then
    echo "Usage: ./build.sh <demo_file>"
    echo ""
    echo "Available demos:"
    echo "  demo/hello.c"
    echo "  demo/factorial.c"
    echo "  demo/fibonacci.c"
    echo "  demo/prime.c"
    exit 1
fi

DEMO_FILE=$1
BASENAME=$(basename "$DEMO_FILE" .c)
ASM_FILE="${BASENAME}.s"
OBJ_FILE="${BASENAME}.o"
EXE_FILE="${BASENAME}"

echo "Compiling $DEMO_FILE..."
./miniic $DEMO_FILE > $ASM_FILE

if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    exit 1
fi

echo "Assembling $ASM_FILE..."
nasm -f elf32 $ASM_FILE -o $OBJ_FILE

if [ $? -ne 0 ]; then
    echo "Assembly failed!"
    exit 1
fi

echo "Linking $OBJ_FILE..."
gcc -m32 $OBJ_FILE -o $EXE_FILE

if [ $? -ne 0 ]; then
    echo "Linking failed!"
    exit 1
fi

echo ""
echo "Running $EXE_FILE..."
echo "---"
./$EXE_FILE
EXIT_CODE=$?
echo "---"
echo "Exit code: $EXIT_CODE"
