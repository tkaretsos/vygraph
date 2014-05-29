#include <pthread.h>
#include <stdio.h>

void* add() {
    int a = 1, b = 2, c = 3;
    c = a + b + c;
    return NULL;
}

int main() {
    int a;
    pthread_t thread;
    if (pthread_create(&thread, NULL, add, NULL)) {
        printf("Error - pthread_create()\n");
        return 1;
    }
    add();
    a = *((int*) add());
    pthread_join(thread, NULL);
    return 0;
}
