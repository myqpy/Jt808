#ifndef JT808_TERMINAL_CONTROL_H_
#define JT808_TERMINAL_CONTROL_H_

#include <stdio.h>
#include <stdbool.h>
//#include <fcntl.h>
#include <stdint.h>
#include "util.h"

// 0x8105终端控制命令字.
union TerminalCtrlCmdByte
{
    struct
    {
        unsigned  char  u8val_0 : 1; //
        unsigned  char  u8val_1 : 1; // 无线升级
        unsigned  char  u8val_2 : 1; // 控制终端连接指定服务器
        unsigned  char  u8val_3 : 1; // 终端关机
        unsigned  char  u8val_4 : 1; // 终端复位
        unsigned  char  u8val_5 : 1; // 终端恢复出厂设置
        unsigned  char  u8val_6 : 1; // 关闭数据通信
        unsigned  char  u8val_7 : 1; // 关闭所有无线通信
    } bit;
    unsigned  char  u8val;
};

// 0x8105终端控制.
struct TerminalControl
{
    // TerminalCtrlCmdByte cmdByte; // 命令字
    unsigned  char  cmdByte;                 // 命令字   1:无线升级；2:控制终端连接指定服务器；3:终端关机；4:终端复位；5:终端恢复出厂设置；6:关闭数据通信;7:关闭所有无线通信
    const char *cmdParameter; // 命令参数
};

#endif
