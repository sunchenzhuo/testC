/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-11-24 15:54:27
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-11-25 10:50:58
 * @FilePath: /代码/C代码/按键/dev_key.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <asm/uaccess.h>

#include "dev_key.h"

#define BUFSIZE 100 // 内核空间大小
int major = 11;     // 主设备号
int minor = 0;      // 次设备号
int dev_num = 1;    // 驱动数量

struct pdev
{
    struct cdev keydev; // 设备结构体
    int gpio;           // 引脚设置
    int irq;            // 中断设置

    struct key_value data; // 记录按键状态
    int newflag;           // 触发中断置1，没有置0

    wait_queue_head_t dr; // 等待读队列
    spinlock_t lock;      // 保护 newflag 和 data
};
struct pdev *pkeydev = NULL; // 设备结构体

int keydev_open(struct inode *pnode, struct file *pfile) // 打开设备
{
    /*container_of宏:作用是通过结构体成员的地址找到整个结构体的起始地址
    ptr: 结构体成员的指针
    type: 包含该成员的结构体类型
    member: ptr所指向的成员在结构体中的名称 */
    pfile->private_data = (void *)(container_of(pnode->i_cdev, struct pdev, keydev));
    return 0;
}

int keydev_release(struct inode *pnode, struct file *pfile) // 关闭设备
{
    return 0;
}

ssize_t keydev_read(struct file *pfile, char __user *user_buf, size_t user_size, loff_t *seek)
{
    int size;
    int ret;
    struct pdev *gkeydev = (struct pdev *)pfile->private_data; // gkeydev从文件结构的private_data获取设备特定数据结构指针
    if (user_size < sizeof(struct key_value))
    {
        printk("expect read size invalid\n");
        return -1;
    }
    spin_lock(&gkeydev->lock); // 保护对共享数据的访问，防止竞态条件
    if (!gkeydev->newflag)
    {
        if (pfile->f_flags & O_NONBLOCK)
        {
            spin_unlock(&gkeydev->lock);
            printk("no read data\n");
            return -1;
        }
        else
        {
            ret = wait_event_interruptible(gkeydev->dr, gkeydev->newflag == 1);
            if (ret)
            {
                spin_unlock(&gkeydev->lock);
                printk("sigal wake\n");
                return -ERESTARTSYS;
            }
        }
    }
    if (user_size > sizeof(struct key_value)) // 量定读取多少
    {
        size = sizeof(struct key_value);
    }
    else
    {
        size = user_size;
    }
    ret = copy_to_user(user_buf, &gkeydev->data, size); // 读取数据
    if (ret)
    {
        spin_unlock(&gkeydev->lock);
        printk("copy_to_user1 fail!\n");
        return -1;
    }
    gkeydev->newflag = 0;
    spin_unlock(&gkeydev->lock);
    return size;
}

/*设备驱动轮询(poll)操作实现，用于检查设备是否有数据可读。*/
unsigned int keydev_poll(struct file *pfile, poll_table *wait)
{
    struct pdev *gkeydev = (struct pdev *)pfile->private_data;
    unsigned int mask = 0;

    poll_wait(pfile, &gkeydev->dr, wait);
    spin_lock(&gkeydev->lock);

    if (gkeydev->newflag)
    {
        mask |= POLLIN | POLLRDNORM;
    }
    spin_unlock(&gkeydev->lock);
    return mask;
}

struct file_operations fops =
    {
        .owner = THIS_MODULE,
        .open = keydev_open,
        .release = keydev_release,
        .read = keydev_read,
        .poll = keydev_poll,
};

irqreturn_t key_handler(int nu, void *arg)
{
    struct pdev *gkeydev = (struct pdev *)arg;
    int status = 0;
    // 读取按键当前电平
    gkeydev->data.status = gpio_get_value(pkeydev->gpio);
    mdelay(1);
    // 做一次简单消抖
    status = gpio_get_value(pkeydev->gpio);
    // 判断两次电平是否一致
    if (status != gkeydev->data.status)
    {
        return IRQ_NONE;
    }
    // 判断是否按下
    if (gkeydev->data.status != KEY_DOWN)
    {
        return IRQ_NONE;
    }
    // 进入临界区
    spin_lock(&gkeydev->lock);
    // 填写按键事件数据
    gkeydev->data.code = KEY3;
    gkeydev->data.status = KEY_DOWN;
    gkeydev->newflag = 1; // 设置 newflag=1 表示“有新数据可读

    spin_unlock(&gkeydev->lock);
    // 唤醒等待队列
    wake_up(&gkeydev->dr);

    return IRQ_HANDLED;
}

int __init mykey_init(void)
{
    dev_t devno = MKDEV(major, minor);
    int ret = -1;
    struct device_node *key_node = NULL;

    if ((ret = register_chrdev_region(devno, dev_num, "mykey")) < 0)
    {
        ret = alloc_chrdev_region(devno, minor, dev_num, "mykey");
        if (ret < 0)
        {
            printk("alloc_chrdev_region!\n");
            return -1;
        }
        major = MAJOR(devno);
    }

    /*设备分配内存空间*/
    pkeydev = (struct pdev *)kmalloc(sizeof(struct pdev), GFP_KERNEL);
    if (pkeydev == NULL)
    {
        unregister_chrdev_region(devno, dev_num);
        printk("pkeydev malloc fail!\n");
        return -1;
    }

    /* 设备初始化 */
    cdev_init(&pkeydev->keydev, &fops);
    pkeydev->keydev.owner = THIS_MODULE;

    /* 设备添加 */
    cdev_add(&pkeydev->keydev, devno, dev_num);

    /* 等待队列初始化 */
    init_waitqueue_head(&pkeydev->dr); // 初始化读等待队列头

    /* 自旋琐初始化 */
    spin_lock_init(&pkeydev->lock); // 初始化自旋锁

    /* 得到结点 */
    key_node = of_find_node_by_path("/mykey3_node");
    if (key_node == NULL)
    {
        unregister_chrdev_region(devno, dev_num);
        cdev_del(&pkeydev->keydev);
        kfree(pkeydev);
        pkeydev = NULL;
        return -1;
    }
    /* 获取按键引脚号*/
    pkeydev->gpio = of_get_named_gpio(key_node, "key3-gpio", 0); // 引脚号
    pkeydev->irq = irq_of_parse_and_map(key_node, 0);            // 中断号

    /* 按键中断初始化 */
    ret = request_irq(pkeydev->irq, key_handler, IRQF_TRIGGER_FALLING, "key_irq", pkeydev);
    if (ret < 0)
    {
        unregister_chrdev_region(devno, dev_num);
        cdev_del(&pkeydev->keydev);
        kfree(pkeydev);
        pkeydev = NULL;
        return -1;
    }
}

void __exit mykey_exit(void)
{
    dev_t devno = MKDEV(major, minor);

    free_irq(&pkeydev->irq, pkeydev);

    cdev_del(&pkeydev->keydev);

    unregister_chrdev_region(devno, dev_num);

    kfree(pkeydev);
    pkeydev = NULL;
}

MODULE_LICENSE("GPL");
module_init(mykey_init);
module_exit(mykey_exit);