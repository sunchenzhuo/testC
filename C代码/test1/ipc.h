/*
 * @Author: shuxiansheng sunchenzhuo5570@163.com
 * @Date: 2025-12-19 11:14:09
 * @LastEditors: shuxiansheng sunchenzhuo5570@163.com
 * @LastEditTime: 2025-12-19 13:27:42
 * @FilePath: /代码/C代码/test1/ipc.h
 * @Description: 
 */
#ifndef IPC_H
#define IPC_H

#include <sys/types.h>

#define SHM_SIZE 256
#define SEM_WRITE 1
#define SEM_READ 0

int ipc_init(key_t key);
void ipc_cleanup(void);

void P(int sem_index);
void V(int sem_index);

char *ipc_get_shmaddr(void);


#endif