/*
 * @Author: shuxiansheng shuxianshengio@126.com
 * @Date: 2025-12-30 14:28:00
 * @LastEditors: shuxiansheng shuxianshengio@126.com
 * @LastEditTime: 2026-01-04 16:00:33
 * @FilePath: /代码/C代码/多线程并发/sever.c
 * @Description:
 */
#include "net.h"

void *client_hander(void *arg);
int main(int argc, char const *argv[])
{
    int i = 0;
    /********socket文件创建***********/
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket: ");
        exit(-1);
    }

    /*******bind设置*******/
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin)); // 清零操作

    sin.sin_family = AF_INET;
    sin.sin_port = htons(SER_PORT);
    /********优化1********/
#if 1
    sin.sin_addr.s_addr = htonl(INADDR_ANY); // 当系统IP地址变化时，这里的地址也会发生变化,监听本机所有网卡 IP
#else
    if (inet_pton(AF_INET, SER_ADDR, (void *)&sin.sin_addr) == 0)
    {
        perror("inet_pton: ");
        exit(-1);
    }
#endif
    if (bind(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        perror("bind: ");
        exit(-1);
    }

    /*******listen*******/
    if (listen(fd, BACK_LOG) < 0) // 设置为服务器模式，与最大接收数
    {
        perror("listen: ");
        exit(-1);
    }
    /*************多线程优化************/
    /************accept 阻塞等待客户端*********/
    // accept 新连接，并创建线程处理
    while (1)
    {
#if 0
        int newfd;
        if ((newfd = accept(fd, NULL, NULL)) < 0)
        {
        perror("accept:");
        exit(-1);
        }
#else
        int newfd[BACK_LOG];
        struct sockaddr_in cin;
        char client_ip[16] = {0};
        socklen_t cin_len = (socklen_t)sizeof(cin);

        memset(&cin, 0, sizeof(cin));

        // 阻塞等待客户端连接
        if ((newfd[i] = accept(fd, (struct sockaddr *)&cin, &cin_len)) < 0) // 阻塞接收
        {
            perror("accept:");
            exit(-1);
        }
        // 打印客户端信息
        if (inet_ntop(AF_INET, (void *)&cin.sin_addr, client_ip, sizeof(cin)) == NULL)
        {
            perror("inet_ntop:");
            exit(-1);
        }
        printf("client Port:%d,IP:%s\n", ntohs(cin.sin_port), client_ip);
#endif
        pthread_t tid;
        // 创建线程，把 newfd[i] 交给线程处理
        if (pthread_create(&tid, NULL, client_hander, (void *)&newfd[i]) != 0)
        {
            printf("tid create error\n");
            exit(-1);
        }
        i++;
    }
    close(fd);
    return 0;
}

// 每个客户端一个线程，循环读数据
void *client_hander(void *newfd)
{
    pthread_detach(pthread_self()); // 设置线程分离属性，线程结束时自动回收
    /********read********/
    char buf[BUFSIZE];
    int ret = -1;
    while (1)
    {
        bzero((void *)buf, BUFSIZE);
        do
        {
            // 循环 read
            ret = read(*(int *)newfd, (void *)buf, BUFSIZE - 1);
        } while (ret < 0 && EINTR == errno); // 被信号中断，继续读
        if (ret < 0)
        {
            perror("read");
            exit(-1);
        }
        if (ret == 0)
        {
            printf("fd:%d client break link!\n", *(int *)newfd);
            break;
        }
        printf("fd:%d read:%s\n", *(int *)newfd, buf);
        if (strncasecmp(buf, USER_QUIT, strlen(USER_QUIT)) == 0)
        {
            printf("fd:%d client choice break link!\n", *(int *)newfd);
            break;
        }
    }
    /****close****/
    close(*(int *)newfd);
    return NULL;
}