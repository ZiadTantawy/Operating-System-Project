#define _GNU_SOURCE 
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>


clock_t start1, start2, start3, start4;
clock_t end1, end2, end3, end4;
double RoundTripTime1, RoundTripTime2, RoundTripTime3, RoundTripTime4;


void *thread1(void *args){
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

void *thread2(void* arg) {
    pthread_t thread_id = pthread_self();
    printf("Thread 2 ID: %lu | : Thread is starting.\n", thread_id);
    printf("Thread 2 ID: %lu | : Thread is running.\n", thread_id);
    printf("Thread 2 ID: %lu | : Thread is ending.\n", thread_id);
    return NULL;
}

void *thread3(void *args){
    int x,y,sum,count,prod;
    float avg;
    printf("insert the first number \n");
    scanf("%d",&x);
    printf("insert the second number \n");
    scanf("%d",&y);
    x=1;
    y=5;
    sum=0;
    prod=1;
  if(x==y){
    sum=x;
    prod=x;
    avg=x;
}
  else if(x<y){
    count=(y-x)+1;
    for(int i=x;i<=y; i++){
        sum+=i;
        prod*=i;
    }
  }
  else{
    count=(x-y)+1;
    for(int i=y;i<=x; i++){
        sum+=i;
        prod*=i;
    }
  }
  avg=(float)sum/count;
  printf("Thread 3 \n Sum = %d, Average = %.2f, Product = %d\n", sum, avg, prod);
}

int main(){
    pthread_t ptid1;
    pthread_t ptid2;
    pthread_t ptid3;

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);   
    CPU_SET(0, &cpuset);

    pthread_attr_t attr;
    struct sched_param param1, param2, param3, param4;

    sched_setaffinity(ptid1, sizeof(cpuset), &cpuset);
    sched_setaffinity(ptid2, sizeof(cpuset), &cpuset);
    sched_setaffinity(ptid3, sizeof(cpuset), &cpuset);

    param1.sched_priority = 30;
    param2.sched_priority = 30;
    param3.sched_priority = 30;
    param4.sched_priority = 30;

    pthread_attr_init(&attr);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO); 
    // pthread_attr_setschedpolicy(&attr, SCHED_RR);     
    // pthread_attr_setschedpolicy(&attr, SCHED_OTHER); 



    
    pthread_create(&ptid3, NULL, &thread3, NULL); 
}