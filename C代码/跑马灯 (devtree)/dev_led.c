/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-11-21 15:11:19
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-11-24 14:56:53
 * @FilePath: /代码/C代码/跑马灯/dev_led.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/slab.h>

#include "dev_led.h"

int major = 11;  // 主设备号
int minor = 0;   // 次设备号
int dev_num = 1; // 设备数量

struct pdev // 设备结构体
{
    struct cdev leddev; // led结构体
    int led2_num;
    int led3_num;
    int led4_num;
    int led5_num;
};
struct pdev *pleddev = NULL;

int led_open(struct inode *pnode, struct file *pfile) // 打开文件设备
{
    gpio_request(pleddev->led2_num, "led2");
    gpio_request(pleddev->led3_num, "led3");
    gpio_request(pleddev->led4_num, "led4");
    gpio_request(pleddev->led5_num, "led5");
    return 0;
}

int led_release(struct inode *pnode, struct file *pfile) // 关闭设备
{
    gpio_free(pleddev->led2_num);
    gpio_free(pleddev->led3_num);
    gpio_free(pleddev->led4_num);
    gpio_free(pleddev->led5_num);
    return 0;
}

int leddev_ioctl(struct file *pfile, unsigned int cmd, unsigned long arg)
{
    switch (cmd)
    {
    case LED2_ON:
        gpio_direction_output(pleddev->led2_num, 1);
        break;
    case LED2_OFF:
        gpio_direction_output(pleddev->led2_num, 0);
        break;
    case LED3_ON:
        gpiod_direction_output(pleddev->led3_num, 1);
        break;
    case LED3_OFF:
        gpio_direction_output(pleddev->led3_num, 0);
        break;
    case LED4_ON:
        gpio_direction_output(pleddev->led4_num, 1);
        break;
    case LED4_OFF:
        gpio_direction_output(pleddev->led4_num, 0);
        break;
    case LED5_ON:
        gpio_direction_output(pleddev->led5_num, 1);
        break;
    case LED5_OFF:
        gpio_direction_output(pleddev->led5_num, 0);
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

    struct device_node *led_node = of_find_node_by_path("/fs4412-leds");
    if (led_node == NULL)
    {
        printk("device_node get fail!\n");
        return -1;
    }

    pleddev->led2_num = of_get_named_gpio(led_node, "led2_num", 0);
    pleddev->led3_num = of_get_named_gpio(led_node, "led3_num", 0);
    pleddev->led4_num = of_get_named_gpio(led_node, "led4_num", 0);
    pleddev->led5_num = of_get_named_gpio(led_node, "led5_num", 0);

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