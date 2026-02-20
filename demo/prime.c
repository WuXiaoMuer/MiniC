// 素数判断
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
