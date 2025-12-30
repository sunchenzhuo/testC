/*
 * @Author: shuxiansheng sunchenzhuo5570@163.com
 * @Date: 2025-12-18 17:41:28
 * @LastEditors: shuxiansheng sunchenzhuo5570@163.com
 * @LastEditTime: 2025-12-19 11:09:56
 * @FilePath: /代码/C代码/SystemV信号灯/test.c
 * @Description: 信号灯P/V操作实现进程间通信
 */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <string.h>
#include <sys/types.h>
#include <sys/sem.h>

#define SEM_READ 0
#define SEM_WRITE 1

//semctl() 的第四个参数需要 union semun
union semun
{
    int val;
};

/**
 * @description: P操作（申请资源 / 减 1），相当于wait()
 * @param {int} semid
 * @param {int} semindex
 * @return {*}
 */
void Poperation(int semid,int semindex)
{
    struct sembuf sbuf;
    sbuf.sem_num=semindex;
    sbuf.sem_op=-1;
    sbuf.sem_flg=0;

    semop(semid,&sbuf,1);
}
/**
 * @description: V 操作（释放资源 / 加 1）相当于signal
 * @param {int} semid
 * @param {int} semindex
 * @return {*}
 */
void Voperation(int semid,int semindex)
{
    struct sembuf sbuf;
    sbuf.sem_num=semindex;
    sbuf.sem_op=1;
    sbuf.sem_flg=0;

    semop(semid,&sbuf,1);
}

int main(){
    key_t key;
    char *shmaddr;
    int semid,shmid;
    key=ftok(".",100);

    if (key<0)
    {
        perror("ftok error");
        exit(1);
    }
    semid=semget(key,2,IPC_CREAT|0666);//key=IPC 键
    if (semid<0)
    {
        perror("semget:");
        exit(-1);
    }
    //创建共享内存
    shmid=shmget(key,500,IPC_CREAT|0666);
    shmaddr=shmat(shmid,NULL,0);//shmaddr 指向一块 父子进程共享的内存
    
    union semun mysem;
    //初始化信号量（同步规则的核心）
    mysem.val=0;
    semctl(semid,SEM_READ,SETVAL,mysem);//不能读
    mysem.val=1;
    semctl(semid,SEM_WRITE,SETVAL,mysem);//可以写
    
    pid_t pid;
    pid=fork();//fork：一写一读
    if (pid<0)
    {
        perror("ftok");
        shmctl(shmid,IPC_RMID,NULL);
        semctl(semid,0,IPC_RMID);
        exit(-1);
    }
    else if(pid==0)
    {
        //子进程（读进程）
        while (1)
        {
            Poperation(semid,SEM_READ);//等待 可以读
            printf("%s\n",shmaddr);//读共享内存
            Voperation(semid,SEM_WRITE);//通知 可以写
        }
        
    }
    else
    {
        //父进程（写进程）
        while (1)
        {
            Poperation(semid,SEM_WRITE);//等待 可以写
            printf(">");//从键盘读
            fgets(shmaddr,32,stdin);//写入共享内存
            Voperation(semid,SEM_READ);//通知 可以读
        }
        
    }
    
}