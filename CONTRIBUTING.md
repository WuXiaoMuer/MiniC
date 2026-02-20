# 贡献指南

感谢你考虑为 MiniC 编译器项目做出贡献！

## 如何贡献

### 报告问题

如果你发现了 bug 或有新功能的建议，请：

1. 检查是否已经有人提出了相同的问题
2. 创建一个新的 issue，提供详细的信息：
   - bug 的描述
   - 复现步骤
   - 预期行为
   - 实际行为
   - 系统环境（操作系统、编译器版本等）

### 提交代码

如果你想贡献代码：

1. Fork 这个仓库
2. 创建你的特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交你的改动 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 创建一个 Pull Request

## 代码风格

- 使用 4 个空格缩进（不要用 tab）
- 函数名使用驼峰命名法（camelCase）
- 变量名使用小写+下划线（snake_case）
- 添加适当的注释
- 保持函数简洁，一个函数只做一件事

## 开发指南

### 编译项目

```bash
gcc -o minic *.c
```

或者使用 Makefile：

```bash
make
```

### 运行测试

```bash
make run-all
```

### 添加新功能

在添加新功能之前：

1. 先在 TODO.md 中标记你要工作的任务
2. 讨论你的实现方案（可以创建 issue）
3. 编写测试用例
4. 实现功能
5. 更新文档

## 文档贡献

如果你发现文档有错误或可以改进：

- 修正错误直接提交 PR
- 如果是翻译，请在 issue 中说明
- 添加新的示例程序时，记得更新 EXAMPLES.md

## 许可

所有贡献都将使用 GPL-3.0 许可证。

## 联系方式

- GitHub Issues: https://github.com/WuXiaoMuer/MiniC/issues
- 作者主页: https://wuxiaomu.dev
- Twitter: [@WuXiaoMuer](https://twitter.com/WuXiaoMuer)

---

再次感谢你的贡献！
