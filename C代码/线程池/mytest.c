/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-12-10 10:56:13
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-12-10 17:14:26
 * @FilePath: /代码/C代码/线程池/mytest.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define TID_NUM 10

typedef int data_t; // 输出类型方便更改
typedef struct Task // 任务定义
{
    void *(*fun)(void *arg); // 任务实现函数
    void *arg;               // 函数传递进的参数
    struct Task *next;
} task;

typedef struct Poll // 线程池定义
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_t tid[TID_NUM];
    task *head;   // 任务队列头结点
    int tid_busy; // 线程池中正在工作的线程数
} pthread_pool;

pthread_pool *pool = NULL;

/**
 * @description: 任务实现
 * @param {void} *arg -传入的参数
 * @return {*}
 */
void *fun(void *arg)
{
    printf("task use %d\n", (data_t)(intptr_t)arg);
}

/**
 * @description: 线程工作函数
 * @param {void} *arg -传入的参数
 * @return {*}
 */
void *pool_thread(void *arg)
{
    while (1)
    {
        task *ta;
        pthread_mutex_lock(&pool->mutex);
        pthread_cond_wait(&pool->cond, &pool->mutex);

        ta = pool->head;               // 从任务队列头部取出一个任务
        pool->head = pool->head->next; // 调整队列头指针指向下一个任务

        pthread_mutex_unlock(&pool->mutex);

        ta->fun(ta->arg); // 调用任务的函数指针，传入任务的参数
        free(ta);
        pool->tid_busy--;
    }
}

/**
 * @description: 任务制作，制作一个任务就通知线程池
 * @param {data_t} arg -传递进的参数
 * @return {-1-函数失败，0-函数成功}
 */
int Add_task(data_t arg)
{
    if (pool->tid_busy >= TID_NUM)
    {
        usleep(10000);
    }

    task *ta, *p;
    ta = (task *)malloc(sizeof(task));
    if (ta == NULL)
    {
        printf("task add error!\n");
        return -1;
    }
    // 初始化任务结构
    ta->fun = fun;
    ta->arg = (void *)(intptr_t)arg;
    p = pool->head;

    pthread_mutex_lock(&pool->mutex);
    // 空队列处理
    if (p == NULL)
    {
        pool->head = ta;
    }
    // 非空队列处理(添加到队尾)
    else
    {
        while (p->next)
        {
            p = p->next; // 遍历到链表末尾
        }
        p->next = ta; // 将新节点添加到末尾
    }
    // 更新忙碌计数和信号通知
    pool->tid_busy++;
    pthread_cond_signal(&pool->cond);
    pthread_mutex_unlock(&pool->mutex);
    return 0;
}

/**
 * @description: 线程池初始化
 * @param {*}
 * @return {-1-函数失败，0-函数成功}
 */
int pool_init()
{
    int i;
    pool = (pthread_pool *)malloc(sizeof(pthread_pool));
    if (pool == NULL)
    {
        printf("pthread pool creat error!\n");
        return -1;
    }
    pthread_cond_init(&pool->cond, NULL);
    pthread_mutex_init(&pool->mutex, NULL);
    for (int i = 0; i < TID_NUM; i++)
    {
        pthread_create(&pool->tid[i], NULL, pool_thread, NULL);
    }
    pool->head = NULL;
    pool->tid_busy = 0;
    return 0;
}

/**
 * @description: 线程池销毁
 * @param {pthread_pool} *pool
 * @return {-1-函数失败，0-函数成功}
 */
int pool_destroy(pthread_pool *pool)
{
    task *ta;
    if (pool == NULL)
    {
        printf("pthread pool is already destory!\n");
        return -1;
    }
    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->cond);

    while (pool->head)
    {
        ta = pool->head;
        pool->head = pool->head->next;
        free(ta);
    }
    pool->tid_busy = 0;
    free(pool);
    pool = NULL;

    return 0;
}
int main(int argc, char const *argv[])
{
    data_t i;
    pool_init();
    sleep(1);
    for (int i = 0; i < 20; i++)
    {
        Add_task(i);
    }
    sleep(1);
    pool_destroy(pool);
    return 0;
}
