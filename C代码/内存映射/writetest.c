/*
 * @Author: shuxiansheng sunchenzhuo5570@163.com
 * @Date: 2025-12-15 11:07:39
 * @LastEditors: shuxiansheng sunchenzhuo5570@163.com
 * @LastEditTime: 2025-12-15 14:47:20
 * @FilePath: /代码/C代码/内存映射/writetest.c
 * @Description: 写共享文件
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    int len, fd=open("1.txt", O_RDWR);
    void *map;
    if (fd==-1)
    {
        perror("open:");
        return -1;
    }
    
    len=lseek(fd, 0, SEEK_END);
    map=mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map==MAP_FAILED)
    {
        perror("mmap:");
        
    }
    while (1)
    {
        memcpy(map++, "a", 1);
        sleep(1);
    }
    return 0;
}
