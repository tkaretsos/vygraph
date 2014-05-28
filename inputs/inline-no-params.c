#include <stdio.h>

void foo() {
    int a = 5, b = 2;
    int c = 3;
    a = b + c;
    b = c + a;
    c = a + b + c;
}

int main() {
    foo();
    printf("hello");
    return 0;
}
