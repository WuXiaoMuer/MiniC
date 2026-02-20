# 调试说明 - GitHub Actions

## 问题分析

### GitHub Actions 中的重定向问题

当使用 `> hello.s` 时，所有输出（包括调试信息）都会被重定向到文件。

### 解决方案

使用 `awk` 提取汇编部分：

```bash
./minic demo/hello.c 2>&1 | tee compiler-output.txt
awk '/; MiniC Generated Assembly/,0' compiler-output.txt > hello.s
```

这样：
1. `2>&1` 合并 stderr 和 stdout
2. `tee` 保存完整输出到 `compiler-output.txt`
3. `awk` 从标记处提取到文件末尾

### 为什么之前失败

之前的代码：
```bash
./minic demo/hello.c > hello.s
```

这导致：
- 调试输出进入 hello.s
- NASM 无法解析调试信息
- 汇编失败

## 当前的解决方法

新的 workflow：
1. 运行编译器，保存完整输出
2. 用 awk 提取汇编部分
3. 独立的汇编和链接步骤
4. 运行并检查结果

这样即使有调试输出，也能正确提取汇编代码。
