/*
 * @Author: shuxiansheng sunchenzhuo5570@163.com
 * @Date: 2025-12-17 10:32:32
 * @LastEditors: shuxiansheng sunchenzhuo5570@163.com
 * @LastEditTime: 2025-12-17 10:44:10
 * @FilePath: /代码/C代码/信号集操作函数/mytest.c
 * @Description: 信号集操作函数
 */
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void handle(int sig){
    printf("I get sig=%d\n",sig);
}
int main(){
    struct sigaction act;
    act.sa_handler=handle;
    sigemptyset(&act.sa_mask);
    act.sa_flags=0;
    sigaction(SIGINT,&act,NULL);

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set,SIGINT);
    sigprocmask(SIG_BLOCK,&set,NULL);//信号屏蔽
    sleep(5);
    sigprocmask(SIG_UNBLOCK,&set,NULL); //信号不屏蔽

    while (1)
    {
        sleep(1);
    }
    
}