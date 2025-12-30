/*
 * @Author: shuxiansheng sunchenzhuo5570@163.com
 * @Date: 2025-12-16 17:08:59
 * @LastEditors: shuxiansheng sunchenzhuo5570@163.com
 * @LastEditTime: 2025-12-16 17:43:40
 * @FilePath: /代码/C代码/sigaction/mytest1.c
 * @Description: 捕获定时器的信号，执行函数（没有捕获时，定时一到，程序就会结束）
 */

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <linux/posix_types.h>
#include <stdlib.h>

void handle(int sig){
    if (sig==SIGINT)
    {
        printf("I cath the SIGINT \n");
    }
    else if (sig==SIGALRM)
    {
        printf("second timer \n");
        alarm(1);
    }
}

int main(){
    struct sigaction act;
    act.sa_handler=handle;
    act.sa_flags=0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT,&act,NULL);
    alarm(1);//设置定时1秒后发送信号
    sigaction(SIGALRM,&act,NULL);
    while (1)
    {
        sleep(1);
    }
}