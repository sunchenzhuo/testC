/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-12-10 08:27:09
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-12-10 10:31:59
 * @FilePath: /代码/C代码/条件变量/mytest.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct resource
{
    struct resource *next;
    int num;
} resour, *res;

int i = 1;
res head = NULL;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *producer(void *arg)
{
    res tx;
    pthread_detach(pthread_self());
    printf("producer create\n");
    while (1)
    {
        tx = (res)malloc(sizeof(resour));
        tx->num = i++;

        printf("produce %d\n", tx->num);

        pthread_mutex_lock(&mutex);

        tx->next = head;
        head = tx;

        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
}

void *consumer(void *arg)
{
    res tx;
    pthread_detach(pthread_self());

    printf("consumer create \n");

    while (1)
    {
        pthread_mutex_lock(&mutex);
        while (head == NULL)
        {
            pthread_cond_wait(&cond, &mutex);
        }

        tx = head;
        head = head->next;
        printf("consumer %d\n", tx->num);
        free(tx);

        pthread_mutex_unlock(&mutex);
    }
}
int main(int argc, char const *argv[])
{
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, producer, NULL);
    sleep(5);
    pthread_create(&tid2, NULL, consumer, NULL);

    while (1)
    {
        sleep(1);
    }

    return 0;
}
