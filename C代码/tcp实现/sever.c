/*
 * @Author: shuxiansheng shuxianshengio@126.com
 * @Date: 2025-12-30 14:28:00
 * @LastEditors: shuxiansheng shuxianshengio@126.com
 * @LastEditTime: 2025-12-30 15:17:40
 * @FilePath: /代码/C代码/tcp实现/sever.c
 * @Description: 
 */
#include "net.h"

int main(int argc, char const *argv[])
{
    /********socket文件创建***********/
    int fd=socket(AF_INET,SOCK_STREAM,0);
    if (fd<0)
    {
        perror("socket: ");
        exit(-1);
    }

    /*******bind设置*******/
    struct sockaddr_in sin;
    memset(&sin,0,sizeof(sin));
    sin.sin_family=AF_INET;
    sin.sin_port=htons(SER_PORT);
#if 0
    sin.sin_addr=inet_addr(SER_ADDR);
#else 
    if(inet_pton(AF_INET,SER_ADDR,(void *)&sin.sin_addr)==0)
    {
        perror("inet_pton: ");
        exit(-1);
    }
#endif
    if(bind(fd,(struct sockaddr *)&sin,sizeof(sin))<0)
    {
        perror("bind: ");
        exit(-1);
    }
    /*******listen*******/
    if(listen(fd,5)<0)
    {
        perror("listen: ");
        exit(-1);
    }
    /********accept等待客户端连接*******/
    int newfd;
    if((newfd=accept(fd,NULL,NULL))<0)
    {
        perror("accept:");
        exit(-1);
    }
    /*****read*****/
    char buf[BUFSIZE];
    int ret=-1;
    while (1)
    {
        bzero((void *)buf,BUFSIZE);
        do
        {
            
        } while (condition);
        
    }
    
    return 0;
}
