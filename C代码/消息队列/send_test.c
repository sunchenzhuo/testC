/*
 * @Author: shuxiansheng sunchenzhuo5570@163.com
 * @Date: 2025-12-18 10:14:06
 * @LastEditors: shuxiansheng sunchenzhuo5570@163.com
 * @LastEditTime: 2025-12-18 11:10:19
 * @FilePath: /代码/C代码/消息队列/send_test.c
 * @Description: 发送消息队列
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MSGSIZE (sizeof(msgT)-sizeof(long))

typedef struct msgT {
    long msg_type;
    char buf[128];
} msgT;

int main(int argc, char const *argv[])
{
    key_t key;
    int get,send;
    msgT msg;

    key = ftok(".", 100);
    if (key<0)
    {
        perror("ftok:");
        exit(1);
    }

    key=msgget(key, IPC_CREAT|0666);
    if (get<0)
    {
        perror("msgget:");
        exit(-1);
    }
    msg.msg_type=1;
    strcpy(msg.buf, "this is message 1");
    send=msgsnd(key,&msg,MSGSIZE,0);
    if (send<0)
    {
        perror("msgsnd:");
        exit(-1);
    }

    msg.msg_type=2;
    strcpy(msg.buf, "this is message 2");
    send=msgsnd(key,&msg,MSGSIZE,0);
    if (send<0)
    {
        perror("msgsnd:");
        exit(-1);
    }
    
    msg.msg_type=3;
    strcpy(msg.buf, "this is message 3");
    send=msgsnd(key,&msg,MSGSIZE,0);
    if (send<0)
    {
        perror("msgsnd:");
        exit(-1);
    }

    msg.msg_type=4;
    strcpy(msg.buf, "this is message 4");
    send=msgsnd(key,&msg,MSGSIZE,0);
    if (send<0)
    {
        perror("msgsnd:");
        exit(-1);
    }
    return 0;
}
