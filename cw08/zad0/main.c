#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *thread_fun1(void *ptr)
{
    sleep(1);
    int *val = (int *)ptr;
    printf("Started thread with arg: %d\n", *val);
    return (void *)911;
}

void *thread_fun2(void *ptr)
{
    sleep(1);
    int *val = (int *)ptr;
    printf("Started thread with arg: %d\n", *val);
    return (void *)420;
}

int main()
{
    int x = 123;
    int y = 0;

    pthread_t thread1;
    pthread_t thread2;

    pthread_create(&thread1, NULL, thread_fun1, &x);
    pthread_create(&thread2, NULL, thread_fun2, &x);
    pthread_cancel(thread2);
    pthread_join(thread1, (void **)&y);
    printf("Joined thread and ret val: %d\n", y);
    pthread_join(thread2, (void **)&y);
    printf("Joined thread and ret val: %d\n", y);
    return 0;
}