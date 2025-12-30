/*
 * @Author: shuxiansheng sunchenzhuo5570@163.com
 * @Date: 2025-12-18 14:20:51
 * @LastEditors: shuxiansheng sunchenzhuo5570@163.com
 * @LastEditTime: 2025-12-18 16:31:10
 * @FilePath: /代码/C代码/有名信号灯/get_test.c
 * @Description: 得到信号灯
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/shm.h>
#include <sys/ipc.h>
#include <signal.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

/**
 * @description: 信号处理函数
 * @param {int} arg
 * @return {*}
 */
void handle(int arg)
{
    sem_unlink("sem_read");//删除信号灯
    exit(-1);
}

int main(int argc,const char *argv[])
{
    key_t key;
    int shmid;
    sem_t *sem_r,*sem_w;//读写信号灯的指针
    void *shmaddr;//共享内存地址指针

    struct sigaction act;
    act.sa_handler=handle;
    act.sa_flags=0;
    sigemptyset(&act.sa_mask);

    sigaction(SIGINT,&act,NULL);

    key=ftok(".",100);//生成IPC键值(基于当前目录和项目ID100)
    if(key<0)
    {
        perror("ftok");
        exit(-1);
    }

    shmid=shmget(key,256,0666|IPC_CREAT);//获取/创建256字节的共享内存段
    if(shmid<0)
    {
        perror("shmid:");
        exit(-1);
    }
    shmaddr=shmat(shmid,NULL,0);//将共享内存附加到进程地址空间
    if(shmaddr==(void *)-1)
    {
        perror("shmaddr:");
        exit(-1);
    }

    sem_w = sem_open("sem_write", O_CREAT|O_RDWR, 0666, 1);
	sem_r = sem_open("sem_read", O_CREAT|O_RDWR, 0666, 0);
    
    while(1)
    {
        sem_wait(sem_r);
        printf("%s\n",(char *)shmaddr);
        sem_post(sem_w);
    }
    return 0;
}