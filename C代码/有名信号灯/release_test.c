/*
 * @Author: shuxiansheng sunchenzhuo5570@163.com
 * @Date: 2025-12-18 14:19:40
 * @LastEditors: shuxiansheng sunchenzhuo5570@163.com
 * @LastEditTime: 2025-12-18 16:13:23
 * @FilePath: /代码/C代码/有名信号灯/release_test.c
 * @Description: 释放信号灯
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

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
    sem_unlink("sem_write"); // 删除信号灯
    exit(-1);
}

int main(int argc, const char *argv[])
{
    key_t key;
    int shmid;
    sem_t *sem_r, *sem_w; // 读写信号灯的指针
    void *shmaddr;        // 共享内存地址指针

    struct sigaction act;
    act.sa_handler = handle;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);

    sigaction(SIGINT, &act, NULL);

    key = ftok(".", 100); // 生成IPC键值(基于当前目录和项目ID100)
    if (key < 0)
    {
        perror("ftok:");
        exit(-1);
    }
    shmid = shmget(key, 256, 0666 | IPC_CREAT); // 获取/创建256字节的共享内存段
    if (shmid < 0)
    {
        perror("shmid:");
        exit(-1);
    }

    shmaddr = shmat(shmid, NULL, 0); // 将共享内存附加到进程地址空间
    if (shmaddr == (void *)-1)
    {
        perror("shmaddr:");
        exit(-1);
    }

    sem_w = sem_open("sem_write", O_CREAT | O_RDWR, 0666, 1); // 初始值为1的写信号灯
    sem_r = sem_open("sem_read", O_CREAT | O_RDWR, 0666, 0);  // 初始值为0的读信号灯

    // 写入数据到共享内存
    while (1)
    {
        sem_wait(sem_w); // 等待写权限(P操作)
        printf(">");
        fgets(shmaddr, 5, stdin); // 从标准输入读取最多4个字符(+'\0')到共享内存
        sem_post(sem_r);          // 释放读权限(V操作)
    }
    return 0;
}