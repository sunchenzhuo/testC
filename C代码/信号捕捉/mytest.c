/*
 * @Author: shuxiansheng sunchenzhuo5570@163.com
 * @Date: 2025-12-16 16:00:53
 * @LastEditors: shuxiansheng sunchenzhuo5570@163.com
 * @LastEditTime: 2025-12-16 16:39:49
 * @FilePath: /代码/C代码/信号捕捉/mytest.c
 * @Description: 信号捕捉
 */
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <linux/posix_types.h>

typedef void (*sighandler_t)(int);

sighandler_t oldact;

void handle(int sig)
{
    printf("I cath the SIGINT \n");
    //    signal(SIGINT,oldact); //按信号的原方式进行，直接退出
}

int main(){
    oldact=signal(SIGINT,handle);//执行程序时，接收ctrl+c的信号
    while(1)
    {
        sleep(1);
    }
}