/*
 * @Author: shuxiansheng sunchenzhuo5570@163.com
 * @Date: 2025-12-19 16:14:10
 * @LastEditors: shuxiansheng sunchenzhuo5570@163.com
 * @LastEditTime: 2025-12-25 09:19:40
 * @FilePath: /代码/C代码/test1/main.c
 * @Description:
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/ipc.h>

#include "ipc.h"
/* ========= 信号处理 ========= */
static void handler_signal(int sig)
{
    (void)sig;
    printf("\n[INFO]exiting...\n");
    ipc_cleanup();
    exit(EXIT_SUCCESS);
}

int main(void)
{
    key_t key;
    pid_t pid;
    char *shmaddr;
    /* 1. 生成 IPC key */
    key = ftok(".", 100);
    if (key == -1)
    {
        perror("ftok:");
        exit(EXIT_FAILURE);
    }

    /* 2. 初始化 IPC */
    if (ipc_init(key) != 0)
    {
        fprintf(stderr, "ipc_init fail!\n");
        exit(EXIT_FAILURE);
    }
    /* 3. 注册 Ctrl+C 信号 */
    signal(SIGINT, handler_signal);

    /* 4. 获取共享内存地址 */
    shmaddr = ipc_get_shmaddr();
    if (shmaddr == NULL)
    {
        fprintf(stderr, "ipc_get_shmaddr fail!\n");
        ipc_cleanup();
        exit(EXIT_FAILURE);
    }

    /* 5. 创建子进程 */
    pid = fork();
    if (pid < 0)
    {
        perror("fork:");
        ipc_cleanup();
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        /* ========= 子进程：读 ========= */
        while (1)
        {
            P(SEM_READ);
            printf("child read:%s\n", shmaddr);
            fflush(stdout);
            V(SEM_WRITE);
        }
    }
    else
    {
        /* ========= 父进程：写 ========= */
        char buf[SHM_SIZE];
        while (1)
        {
            P(SEM_WRITE);
            printf("parent write >");
            fflush(stdout);
            if (fgets(buf, sizeof(buf), stdin) == NULL)
            {
                V(SEM_WRITE);
                continue;
            }
            strncpy(shmaddr, buf, SHM_SIZE - 1);
            shmaddr[SHM_SIZE - 1] = '\0';

            V(SEM_READ);
        }
    }
    return 0;
}