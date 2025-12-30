/*
 * @Author: shuxiansheng sunchenzhuo5570@163.com
 * @Date: 2025-12-12 09:25:43
 * @LastEditors: shuxiansheng sunchenzhuo5570@163.com
 * @LastEditTime: 2025-12-12 10:45:42
 * @FilePath: /代码/C代码/有名管道/writetest.c
 * @Description: 有名管道写操作
 */
#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<string.h>

int main(int argc, char const *argv[])
{
    int re,fd;
    char buf[20];
    re=mkfifo("myfifo",0666);
    if (re==-1)
    {
        perror("myfifo:");
    }
    fd=open("myfifo",O_WRONLY);
    if (fd==-1)
    {
        perror("myfifo:");
        return -1;
    }
    while (1)
    {
        fgets(buf, 20,  stdin);// 从键盘读取一行
        write(fd,buf,strlen(buf));
    }
       

    return 0;
}
