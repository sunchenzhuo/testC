/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-11-21 15:19:09
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-11-24 09:11:24
 * @FilePath: /代码/C代码/跑马灯/dev_led.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef DEVLED_H
#define DEVLED_H

#include <asm/ioctl.h>

#define GPX2CON 0x11000C40
#define GPX2DAT 0x11000C44
#define GPX1CON 0x11000C20
#define GPX1DAT 0x11000C24
#define GPF3CON 0x113001E0
#define GPF3DAT 0x114001E4

#define IOCTL_TYPE 'k'
#define LED2_ON _IOR(IOCTL_TYPE, 1, int *)
#define LED2_OFF _IOR(IOCTL_TYPE, 2, int *)
#define LED3_ON _IOR(IOCTL_TYPE, 3, int *)
#define LED3_OFF _IOR(IOCTL_TYPE, 4, int *)
#define LED4_ON _IOR(IOCTL_TYPE, 5, int *)
#define LED4_OFF _IOR(IOCTL_TYPE, 6, int *)
#define LED5_ON _IOR(IOCTL_TYPE, 7, int *)
#define LED5_OFF _IOR(IOCTL_TYPE, 8, int *)

#endif