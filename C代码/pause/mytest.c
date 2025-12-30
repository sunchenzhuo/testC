/*
 * @Author: shuxiansheng sunchenzhuo5570@163.com
 * @Date: 2025-12-17 10:59:08
 * @LastEditors: shuxiansheng sunchenzhuo5570@163.com
 * @LastEditTime: 2025-12-17 11:08:49
 * @FilePath: /代码/C代码/pause/mytest.c
 * @Description: pause/sigsuspend函数
 */
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>

void handle(int sig){
    printf("I get sig=%d\n",sig);
}

void mytask(){
    printf("My task start\n");
    sleep(3);
    printf("My task end\n");
}

int main(){
    struct sigaction act;
    act.sa_handler=handle;
    act.sa_flags=0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT,&act,NULL);

    sigset_t set,set2;
    sigemptyset(&set2);
    sigaddset(&set,SIGHUP);
    sigaddset(&set2,SIGINT);
    
    pause();
    while (1)
    {
        sigprocmask(SIG_BLOCK,&set,NULL); //屏蔽SIGHUP信号
        mytask();//得到信号执行任务
        //      sigprocmask(SIG_UNBLOCK,&set,NULL);
        //      pause();
        sigsuspend(&set2); //临时屏蔽SIGINT信号，等待其他信号到来       
    }

    printf("After pause\n");
    while (1)
    {
        sleep(1);
    }
    
    
}