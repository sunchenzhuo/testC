/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-11-19 10:22:48
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-11-19 11:01:15
 * @FilePath: /代码/C代码/text_app.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include<stdio.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<sys/ioctl.h>

#include "mytest.h"

int main(int argc, char const *argv[])
{
    char buf[20]="hello...";
    int ret;
    int dev_max;//设备空间大小
    int dev_seek;//设备当前存储位置

    if (argc<2)
    {
        printf("Run without file!\n");
    }
    /* 打开设备文件 */
    int fd=open("/dev/mydev",O_RDWR);
    if (fd<0)
    {
        perror("open");
        return -1;
    }

    /* 读取设备属性 */
    ret=ioctl(fd,MYDEV_IOCTL_GET_MUTEX,&dev_max);//读取设备空间大小
    if (ret<0)
    {
        perror("ioctl1");
        return -1;
    }
    printf("Space size of the kernel device is %d\n",dev_max);

    ret=ioctl(fd,MYDEV_IOCTL_GET_SEEK,&dev_seek);//读取设备当前存储位置
    if (ret<0)
    {
        perror("ioctl2");
        return -1;
    }
    printf("Current location of kernel storage is %d\n",dev_seek);
    /* 向设备内核空间写入数据 */
    ret=write(fd,buf,strlen(buf));
    if (ret<0)
    {
        perror("write");
        return -1;
    }
    /* 向设备内核空间读数据 */
    memset(buf,0,sizeof(buf));
    ret=read(fd,buf,5);
    if (ret<0)
    {
        perror("read1");
        return -1;
    }

    printf("%s",buf);
    memset(buf,0,sizeof(buf));
    ret=read(fd,buf,5);//这里再读一次观察验证内核函数是否正确
    if (ret<0)
    {
        perror("read2");
        return -1;
    }

    printf("%s\n",buf);
    /* 关闭设备 */
    close(fd);
    
    return 0;
}

