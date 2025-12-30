/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-11-18 10:24:47
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-11-20 17:26:11
 * @FilePath: /代码/C代码/mytest.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/wait.h>
#include<linux/poll.h>
#include<linux/sched.h>
#include<linux/fcntl.h>
#include<linux/uaccess.h>
#include<linux/init.h>


#include "mytest.h"

#define BUFSIZE 100//模拟内核空间大小
int major=11;//主设备号
int minor=0;//从设备号
int dev_num=1;//设备数量

struct pdev{
    struct cdev mydev;//设备结构体
    char dev_buf[BUFSIZE];//模拟内核空间
    int buf_seek;//内核空间存储值

    wait_queue_head_t dr;// 唤醒等待读队列的进程
    wait_queue_head_t dw;// 唤醒等待写队列的进程

    struct fasync_struct *pasync_obj;//驱动信号对象
};

struct pdev pmydev;

int mydev_open(struct inode *pnode,struct file *pfile)//打开设备
{
    pfile->private_data=(void *)(container_of(pnode->i_cdev,struct pdev,mydev));//获取设备结构体地址
    return 0;
}
int mydev_release(struct inode *pnode,struct file *pfile)//关闭设备
{
    return 0;
}

ssize_t mydev_read(struct file *pfile,char __user *user_buf,size_t user_size,loff_t *seek)//读设备
{
    /*gmydev通过pfile->private_data获取，这是在设备打开时设置的设备特定数据结构*/
    int size;// 实际要读取的数据大小
    int ret;// 操作返回值
    struct pdev *gmydev =pfile->private_data;// 获取设备私有数据结构
    
    printk("%s\n",__FUNCTION__);
    if (gmydev->buf_seek<=0)//检查设备结构体gmydev中的缓冲区位置/大小指示器buf_seek
    {
        if (pfile->f_flags & O_NONBLOCK)//非阻塞模式处理
        {
            printk("no data read\n");
            return 1;
        }
        else
        {
            ret=wait_event_interruptible(gmydev->dr,gmydev->buf_seek>0);//阻塞模式处理
            if (ret)//判断是否为信号唤醒
            {
                printk("sigal wake!\n");
                return -ERESTARTSYS;
            }
            
        }
        
        
    }
    printk("-----------2----------\n");
    //数据大小确定
    if (user_size>gmydev->buf_seek)
    {
        size=gmydev->buf_seek;
    }
    else
    {
        size=user_size;
    }
    printk("-----------3----------\n");
    //数据拷贝到用户空间
    ret=copy_to_user(user_buf,gmydev->dev_buf,size);//读取数据
    if (ret)
    {
        printk("copy_to_user1 fail!");
        return -1;
    }
    printk("-----------4----------\n");
    //缓冲区数据处理,将剩余未被读取的数据向前移动
    memcpy(gmydev->dev_buf,gmydev->dev_buf+size,gmydev->buf_seek-size);

    gmydev->buf_seek-=size;//减少缓冲区的有效数据计数器，反映已经被读取走的数据量。

    printk("-----------5----------\n");
    //唤醒等待进程
    wake_up_interruptible(&gmydev->dw);

    printk("-----------6----------\n");
    return size;
}

ssize_t mydev_write(struct file *pfile,char __user *user_buf,size_t user_size,loff_t *seek)//写设备
{
    int size;
    int ret;
    struct pdev *gmydev=pfile->private_data;// 获取设备私有数据结构

    //检查缓冲区是否已满
    if (gmydev->buf_seek>=BUFSIZE)//非阻塞模式
    {
        if (pfile->f_flags & O_NONBLOCK)
        {
            printk("buf full!\n");
            return -1;
        }
        else//阻塞模式
        {
            ret=wait_event_interruptible(gmydev->dw,gmydev->buf_seek<BUFSIZE);//加入写等待队列
            if (ret)//判断是否为信号唤醒
            {
                printk("sigal wake\n");
                return -ERESTARTSYS;
            }
            
        }
        
    }
    //确定实际写入大小
    if (user_size>BUFSIZE-gmydev->buf_seek)//量定写入多少
    {
        size=BUFSIZE-gmydev->buf_seek;
    }
    else
    {
        size=user_size;
    }
    //从用户空间拷贝数据
    ret=copy_from_user(gmydev->dev_buf+gmydev->buf_seek,user_buf,size);//写入数据
    if (ret)//判断是否为信号唤醒
    {
        printk("copy_from_user fail!\n");
        return -1;
    }

    //更新缓冲区位置和唤醒读进程
    gmydev->buf_seek+=size;//增加已写入的数据量，更新缓冲区位置

    wake_up_interruptible(&gmydev->dr);//唤醒等待读

    //Fasync通知机制(可选)
    if (gmydev->pasync_obj!=NULL)
    {
        kill_fasync(&gmydev->pasync_obj,SIGIO,POLL_IN);//向app发送读信号
    }
    
    return size;
}

/*这是一个Linux内核设备驱动中的ioctl（输入/输出控制）函数实现。ioctl是设备驱动中用于执行特定设备命令的标准接口。*/
long mydev_ioctl(struct file *pfile,unsigned int cmd,unsigned long arg)
{
    int __user *pret=(int *)arg;//将用户空间的arg参数转换为指向整型的用户空间指针
    int maxlen=BUFSIZE;//初始化为BUFSIZE
    int ret;// 用于存储操作返回值
    struct pdev *gmydev=pfile->private_data;

    switch (cmd)
    {
    /*将内核中的maxlen值(BUFSIZE)复制到用户空间指针pret指向的位置*/
    case MYDEV_IOCTL_GET_MUTEX:
        ret=raw_copy_to_user(pret,&maxlen,sizeof(int));//进行内核到用户空间的数据拷贝
        if (ret)
        {
            printk("copy_to_user1 fail!\n");
            return -1;
        }
        
        break;
    /* 将设备私有数据中的buf_seek成员复制到用户空间指针pret指向的位置*/
    case MYDEV_IOCTL_GET_SEEK:
        ret=raw_copy_to_user(pret,&gmydev->buf_seek,sizeof(int));
        if (ret)
        {
            printk("copy_to_user2 fail!\n");
            return -1;
        }
        
        break;
    default:
        printk("cmd error!\n");
        break;
    }
    return 0;
}

/*Linux 设备驱动中实现异步通知的两个关键机制：poll 和 fasync。*/
unsigned int mydev_poll(struct file *pfile,poll_table *wait)

    /*
    功能说明
    这个函数实现了设备的轮询接口,
    允许用户空间程序检查设备是否可读或可写而不阻塞。
    */

{
    struct pdev *gmydev=(struct pdev *)pfile->private_data;
    unsigned int mask=0;

    poll_wait(pfile,&gmydev->dr,wait);
    poll_wait(pfile,&gmydev->dw,wait);

    if (gmydev->buf_seek>0)//缓冲区中有数据可读
    {
        mask|=POLLIN|POLLRDNORM;//POLLIN: 有数据可读,POLLRDNORM: 普通数据可读
    }
    if (gmydev->buf_seek<BUFSIZE)//缓冲区有空间可写
    {
        mask |=POLLOUT|POLLWRNORM;//POLLOUT: 可以写入而不阻塞,POLLWRNORM: 等同于POLLOUT
    }
    return mask;
    
}
//mydev_fasync函数
static int mydev_fasync(int fd,struct file *pfile,int mode)
{
    /*
    功能说明
    这个函数实现了设备的异步通知机制(fasync)，
    允许驱动程序在事件发生时向应用程序发送信号。
    */
    struct pdev *gmydev=(struct pdev *)pfile->private_data;
    return fasync_helper(fd,pfile,mode,&gmydev->pasync_obj);
}

/*这是一个Linux内核模块的代码，实现了一个字符设备驱动。*/
struct file_operations fops={
    .open=mydev_open,//打开设备时调用
    .release=mydev_release,//关闭设备时调用
    .read=mydev_read,//读取设备数据
    .write=mydev_write,//向设备写入数据
    .unlocked_ioctl=mydev_ioctl,//设备的控制命令接口
    .poll=mydev_poll,
    .fasync=mydev_fasync,
};

int __init mytest1_init(void)
{
    //创建设备号
    dev_t devno=MKDEV(major,minor);
    int ret=-1;
    
    //注册设备号区域
    if((ret=register_chrdev_region(devno,dev_num,"mytest1"))<0)//先尝试静态注册已知的设备号
    {
        ret=alloc_chrdev_region(&devno,minor,dev_num,"mytest1");//如果失败则动态分配
        if (ret<0)
        {
            printk("alloc_chrdev_region fail!\n");
            return -1;
        }
        major=MAJOR(devno);
    }
    //初始化字符设备
    fops.owner=THIS_MODULE;
    cdev_init(&pmydev.mydev,&fops);
    pmydev.mydev.owner=THIS_MODULE;
    // 添加字符设备到系统
    cdev_add(&pmydev.mydev,devno,dev_num);

    /*等待队列初始化*/
    init_waitqueue_head(&pmydev.dr);//初始化读等待队列头
    init_waitqueue_head(&pmydev.dw);//初始化写等待队列头

    return 0;
}
//模块退出函数(mytest1_exit)
void __exit mytest1_exit(void)
{
    dev_t devno=MKDEV(major,minor);

    cdev_del(&pmydev.mydev);

    unregister_chrdev_region(devno,dev_num);
}
//Linux内核模块相关宏

MODULE_LICENSE("GPL");//声明模块许可证(GPL)
module_init(mytest1_init);//声明模块许可证(GPL)
module_exit(mytest1_exit);//声明模块许可证(GPL)