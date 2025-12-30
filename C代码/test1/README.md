<!--
 * @Author: shuxiansheng sunchenzhuo5570@163.com
 * @Date: 2025-12-19 11:16:49
 * @LastEditors: shuxiansheng sunchenzhuo5570@163.com
 * @LastEditTime: 2025-12-25 11:12:36
 * @FilePath: /代码/C代码/test1/README.md
 * @Description: 
-->
# test1

基于 System V IPC 的父子进程聊天示例。

## 技术点
- 共享内存（shm）
- 信号量（sem）
- 进程同步

## 编译
make

## 运行
./test

## 说明
父进程输入，子进程输出，一写一读。

业务逻辑
ipc.c
├── 私有全局变量（IPC 资源句柄）
│   ├── shm_id        # 共享内存 ID
│   ├── sem_id        # 信号量集 ID
│   └── shm_addr      # 映射后的共享内存地址
│
├── 内部工具函数（不对外）
│   ├── sem_P()       # 信号量 P 操作（-1）
│   └── sem_V()       # 信号量 V 操作（+1）
│
├── 对外接口实现
│   ├── ipc_init()
│   │   ├── semget()      # 创建信号量
│   │   ├── shmget()      # 创建共享内存
│   │   ├── shmat()       # 映射共享内存
│   │   ├── semctl()      # 初始化信号量
│   │   └── memset()      # 清空共享内存
│   │
│   ├── ipc_cleanup()
│   │   ├── shmdt()       # 解除映射
│   │   ├── shmctl()      # 删除共享内存
│   │   └── semctl()      # 删除信号量
│   │
│   ├── P()               # 对外的 P 操作封装
│   ├── V()               # 对外的 V 操作封装
│   └── ipc_get_shmaddr() # 返回共享内存地址

main.c
├── 信号处理
│   └── handler_signal()     # Ctrl+C 退出时清理资源
│
├── 主流程
│   ├── ftok()               # 生成 IPC key
│   ├── ipc_init()           # 初始化 IPC 资源
│   ├── ipc_get_shmaddr()    # 获取共享内存地址
│   ├── fork()               # 创建子进程
│   │
│   ├── 父进程（写端）
│   │   ├── P(SEM_WRITE)
│   │   ├── 从 stdin 读数据
│   │   ├── 写入共享内存
│   │   └── V(SEM_READ)
│   │
│   └── 子进程（读端）
│       ├── P(SEM_READ)
│       ├── 从共享内存读数据
│       └── V(SEM_WRITE)
│
└── 程序退出
    └── ipc_cleanup()

ipc.h
├── 宏定义
│   ├── SHM_SIZE   定义共享内存大小
│   ├── SEM_READ   读信号量（控制读权限
│   └── SEM_WRITE  写信号量（控制写权限）
│
├── 函数声明
│   ├── int  ipc_init(key_t key);   初始化 IPC 资源
│   ├── void ipc_cleanup(void);     释放 IPC 资源
│   ├── void P(int sem_index);      信号量 P 操作（申请资源）
│   ├── void V(int sem_index);      信号量 V 操作（释放资源）
│   └── char *ipc_get_shmaddr(void);  获取共享内存映射地址