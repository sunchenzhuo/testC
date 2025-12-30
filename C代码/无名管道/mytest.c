/*
 * @Author: shuxiansheng sunchenzhuo5570@163.com
 * @Date: 2025-12-11 09:15:51
 * @LastEditors: shuxiansheng sunchenzhuo5570@163.com
 * @LastEditTime: 2025-12-11 11:08:31
 * @FilePath: /代码/C代码/管道/mytest.c
 * @Description: 
 */

// 子进程1和子进程2分别往管道中写入字符串；父进程读管道内容并打印；
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, const char *argv[])
{
    //用于存储管道的文件描述符，pipefd[0]是读端，pipefd[1]是写端,存储pipe()调用的返回值
    int pipefd[2], pi, i;
    pi = pipe(pipefd);
    pid_t pid;
    char buf[20] = {0};
    if (pi < 0)
    {
        perror("pipe:");
        return -1;
    }
    // fork循环创建子进程
    for (i = 0; i < 2; i++)
    {
        pid = fork();
        if (pid < 0)
        {
            perror("pid create:");
            return 0;
        }
        else if (pid == 0)
        {
            break;
        }
        else
        {
        }
    }
    //父进程处理（i ==2）
    if (i == 2)
    {
        close(pipefd[1]);//关闭写端
        while (1)
        {
            memset(buf, 0, 20);
            read(pipefd[0], buf, 20);
            printf("pipe read %s\n", buf);
            usleep(100);
        }
    }
    //子进程1处理(i==1)
    if (i == 1)
    {
        close(pipefd[0]);//关闭读端
        while (1)
        {
            memset(buf, 0, 20);
            strcpy(buf, "this is b");
            write(pipefd[1], buf, strlen(buf));
            usleep(300);
        }
    }
    //子进程0处理(i==0)
    if (i == 0)
    {
        close(pipefd[0]);//关闭读端 
        while (1)
        {
            memset(buf, 0, 20);
            strcpy(buf, "this is a");
            write(pipefd[1], buf, strlen(buf));
            usleep(1);
        }
    }
    return 0;
}
