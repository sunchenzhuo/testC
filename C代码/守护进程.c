/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-07-12 17:15:00
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-07-12 17:42:13
 * @FilePath: /代码/C代码/守护进程.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include<stdio.h>//FILE,fopen,fprintf,fflush,perror
#include<stdlib.h>//exit
#include<unistd.h>//fork,chair.close,sleep
#include<sys/types.h>//pid_t
#include<sys/stat.h>//umask,setsid
#include<time.h>//time,ctime1
int main()
{
    pid_t pid;//用于存储进程ID
    FILE *fp;//文件指针，用于操作日志文件
    time_t t;//时间变量，用于存储当前时间
    int i;//循环计数器
    //fork()创建子进程
    if ((pid=fork())<0 )//返回值小于0创建失败
    {
        perror("fork");
        exit(-1);//打印错误并退出
    }
    //返回值大于0：父进程执行直接退出
    else if (pid>0)
    {
        exit(0);//子进程继续执行
    }
    setsid();//使子进程成为新会话的领头进程，脱离原终端控制
    umask(0);//将文件掩码设置为0，使得后续创建的文件有最大权限
    chdir("/tmp");//将工作目录改为/tmp
    for ( i = 0; i < 3; i++)
    {
        close(i);//关闭标准输入(0)、标准输出(1)和错误输出(2),因为守护进程不需要与终端进行交互
    }
    if ((fp=fopen("time.log","a"))==NULL)//以追加模式打开日志文件，如果失败则报错退出
    {
        perror("fopen");
        exit(-1);
    }
    while (1)
    {
        time(&t);//获取当前时间戳存入变量t中
        fprintf(fp,"%s",ctime(&t));//格式化时间为字符串写入文件。fflush(fp)
        fflush(fp);//立刻刷新缓冲区确保数据写入磁盘
        sleep(1);
    }
    
}