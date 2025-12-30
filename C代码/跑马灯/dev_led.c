/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-11-21 15:11:19
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-11-24 11:16:06
 * @FilePath: /代码/C代码/跑马灯/dev_led.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/io.h>
#include <linux/slab.h>

#include "dev_led.h"

int major = 11;  // 主设备号
int minor = 0;   // 次设备号
int dev_num = 1; // 设备数量

struct pdev // 设备结构体
{
    struct cdev leddev;              // led结构体
    volatile unsigned long *led2con; // 存放led2的功能寄存器值，下面类似
    volatile unsigned long *led2dat; // 控制led2的数据寄存器，下面类似

    volatile unsigned long *led3con;
    volatile unsigned long *led3dat;

    volatile unsigned long *led4con;
    volatile unsigned long *led4dat;

    volatile unsigned long *led5con;
    volatile unsigned long *led5dat;
};
struct pdev *pleddev = NULL;

int led_open(struct inode *pnode, struct file *pfile) // 打开文件设备
{
    /*硬件地址映射*/
    pleddev->led2con = ioremap(GPX2CON, sizeof(int));
    pleddev->led2dat = ioremap(GPX2DAT, sizeof(int));
    pleddev->led3con = ioremap(GPX1CON, sizeof(int));
    pleddev->led3dat = ioremap(GPX1DAT, sizeof(int));
    pleddev->led4con = ioremap(GPF3CON, sizeof(int));
    pleddev->led4dat = ioremap(GPF3DAT, sizeof(int));
    pleddev->led5con = pleddev->led4con;
    pleddev->led5dat = pleddev->led4dat;

    /*led初始化*/
    /* 初始化 LED2 (GPX2_7) */
    writel(readl(pleddev->led2con) & (~(0xF << 28)) | (0x1 << 28), pleddev->led2con);
    writel(readl(pleddev->led2dat) & (~(0x1 << 7)), pleddev->led2dat);

    /* LED3 (GPX1_0) */
    writel(readl(pleddev->led3con) & (~(0xF << 0)) | (0x1 << 0), pleddev->led3con);
    writel(readl(pleddev->led3dat) & (~(0x1 << 0)), pleddev->led3dat);

    /* LED4 (GPF3_4) */
    writel(readl(pleddev->led4con) & (~(0xF << 16)) | (0x1 << 16), pleddev->led4con);
    writel(readl(pleddev->led4dat) & (~(0x1 << 4)), pleddev->led4dat);

    /* LED5 (GPF3_5) */
    writel(readl(pleddev->led5con) & (~(0xF << 20)) | (0x1 << 20), pleddev->led5con);
    writel(readl(pleddev->led5dat) & (~(0x1 << 5)), pleddev->led5dat);

    return 0;
}

int led_release(struct inode *pnode, struct file *pfile) // 关闭设备
{
    /*取消映射*/
    iounmap(pleddev->led2con);
    iounmap(pleddev->led2dat);
    iounmap(pleddev->led3con);
    iounmap(pleddev->led3dat);
    iounmap(pleddev->led4con);
    iounmap(pleddev->led4dat);
    iounmap(pleddev->led5con);
    iounmap(pleddev->led5dat);
    return 0;
}

int leddev_ioctl(struct file *pfile, unsigned int cmd, unsigned long arg)
{
    switch (cmd)
    {
    case LED2_ON:
        writel(readl(pleddev->led2dat) | (0x1 << 7), pleddev->led2dat); // 0x1 << 7 = 设置 第7位为1  | = OR 操作 → 把 bit7 设置为 1
        break;
    case LED2_OFF:
        writel(readl(pleddev->led2dat) & (~(0x1 << 7)), pleddev->led2dat); //~(0x1 << 7) = bit7 清零掩码  & = AND 操作 → 清除 bit7
        break;
    case LED3_ON:
        writel(readl(pleddev->led3dat) | (0x1 << 0), pleddev->led3dat);
        break;
    case LED3_OFF:
        writel(readl(pleddev->led3dat) & (~(0x1 << 0)), pleddev->led3dat);
        break;
    case LED4_ON:
        writel(readl(pleddev->led4dat) | (0x1 << 4), pleddev->led4dat);
        break;
    case LED4_OFF:
        writel(readl(pleddev->led4dat) & (~(0x1 << 4)), pleddev->led4dat);
        break;
    case LED5_ON:
        writel(readl(pleddev->led5dat) | (0x1 << 5), pleddev->led5dat);
        break;
    case LED5_OFF:
        writel(readl(pleddev->led5dat) & (~(0x1 << 5)), pleddev->led5dat);
        break;
    default:
        printk("cmd error!\n");
        return -1;
    }
    return 0;
}

struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .release = led_release,
    .unlocked_ioctl = leddev_ioctl,

};

int __init led_init(void)
{
    // 设备号分配与注册
    dev_t devno = MKDEV(major, minor);
    int ret = -1;
    if ((ret = register_chrdev_region(devno, dev_num, "dev_led")) < 0) // 尝试静态注册指定的设备号范围
    {
        ret = alloc_chrdev_region(&devno, minor, dev_num, "dev_led");
        if (ret < 0)
        {
            printk("alloc_chrdev_region fail\n!");
            return -1;
        }
        major = MAJOR(devno);
    }

    /*设备分配存储空间内存*/
    pleddev = (struct pdev *)kmalloc(sizeof(struct pdev), GFP_KERNEL); // kmalloc()：在内核空间动态分配内存GFP_KERNEL：标准的内核内存分配标志
    if (pleddev == NULL)
    {
        unregister_chrdev_region(devno, dev_num); // 释放之前注册的设备号
        printk("pmydev malloc fail\n");
        return -1;
    }

    /*设备初始化*/
    cdev_init(&pleddev->leddev, &fops);
    pleddev->leddev.owner = THIS_MODULE;

    /*设备添加*/
    cdev_add(&pleddev->leddev, devno, dev_num);
    return 0;
}

void __exit led_exit(void)
{
    dev_t devno = MKDEV(major, minor);

    cdev_del(&pleddev->leddev);

    unregister_chrdev_region(devno, dev_num);

    kfree(pleddev);
}

MODULE_LICENSE("GPL");
module_init(led_init);
module_exit(led_exit);