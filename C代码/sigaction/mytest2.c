/*
 * @Author: shuxiansheng sunchenzhuo5570@163.com
 * @Date: 2025-12-16 17:51:16
 * @LastEditors: shuxiansheng sunchenzhuo5570@163.com
 * @LastEditTime: 2025-12-17 09:42:48
 * @FilePath: /代码/C代码/sigaction/mytest2.c
 * @Description: 捕获结束信号
 */

#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>

void handle(int sige)
{
    wait(NULL);
    if(sige==SIGCHLD)
        printf("I cath SIGCHLD\n");
}
int main(int argc, char const *argv[])
{
    struct sigaction act;
    act.sa_handler=handle;
    act.sa_flags=0;
    sigemptyset(&act.sa_mask);
    pid_t pid=fork();
    if (pid>0)
    {
        sigaction(SIGCHLD,&act,NULL);//设置捕获信号
        while (1)
        {
            sleep(1);
            printf("father\n");
        }
    }
    else if (pid==0)
    {
        sleep(5);
        exit(0);
    }
    return 0;
}
