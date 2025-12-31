/*
 * @Author: shuxiansheng shuxianshengio@126.com
 * @Date: 2025-12-25 14:48:52
 * @LastEditors: shuxiansheng shuxianshengio@126.com
 * @LastEditTime: 2025-12-31 16:05:57
 * @FilePath: /代码/C代码/tcp实现/client.c
 * @Description:
 */
#include "net.h"
int main(int argc, char const *argv[])
{
    /*******socket********/
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket");
        exit(-1);
    }
    /*******connet配置********/
    /*******配置服务器结构体********/
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));

    /*******填充 sockaddr_in********/
    sin.sin_family = AF_INET;
    sin.sin_port = htons(SER_PORT);
#if 0:
    sin.sin_addr=inet_addr(SER_ADDR);
#else
    if (inet_pton(AF_INET, SER_ADDR, (void *)&sin.sin_addr) == 0)
    {
        perror("inet_pton");
        exit(-1);
    }
#endif
    /*******发起连接********/
    if (connect(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        perror("connect");
        exit(-1);
    }
    printf("client connect success\n");
    /*******数据收发********/
    /*******数据收发循环********/
    char buf[BUFSIZE] = {0};
    int ret = -1;
    while (1)
    {
        bzero(buf, 0);
        /****从键盘读取数据****/
        if (fgets(buf, BUFSIZE - 1, stdin) == 0)
        {
            continue;
        }
        /****发送数据****/
        do
        {
            ret = write(fd, buf, strlen(buf));
        } while (ret < 0 && errno == EINTR);
        /******判断退出命令******/
        if (!strncasecmp(buf, USER_QUIT, strlen(USER_QUIT)))
        {
            printf("Client is exiting!\n");
            break;
        }
    }
    close(fd);
    return 0;
}
