/*
 * @Author: shuxiansheng sunchenzhuo5570@163.com
 * @Date: 2025-12-12 09:24:51
 * @LastEditors: shuxiansheng sunchenzhuo5570@163.com
 * @LastEditTime: 2025-12-12 10:45:47
 * @FilePath: /代码/C代码/有名管道/readtest.c
 * @Description: 读管道
 */
#include<stdio.h>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>
#include<string.h>

int main(int argc, char const *argv[])
{
    int re,fd;
    char buf[20];
    fd=open("myfifo",O_RDONLY);
    if (re==-1)
    {
        perror("myfifo");
        return -1;
    }
    while (1)
    {
        read(fd,buf,20);
        printf("read=%s\n",buf);
    }
    
    
    return 0;
}
