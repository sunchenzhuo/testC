/*
 * @Author: shuxiansheng sunchenzhuo5570@163.com
 * @Date: 2025-12-15 11:09:17
 * @LastEditors: shuxiansheng sunchenzhuo5570@163.com
 * @LastEditTime: 2025-12-15 16:23:24
 * @FilePath: /代码/C代码/SystemV共享内存/readtest.c
 * @Description: 共享内存写操作
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char const *argv[])
{
    key_t key;
    int shmid;
    void *buf;
    if ((key=ftok("keytest",100))==-1)
    {
        perror("ftok:");
        exit(-1);
    }
    printf("%d\n",key);
    /**
     * @description: 内存映射操作
     * @return {*}
     */    
    buf=shmat(6,NULL,0);
    printf("%s\n",(char *)buf);
    /**
     * @description: 内存解除映射操作
     * @return {*}
     */    
    shmdt(buf);
    /**
     * @description: 删除共享内存
     * @return {*}
     */    
    shmctl(6,IPC_RMID,NULL);
    return 0;
}
