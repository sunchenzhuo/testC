/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-11-25 10:50:31
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-11-25 17:42:10
 * @FilePath: /代码/C代码/按键/key_app.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>

#include "dev_key.h"

int main(int argc, char const *argv[])
{
    struct key_value key3;
    int ret;

    if (argc < 2)
    {
        printf("Run without faile!\n");
        return -1;
    }

    /* 打开设备文件 */
    int fd = open("/dev/mykey", O_RDWR);
    if (fd < 0)
    {
        perror("open");
        return -1;
    }

    /*循环：
        清空 fd_set
        将 fd 加入 fd_set
        select 阻塞等待按键中断唤醒
        ↓
        驱动 keydev_poll() 返回可读
        select 返回
        如果 fd 可读：
            read() 获取 key3 事件（code + status）
        显示 key3 on/off*/

    /* 向设备内核空间读数据 */
    fd_set readfds;
    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        /*调用 select()*/
        ret = select(fd + 1, &readfds, NULL, NULL, NULL);
        if (ret < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                printf("select error!\n");
                break;
            }
        }
    }
    /*检查 fd 是否可读*/
    if (FD_ISSET(fd, &readfds))
    {
        /*读取按键事件*/
        ret = read(fd, &key3, sizeof(struct key_value)); // 此时驱动的 keydev_read() 被调用
        if (ret < 0)
        {
            perror("read");
            close(fd);
            return -1;
        }

        /*打印按键状态*/
        if (key3.status == 0)
        {
            printf("key3 on\n");
        }
        else
        {
            printf("key3 off\b");
        }
    }
    close(fd);

    return 0;
}
