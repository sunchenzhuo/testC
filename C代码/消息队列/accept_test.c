/*
 * @Author: shuxiansheng sunchenzhuo5570@163.com
 * @Date: 2025-12-18 10:18:02
 * @LastEditors: shuxiansheng sunchenzhuo5570@163.com
 * @LastEditTime: 2025-12-18 11:11:36
 * @FilePath: /代码/C代码/消息队列/accept_test.c
 * @Description: 消息队列接收消息
 */
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>

#define MSGSIZE (sizeof(msgT)-sizeof(long))

typedef struct msg{
    long msg_type;
    char buf[128];
}msgT;

int main(int argc, char const *argv[])
{
    key_t key;
    int get,re,count=0;
    msgT msg;

    key=ftok(".",100);
    if(key<0){
        perror("ftok:");
        exit(-1);
    }

    get=msgget(key,IPC_CREAT|0666);
    if(get<0){
        perror("msgget:");
        exit(-1);
    }

    while (1)
    {
        re=msgrcv(get,&msg,MSGSIZE,0,0);
        if(re<0){
            perror("msgrcv:");
            exit(-1);
        }
        count++;
        if (count==4)
        {
            break;
        }
        printf("msgrcv:%s \n",msg.buf);
    }
    msgctl(get,IPC_RMID,0);
    return 0;
}
