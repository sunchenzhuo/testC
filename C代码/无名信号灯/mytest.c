/*
 * @Author: shuxiansheng sunchenzhuo5570@163.com
 * @Date: 2025-12-18 16:44:10
 * @LastEditors: shuxiansheng sunchenzhuo5570@163.com
 * @LastEditTime: 2025-12-18 17:09:55
 * @FilePath: /代码/C代码/无名信号灯/mytest.c
 * @Description: 无名信号灯实现进程间通信
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <pthread.h>

sem_t sem_r,sem_w;
void *shmaddr;

void handle(int arg)
{
    sem_destroy(&sem_r);
    sem_destroy(&sem_w);
    exit(-1);
}

void *thread1(void *arg)
{
    while(1)
    {
        sem_wait(&sem_r);
        printf("%s\n",(char *)shmaddr);
        sem_post(&sem_w);
    }
    exit(0);
}

int main(int argc, char const *argv[])
{
    key_t key;
    int shmid;

    struct sigaction act;
    act.sa_handler=handle;
    act.sa_flags=0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT,&act,NULL);
    key=ftok(".",100);

    if (key<0)
    {
        perror("ftok:");
        exit(-1);
    }

    shmid=shmget(key,256,IPC_CREAT|0666);
    if(shmid<0)
    {
        perror("shmget:");
        exit(-1);
    }

    shmaddr=shmat(shmid,NULL,0);
    if (shmaddr==(void *)-1)
    {
        perror("shmaddr:");
        exit(-1);
    }
    
    sem_init(&sem_r,0,0);
    sem_init(&sem_w,0,1);

    pthread_t tid;
    pthread_create(&tid,NULL,thread1,NULL);

    while (1)
    {
        sem_wait(&sem_w);
        printf(">");
        fgets(shmaddr,5,stdin);
        sem_post(&sem_r);
    }
    return 0;
}

