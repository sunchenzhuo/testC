/*
 * @Author: shuxiansheng sunchenzhuo5570@163.com
 * @Date: 2025-12-25 14:48:19
 * @LastEditors: shuxiansheng shuxianshengio@126.com
 * @LastEditTime: 2026-01-04 10:07:45
 * @FilePath: /代码/C代码/多线程并发/net.h
 * @Description: 集中声明“网络通信相关的公共配置和接口”，让网络程序做到“配置统一、代码解耦、易维护”
 */
#ifndef NET_H
#define NET_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>

#define SER_PORT 5001//端口号
#define SER_ADDR "192.168.182.128"//服务器IP地址
#define BUFSIZE 15//发送数据大小
#define USER_QUIT "quit"//用户退出标志
#define BACK_LOG 5//监听队列长度

#endif // NET_H