/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-11-18 11:29:23
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-11-18 11:42:17
 * @FilePath: /代码/C代码/mytest.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef MYTEST_H
#define MYTEST_H

#include<asm/ioctl.h>

#define IOCTL_TYPE 'k'
#define MYDEV_IOCTL_GET_MUTEX _IOR(IOCTL_TYPE,1,int)
#define MYDEV_IOCTL_GET_SEEK _IOR(IOCTL_TYPE,2,int)
#endif