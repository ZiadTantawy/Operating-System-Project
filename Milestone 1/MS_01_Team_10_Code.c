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

void *thread1(void *args) {
    char c1, c2;
    printf("Enter two alphabetical characters: ");
    fflush(stdout); // Ensure the prompt is displayed immediately
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
    pthread_exit(NULL);
}

void *thread2(void* arg) {
    pthread_t thread_id = pthread_self();
    printf("Thread 2 ID: %lu | : Thread is starting.\n", thread_id);
    printf("Thread 2 ID: %lu | : Thread is running.\n", thread_id);
    printf("Thread 2 ID: %lu | : Thread is ending.\n", thread_id);
    return NULL;
}

void *thread3(void *args) {
    int x, y, sum, count, prod;
    float avg;
    printf("Insert the first number: \n");
    scanf("%d", &x);
    printf("Insert the second number: \n");
    scanf("%d", &y);

    sum = 0;
    prod = 1;
    if (x == y) {
        sum = x;
        prod = x;
        avg = x;
    } else if (x < y) {
        count = (y - x) + 1;
        for (int i = x; i <= y; i++) {
            sum += i;
            prod *= i;
        }
    } else {
        count = (x - y) + 1;
        for (int i = y; i <= x; i++) {
            sum += i;
            prod *= i;
        }
    }
    avg = (float)sum / count;
    printf("Thread 3\nSum = %d, Average = %.2f, Product = %d\n", sum, avg, prod);
    pthread_exit(NULL);
}

void set_thread_scheduling(pthread_t thread, int policy, int priority) {
    struct sched_param param;
    param.sched_priority = priority;
    pthread_setschedparam(thread, policy, &param);
}

void print_metrics(metrics_t metrics, const char* thread_name, const char* policy_name) {
    metrics.execution_time = ((long double) (metrics.finish_time - metrics.start_time)) / CLOCKS_PER_SEC;
    metrics.waiting_time = ((long double) (metrics.start_time - metrics.release_time)) / CLOCKS_PER_SEC;
    metrics.response_time = metrics.waiting_time;
    metrics.turnaround_time = ((long double) (metrics.finish_time - metrics.release_time)) / CLOCKS_PER_SEC;
    if (metrics.execution_time > 0) {
        metrics.cpu_utilization = ((long double) (metrics.usage.ru_utime.tv_sec + metrics.usage.ru_stime.tv_sec)) / metrics.execution_time;
    } else {
        metrics.cpu_utilization = 0.0;
    }

    printf("%s with %s Scheduling:\n", thread_name, policy_name);
    printf("Execution Time: %Lf seconds\n", metrics.execution_time);
    printf("Release Time: %ld\n", metrics.release_time);
    printf("Start Time: %ld\n", metrics.start_time);
    printf("Finish Time: %ld\n", metrics.finish_time);
    printf("Waiting Time: %Lf seconds\n", metrics.waiting_time);
    printf("Response Time: %Lf seconds\n", metrics.response_time);
    printf("Turnaround Time: %Lf seconds\n", metrics.turnaround_time);
    printf("CPU Utilization: %Lf\n", metrics.cpu_utilization);
    printf("Memory Consumption: %ld KB\n", metrics.usage.ru_maxrss);
}

void run_thread_with_policy(pthread_t *thread, pthread_attr_t *attr, void *(*thread_func)(void *), int policy, int priority, const char* thread_name, const char* policy_name) {
    struct sched_param param;
    cpu_set_t cpuset;
    metrics_t metrics;

    // Set scheduling policy and priority
    pthread_attr_setschedpolicy(attr, policy);
    param.sched_priority = priority;
    pthread_attr_setschedparam(attr, &param);

    // Set CPU affinity (BEFORE thread starts)
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset); // Assigning the thread to CPU core 0

    metrics.release_time = clock();
    
    pthread_create(thread, attr, thread_func, NULL);

    // Apply CPU affinity to the created thread immediately
    pthread_setaffinity_np(*thread, sizeof(cpu_set_t), &cpuset);
    cpu_set_t check_set;
    CPU_ZERO(&check_set);
    pthread_getaffinity_np(*thread, sizeof(cpu_set_t), &check_set);
    if (!CPU_ISSET(0, &check_set)) {
        printf("CPU affinity was not set correctly!\n");
    }

    metrics.start_time = clock();
    pthread_join(*thread, NULL);
    metrics.finish_time = clock();
    getrusage(RUSAGE_THREAD, &metrics.usage);
    print_metrics(metrics, thread_name, policy_name);
}



int main(){
    pthread_t ptid1, ptid2, ptid3;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

    // Run each thread with FIFO, Round Robin, and SJF scheduling policies
    // run_thread_with_policy(&ptid1, &attr, thread1, SCHED_FIFO, 90, "Thread 1", "FIFO");
    run_thread_with_policy(&ptid1, &attr, thread1, SCHED_RR, 90, "Thread 1", "Round Robin");
    // run_thread_with_policy(&ptid1, &attr, thread1, SCHED_OTHER, 0, "Thread 1", "SJF");

    // run_thread_with_policy(&ptid2, &attr, thread2, SCHED_FIFO,50, "Thread 2", "FIFO");
    run_thread_with_policy(&ptid2, &attr, thread2, SCHED_RR, 50, "Thread 2", "Round Robin");
    // run_thread_with_policy(&ptid2, &attr, thread2, SCHED_OTHER, 0, "Thread 2", "SJF");

    // run_thread_with_policy(&ptid3, &attr, thread3, SCHED_FIFO, 10, "Thread 3", "FIFO");
    run_thread_with_policy(&ptid3, &attr, thread3, SCHED_RR, 10, "Thread 3", "Round Robin");
    // run_thread_with_policy(&ptid3, &attr, thread3, SCHED_OTHER, 0, "Thread 3", "SJF");

    pthread_attr_destroy(&attr);

    return 0;
}