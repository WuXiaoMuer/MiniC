# MiniC 示例程序

本目录包含多个示例程序，展示MiniC编译器的各项功能。

## 示例列表

### 1. hello.c - 最简单的程序
```c
int main() {
    int n;
    n = 42;
    return n;
}
```

**功能**: 演示最基本的变量声明、赋值和返回

**预期输出**: 程序退出码为42

**编译和运行**:
```bash
./miniic demo/hello.c > hello.s
nasm -f elf32 hello.s -o hello.o
gcc -m32 hello.o -o hello
./hello
echo $?  # 显示: 42
```

---

### 2. factorial.c - 递归函数
```c
int factorial(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

int main() {
    int result;
    result = factorial(5);
    return result;
}
```

**功能**: 演示递归函数调用、条件判断

**预期输出**: 程序退出码为120 (5! = 5×4×3×2×1 = 120)

**编译和运行**:
```bash
./miniic demo/factorial.c > factorial.s
nasm -f elf32 factorial.s -o factorial.o
gcc -m32 factorial.o -o factorial
./factorial
echo $?  # 显示: 120
```

**递归过程**:
```
factorial(5)
→ 5 * factorial(4)
→ 5 * (4 * factorial(3))
→ 5 * (4 * (3 * factorial(2)))
→ 5 * (4 * (3 * (2 * factorial(1))))
→ 5 * (4 * (3 * (2 * 1)))
→ 5 * 4 * 3 * 2 * 1
= 120
```

---

### 3. fibonacci.c - 循环语句
```c
int main() {
    int a;
    int b;
    int temp;
    int i;

    a = 0;
    b = 1;

    for (i = 0; i < 10; i = i + 1) {
        temp = a + b;
        a = b;
        b = temp;
    }

    return a;
}
```

**功能**: 演示for循环、多变量操作

**预期输出**: 程序退出码为55（斐波那契数列的第10项）

**斐波那契数列**: 0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55

**循环过程**:
```
i=0: a=0, b=1, temp=1 → a=1, b=1  (第2项)
i=1: a=1, b=1, temp=2 → a=1, b=2  (第3项)
i=2: a=1, b=2, temp=3 → a=2, b=3  (第4项)
...
i=9: a=34, b=55 → a=55, b=89 (第11项)
```

**编译和运行**:
```bash
./miniic demo/fibonacci.c > fibonacci.s
nasm -f elf32 fibonacci.s -o fibonacci.o
gcc -m32 fibonacci.o -o fibonacci
./fibonacci
echo $?  # 显示: 55
```

---

### 4. prime.c - 素数判断
```c
int isPrime(int n) {
    int i;
    if (n <= 1) {
        return 0;
    }
    for (i = 2; i < n; i = i + 1) {
        if (n / i * i == n) {
            return 0;
        }
    }
    return 1;
}

int main() {
    int result;
    result = isPrime(17);
    return result;
}
```

**功能**: 演示嵌套函数调用、逻辑判断

**预期输出**: 程序退出码为1（表示17是素数）

**素数判断原理**:
- 从2到n-1逐个检查
- 如果n能被某个数整除，则不是素数
- 否则就是素数

**编译和运行**:
```bash
./miniic demo/prime.c > prime.s
nasm -f elf32 prime.s -o prime.o
gcc -m32 prime.o -o prime
./prime
echo $?  # 显示: 1 (是素数)
```

**测试不同数值**:
修改 `isPrime(17)` 为其他值测试：
- `isPrime(2)` → 1 (素数)
- `isPrime(7)` → 1 (素数)
- `isPrime(10)` → 0 (不是素数)
- `isPrime(23)` → 1 (素数)

---

## 自创建示例

### 示例5: 基本算术运算
```c
int main() {
    int a;
    int b;
    int result;

    a = 10;
    b = 3;

    result = a + b;       // 13
    result = a - b;       // 7
    result = a * b;       // 30
    result = a / b;       // 3
    result = a % b;       // 1

    return result;
}
```

### 示例6: 条件表达式
```c
int max(int x, int y) {
    if (x > y) {
        return x;
    } else {
        return y;
    }
}

int main() {
    int m;
    m = max(10, 20);
    return m;  // 20
}
```

### 示例7: 嵌套循环
```c
int main() {
    int sum;
    int i;
    int j;

    sum = 0;
    for (i = 1; i < 5; i = i + 1) {
        for (j = 1; j < 3; j = j + 1) {
            sum = sum + i * j;
        }
    }

    return sum;
}
```

### 示例8: 逻辑运算
```c
int main() {
    int a;
    int b;
    int result;

    a = 1;
    b = 0;

    // 逻辑与
    if (a && b) {
        result = 1;
    } else {
        result = 0;
    }

    // 逻辑或
    if (a || b) {
        result = 1;
    }

    // 逻辑非
    if (!b) {
        result = 1;
    }

    return result;
}
```

### 示例9: 比较运算
```c
int main() {
    int a;
    int b;
    int result;

    a = 5;
    b = 10;

    if (a == b) { result = 0; }
    if (a != b) { result = 1; }
    if (a < b)  { result = 1; }
    if (a > b)  { result = 0; }
    if (a <= b) { result = 1; }
    if (a >= b) { result = 0; }

    return result;
}
```

### 示例10: while循环
```c
int main() {
    int count;
    int sum;

    count = 0;
    sum = 0;

    while (count < 10) {
        sum = sum + count;
        count = count + 1;
    }

    return sum;  // 0+1+2+3+4+5+6+7+8+9 = 45
}
```

---

## 调试技巧

### 查看生成的汇编代码
```bash
./miniic demo/hello.c | less
```

### 只进行词法分析（修改main.c）
```bash
# 编译后运行，查看token输出
./miniic demo/hello.c 2>&1 | head -20
```

### 查看AST结构
生成的AST会打印到标准输出：
```bash
./miniic demo/factorial.c
```

### 使用gdb调试可执行程序
```bash
gdb ./hello
(gdb) break main
(gdb) run
(gdb) step
(gdb) print $eax
```

---

## 常见问题

### Q: 为什么不能使用printf？
A: 当前MiniC不支持标准库函数。所有输出通过程序的返回值（exit code）来展示。

### Q: 如何看到更详细的中间结果？
A: 编译器的输出已经包含词法分析结果、AST结构等信息。可以重定向到文件查看：
```bash
./miniic demo/factorial.c > output.txt 2>&1
```

### Q: 程序运行后没有输出怎么办？
A: MiniC程序通过返回值输出结果。使用 `echo $?` 查看返回码。

### Q: 如何添加自己的示例？
A: 创建新的.c文件，确保语法符合MiniC支持的语言特征，然后像其他示例一样编译运行。

---

## 下一步

- 尝试修改现有示例，观察结果变化
- 创建自己的程序
- 研究`ARCHITECTURE.md`了解编译器内部原理
- 查看`TODO.md`了解可以改进的方向
