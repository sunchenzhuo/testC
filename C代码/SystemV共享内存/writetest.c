/*
 * @Author: shuxiansheng sunchenzhuo5570@163.com
 * @Date: 2025-12-15 11:09:17
 * @LastEditors: shuxiansheng sunchenzhuo5570@163.com
 * @LastEditTime: 2025-12-15 16:16:04
 * @FilePath: /代码/C代码/System V共享内存/writetest.c
 * @Description: 共享内存写入数据
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    key_t key;
    int shmid;
    void *buf;
    key = ftok("keytest", 100);
    if (key == -1)
    {
        perror("ftok");
        exit(-1);
    }
    printf("%d\n", key);
    shmid = shmget(key, 512, IPC_CREAT | 0666);
    if (shmid==-1)
    {
        perror("shmget:");
        exit(-1);
    }
    printf("%d\n", shmid);
    buf = shmat(shmid, NULL, 0);
    if (buf == (void *)-1)
    {
        perror("shmat");
        exit(-1);
    }
    strcpy(buf, "hello world");
    while (1)
    {
        sleep(1);
    }

    return 0;
}
