#include <pthread.h>
#include <stdio.h>

void* say_hi() {
    printf("hi from thread\n");
    return NULL;
}

int main() {
    pthread_t thread;
    if (pthread_create(&thread, NULL, say_hi, NULL)) {
        printf("Error - pthread_create()\n");
        return 1;
    }
    printf("hi from main\n");
    pthread_join(thread, NULL);
    return 0;
}
