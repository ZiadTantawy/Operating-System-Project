#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
void *func1(void *args){
    char c1, c2;
    printf("Enter two alphabetical characters: ");
    scanf("%c %c", &c1, &c2);
    if(c1>c2){
        char temp = c1;
        c1=c2;
        c2=temp;
    }
    for(char c = c1;c<=c2;c++){
        printf("%c ", c);
    }
    pthread_exit(NULL);
}

int main(){
    pthread_t thread1;
    pthread_create(&thread1, NULL, func1, NULL);
    pthread_join(thread1, NULL);
    return 0;
}