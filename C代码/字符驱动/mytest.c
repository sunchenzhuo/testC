/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-11-18 10:24:47
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-11-18 17:28:27
 * @FilePath: /代码/C代码/mytest.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/cdev.h>
#include<linux/fs.h>
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
    /*核心逻辑
        确定读取大小：比较用户请求的大小(user_size)和设备当前可用数据大小(buf_seek)
        取两者中较小的值作为实际读取大小，避免越界*/
    if (user_size>gmydev->buf_seek) 
    {
        size=gmydev->buf_seek;
    }
    else
    {
        size=user_size;
    }
    /*数据拷贝到用户空间：使用raw_copy_to_user将内核缓冲区的数据(dev_buf)复制到用户空间(user_buf)
        如果拷贝失败则返回错误(-1)*/
    ret=raw_copy_to_user(user_buf,gmydev->dev_buf,size);//读取数据
    if (ret)
    {
        printk("copy_to_user fail!");
        return -1;
    }
    /*缓冲区管理：将剩余数据(size之后的数据)移动到缓冲区开头，覆盖已读数据
        更新缓冲区中有效数据的偏移量(buf_seek) */
    memcpy(gmydev->dev_buf,gmydev->dev_buf+size,BUFSIZE-size);

    gmydev->buf_seek-=size;
    return size;
    
}

ssize_t mydev_write(struct file *pfile,char __user *user_buf,size_t user_size,loff_t *seek)//写设备
{
    int size;
    int ret;
    struct pdev *gmydev=pfile->private_data;// 获取设备私有数据结构

    if (user_size>BUFSIZE-gmydev->buf_seek)
    {
        size=BUFSIZE-gmydev->buf_seek;
    }
    else
    {
        size=user_size;
    }
    
    ret=raw_copy_from_user(gmydev->dev_buf+gmydev->buf_seek,user_buf,size);
    if (ret)
    {
        printk("copy_from_user fail!\n");
        return -1;
    }

    gmydev->buf_seek+=size;
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
            printk("copy_to_user fail!\n");
            return 1;
        }
        
        break;
    /* 将设备私有数据中的buf_seek成员复制到用户空间指针pret指向的位置*/
    case MYDEV_IOCTL_GET_SEEK:
        ret=raw_copy_to_user(pret,&gmydev->buf_seek,sizeof(int));
        if (ret)
        {
            printk("copy_to_user fail!\n");
            return -1;
        }
        
        break;
    default:
        printk("cmd error!\n");
        break;
    }
    return 0;
}
/*这是一个Linux内核模块的代码，实现了一个字符设备驱动。*/
struct file_operations fops={
    .open=mydev_open,//打开设备时调用
    .release=mydev_release,//关闭设备时调用
    .read=mydev_read,//读取设备数据
    .write=mydev_write,//向设备写入数据
    .unlocked_ioctl=mydev_ioctl,//设备的控制命令接口
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