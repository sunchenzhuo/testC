/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-11-24 15:33:17
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-11-24 15:54:32
 * @FilePath: /代码/C代码/按键/dev_key.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef DEVKEY_H
#define DEVKEY_H


enum KEYCODE
{
    KEY2=1002,
    KEY3,
    KEY4,
};

enum KEY_STATUS
{
    KEY_DOWN=0,
    KEY_up,
};

struct key_value
{
    int code;
    int status;
};

#endif