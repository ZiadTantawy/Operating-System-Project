#define _GNU_SOURCE 
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <sys/resource.h>
#include <sys/time.h>

typedef struct {
    clock_t release_time;
    clock_t start_time;
    clock_t finish_time;
    long double waiting_time;
    long double response_time;
    long double turnaround_time;
    long double cpu_utilization;
    long double execution_time;
    struct rusage usage;
} metrics_t;

metrics_t metrics1, metrics2, metrics3;
struct rusage system_usage_start, system_usage_end;

void calculate_metrics(metrics_t *metrics) {
    metrics->execution_time = ((long double)(metrics->finish_time - metrics->start_time)) / CLOCKS_PER_SEC;
    metrics->waiting_time = ((long double)(metrics->start_time - metrics->release_time)) / CLOCKS_PER_SEC;
    metrics->response_time = metrics->waiting_time;
    metrics->turnaround_time = ((long double)(metrics->finish_time - metrics->release_time)) / CLOCKS_PER_SEC;

    struct timeval utime = metrics->usage.ru_utime;
    struct timeval stime = metrics->usage.ru_stime;
    
    long double total_cpu_time = (utime.tv_sec + stime.tv_sec) + (utime.tv_usec + stime.tv_usec) / 1e6;
    long double total_elapsed_time = ((long double)(metrics->finish_time - metrics->release_time)) / CLOCKS_PER_SEC;
    metrics->cpu_utilization = (total_elapsed_time > 0) ? (total_cpu_time / total_elapsed_time) : 0.0;
}

void print_metrics(metrics_t *metrics, const char* thread_name, const char* policy_name) {
    printf("\n%s with %s Scheduling:\n", thread_name, policy_name);
    printf("Execution Time: %Lf seconds\n", metrics->execution_time);
    printf("Release Time: %ld\n", metrics->release_time);
    printf("Start Time: %ld\n", metrics->start_time);
    printf("Finish Time: %ld\n", metrics->finish_time);
    printf("Waiting Time: %Lf seconds\n", metrics->waiting_time);
    printf("Response Time: %Lf seconds\n", metrics->response_time);
    printf("Turnaround Time: %Lf seconds\n", metrics->turnaround_time);
    printf("CPU Utilization: %Lf\n", metrics->cpu_utilization);
    printf("Memory Consumption: %ld KB\n", metrics->usage.ru_maxrss);
}

void *thread1(void *args) {
    metrics1.start_time = clock();
    char c1, c2;
    printf("Enter two alphabetical characters: ");
    fflush(stdout); 
    scanf(" %c %c", &c1, &c2);
    if (c1 > c2) {
        char temp = c1;
        c1 = c2;
        c2 = temp;
    }
    for (char c = c1; c <= c2; c++) {
        printf("%c ", c);
    }
    printf("\n");
    metrics1.finish_time = clock();
    getrusage(RUSAGE_THREAD, &metrics1.usage);
    calculate_metrics(&metrics1);
    pthread_exit(NULL);
}

void *thread2(void* arg) {
    metrics2.start_time = clock();
    pthread_t thread_id = pthread_self();
    printf("Thread 2 ID: %lu |  Thread is starting.\n", thread_id);
    for(long i=0;i<1e9;i++){}
    printf("Thread 2 ID: %lu | : Thread is running.\n", thread_id);
    for(long i=0;i<1e9;i++){}
    printf("Thread 2 ID: %lu | : Thread is ending.\n", thread_id);
    metrics2.finish_time = clock();
    getrusage(RUSAGE_THREAD, &metrics2.usage);
    calculate_metrics(&metrics2);
    return NULL;
}

void *thread3(void *args) {
    metrics3.start_time = clock();
    int x, y, sum = 0, prod = 1, count;
    float avg;
    printf("Insert the first number: ");
    scanf("%d", &x);
    printf("Insert the second number: ");
    scanf("%d", &y);

    count = (x < y) ? (y - x) + 1 : (x - y) + 1;
    for (int i = (x < y ? x : y); i <= (x > y ? x : y); i++) {
        sum += i;
        prod *= i;
    }
    avg = (float)sum / count;
    printf("Thread 3: Sum = %d, Average = %.2f, Product = %d\n", sum, avg, prod);
    
    metrics3.finish_time = clock();
    getrusage(RUSAGE_THREAD, &metrics3.usage);
    calculate_metrics(&metrics3);
    pthread_exit(NULL);
}

void print_system_cpu_load() {
    struct timeval utime_start = system_usage_start.ru_utime;
    struct timeval stime_start = system_usage_start.ru_stime;
    struct timeval utime_end = system_usage_end.ru_utime;
    struct timeval stime_end = system_usage_end.ru_stime;

    long double total_cpu_time_start = (utime_start.tv_sec + stime_start.tv_sec) + (utime_start.tv_usec + stime_start.tv_usec) / 1e6;
    long double total_cpu_time_end = (utime_end.tv_sec + stime_end.tv_sec) + (utime_end.tv_usec + stime_end.tv_usec) / 1e6;

    long double total_cpu_time = total_cpu_time_end - total_cpu_time_start;

    printf("\nSystem CPU Load: %Lf seconds\n", total_cpu_time);
}

int main(){
    pthread_t ptid1, ptid2, ptid3;
    pthread_attr_t attr1, attr2, attr3;
    struct sched_param param;
    cpu_set_t cpuset;

    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);

    pthread_attr_init(&attr1);
    pthread_attr_init(&attr2);
    pthread_attr_init(&attr3);
    
    pthread_attr_setinheritsched(&attr1, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setinheritsched(&attr2, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setinheritsched(&attr3, PTHREAD_EXPLICIT_SCHED);

    // Get system usage before starting threads
    getrusage(RUSAGE_SELF, &system_usage_start);

    // Thread 1
    metrics1.release_time = clock();
    pthread_attr_setschedpolicy(&attr1, SCHED_RR);
    param.sched_priority = 20;
    pthread_attr_setschedparam(&attr1, &param);
    pthread_create(&ptid1, &attr1, thread1, NULL);

    // Thread 2
    metrics2.release_time = clock();
    pthread_attr_setschedpolicy(&attr2, SCHED_RR);
    param.sched_priority = 30;
    pthread_attr_setschedparam(&attr2, &param);
    pthread_create(&ptid2, &attr2, thread2, NULL);

    // Thread 3
    metrics3.release_time = clock();
    pthread_attr_setschedpolicy(&attr3, SCHED_RR);
    param.sched_priority = 10;
    pthread_attr_setschedparam(&attr3, &param);
    pthread_create(&ptid3, &attr3, thread3, NULL);

    // Join threads
    pthread_join(ptid1, NULL);
    print_metrics(&metrics1, "Thread 1", "Round Robin");

    pthread_join(ptid2, NULL);
    print_metrics(&metrics2, "Thread 2", "Round Robin");

    pthread_join(ptid3, NULL);
    print_metrics(&metrics3, "Thread 3", "Round Robin");

    // Get system usage after threads have finished
    getrusage(RUSAGE_SELF, &system_usage_end);
    print_system_cpu_load();

    pthread_attr_destroy(&attr1);
    pthread_attr_destroy(&attr2);
    pthread_attr_destroy(&attr3);

    return 0;
}