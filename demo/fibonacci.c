// 斐波那契数列
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
