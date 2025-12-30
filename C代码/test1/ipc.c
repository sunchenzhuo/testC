/*
 * @Author: shuxiansheng sunchenzhuo5570@163.com
 * @Date: 2025-12-19 11:14:16
 * @LastEditors: shuxiansheng sunchenzhuo5570@163.com
 * @LastEditTime: 2025-12-25 11:13:51
 * @FilePath: /代码/C代码/test1/ipc.c
 * @Description:
 */
#include "ipc.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include <semaphore.h>
#include <error.h>
#include <string.h>
#include <signal.h>

/* ========= 文件内私有变量（全局 IPC 资源） ========= */
static int shm_id = -1;//共享内存的编号
static int sem_id = -1;//信号量的编号
static char *shm_addr = NULL;//进程映射到的共享内存地址

/* System V 信号量需要的 union */
union semun
{
    int val;
};

/* ========= 内部函数：信号量操作,申请资源 ========= */
static void sem_P(int sem_index)
{
    struct sembuf sbuf;
    sbuf.sem_num = sem_index;
    sbuf.sem_op = -1; // P操作
    sbuf.sem_flg = 0;

    if (semop(sem_id, &sbuf, 1) == -1)
    {
        perror("semop P :");
        exit(EXIT_FAILURE);
    }
}
/* ========= 内部函数：信号量操作,释放资源========= */
static void sem_V(int sem_index)
{
    struct sembuf sbuf;
    sbuf.sem_num = sem_index;
    sbuf.sem_op = 1; // V操作
    sbuf.sem_flg = 0;

    if (semop(sem_id, &sbuf, 1) == -1)
    {
        perror("semop V :");
        exit(EXIT_FAILURE);
    }
}

/* ========= 对外接口实现 ========= */
int ipc_init(key_t key)
{
    /* 1. 创建信号量集（2 个） */
    sem_id = semget(key, 2, IPC_CREAT | 0666);
    if (sem_id < 0)
    {
        perror("semget:");
        exit(-1);
    }

    /* 2. 创建共享内存 ，在系统中申请一块 所有进程都能访问的内存区域*/
    shm_id = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id < 0)
    {
        perror("shmget:");
        exit(-1);
    }
    /* 3. 映射共享内存 */
    shm_addr = (char *)shmat(shm_id, NULL, 0);//把“内核里的共享内存”映射成一个普通 char*
    if (shm_addr == (void *)-1)
    {
        perror("shmat:");
        shm_addr = NULL;
        return -1;
    }

    /* 4. 初始化信号量 */
    union semun arg;
    arg.val = 0; // 初始不能读
    if (semctl(sem_id, 0, SETVAL, arg) == -1)
    {
        perror("semctl SEM_READ");
        return -1;
    }
    arg.val = 1; // 初始能写
    if (semctl(sem_id, 1, SETVAL, arg) == -1)
    {
        perror("semctl SEM_WRITE");
        return -1;
    }

    /*清空共享内存*/
    memset(shm_addr, 0, SHM_SIZE);

    return 0;
}

void ipc_cleanup(void)
{
     /* 1. 解除共享内存映射 */
     if(shm_addr!=NULL)
     {
        shmdt(shm_addr);
        shm_addr=NULL;
     }
     /* 2. 删除共享内存 */
     if (shm_id!=-1)
     {
        shmctl(shm_id,IPC_RMID,NULL);
        shm_id=-1;
     }
     /* 3. 删除信号量 */
     if(sem_id!=-1)
     {
        semctl(sem_id,0,IPC_RMID);
        sem_id=-1;
     }
}
/**
 * @description: 进行信号量P操作
 * @param {int} sem_index
 * @return {*}
 */
void P(int sem_index){
    sem_P(sem_index);
}
/**
 * @description: 进行信号量V操作
 * @param {int} sem_index
 * @return {*}
 */
void V(int sem_index){
    sem_V(sem_index);
}
/**
 * @description: 返回共享内存地址
 * @return {*}
 */
char *ipc_get_shmaddr(void)
{
    return shm_addr;
}