/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-07-12 17:50:58
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-07-12 17:59:54
 * @FilePath: /代码/C代码/守护进程tset.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>

int main()
{
    time_t t;
    pid_t pid;
    FILE *fp;
    int i;
    if ((pid=fork())<0)
    {
        perror("fork");
        exit(-1);
    }
    else if (pid>0)
    {
        exit(0);
    }
    setsid();
    umask(0);
    chdir("/代码");
    for ( i = 0; i < 3; i++)
    {
        close(i);
    }
    if ((fp=fopen("time.log","a"))==NULL)
    {
        perror("fopen");
        exit(-1);
    }
    while (1)
    {
        time(&t);
        fprintf(fp,"%s",ctime(&t));
        fflush(fp);
        sleep(1);
    }
    
}