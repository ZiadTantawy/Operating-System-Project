#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void* thread_function(void* arg) {
    pthread_t thread_id = pthread_self();
    printf("Thread 2 ID: %lu |  Thread is starting.\n", thread_id);
    printf("Thread 2 ID: %lu | : Thread is running.\n", thread_id);
    printf("Thread 2 ID: %lu | : Thread is ending.\n", thread_id);
    return NULL;
}

int main() {
    pthread_t thread2;
    
    pthread_create(&thread2, NULL, thread_function, NULL) != 0;
    
    pthread_join(thread2, NULL);
    
    return 0;
}