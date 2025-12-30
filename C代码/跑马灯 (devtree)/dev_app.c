/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-11-24 11:16:31
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-11-24 11:33:43
 * @FilePath: /代码/C代码/跑马灯/dev_app.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <time.h>

#include "dev_led.h"

int main(int argc, char const *argv[])
{
    int ret;
    if (argc < 2)
    {
        printf("Run Without file!\n");
        return -1;
    }
    int fd = open("/dev/dev_led", O_RDWR);
    if (fd < 0)
    {
        perror("open");
        return -1;
    }
    while (1)
    {
        ioctl(fd, LED2_ON, 0);
        sleep(1);
        ioctl(fd, LED2_OFF, 0);
        sleep(1);
        ioctl(fd, LED3_ON, 0);
        sleep(1);
        ioctl(fd, LED3_OFF, 0);
        sleep(1);
        ioctl(fd, LED4_ON, 0);
        sleep(1);
        ioctl(fd, LED4_OFF, 0);
        sleep(1);
        ioctl(fd, LED5_ON, 0);
        sleep(1);
        ioctl(fd, LED5_OFF, 0);
        sleep(1);
    }

    close(fd);

    return 0;
}
