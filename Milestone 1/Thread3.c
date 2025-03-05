#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
void *func3(void *args){
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
  pthread_t ptid3;
  pthread_create(&ptid3, NULL, &func3, NULL); 
  pthread_join(ptid3, NULL);
  printf("This line will be printed after thread ends\n"); 
  pthread_exit(NULL);
  return 0;
} 