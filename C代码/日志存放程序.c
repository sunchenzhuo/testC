/*
 * @Description: 
 * @Author: shuxiansheng
 * @Date: 2025-07-10 19:18:18
 * @LastEditTime: 2025-07-10 20:27:37
 * @LastEditors:  
 */
#include<stdio.h>
#include<unistd.h>
#include<time.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>


int main(int argc,const char *argv[])
{
    long int clock;
    struct tm *clocktim;
    int order=1;
    char str[32]={0},buf[32]={0};
    int p=open("1.txt",O_RDWR|O_CREAT|O_APPEND, 0664);
    if (p==-1)
    {
        perror("fopen:");
        return -1;
    }
    while (read(p,str,32)!=0)
    {
        if (str[strlen(str)-1]=='\0')
        {
            order++;
        }
    }
    while (1)
    {
        clock=time(NULL);
        clocktim=localtime(&clock);
        printf("%04d-%02d-%02d %02d:%02d:%02d\n", clocktim->tm_year+1900, clocktim->tm_mon+1, clocktim->tm_mday,
				clocktim->tm_hour, clocktim->tm_min, clocktim->tm_sec); //打印在标准输出，看是否正确
		printf("%s",buf);  //打印入文件
        if(write(p,buf,strlen(buf))==-1)
        {
            perror("write:");
            return -1;
        }
        memset(buf,0,sizeof(buf));
        order++;
        sleep(1);
    }
    close(p);
    return 0;
    
    
}