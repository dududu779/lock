#ifndef __MATRIXKEY_H
#define __MATRIXKEY_H

//=============================================================================
#define ROW                         4   // 行数
#define COL                         3   // 列数
#define Key_COUNT				    12  // 按键数量


// 按键键码定义
//=============================================================================
#define Key_0     0
#define Key_1     1
#define Key_2     2
#define Key_3     3
#define Key_4     4
#define Key_5     5
#define Key_6     6
#define Key_7     7
#define Key_8     8
#define Key_9     9
#define Key_STAR  10   // * 号
#define Key_HASH  11   // # 号
#define Key_NONE  0xFF // 无按键

// 按键状态定义
//=============================================================================
#define KEY_PRESSED				    1//按键按下
#define KEY_UNPRESSED			    0//按键松开

// 按键事件定义
//=============================================================================
#define HOLD				    0x01//按下不放（按下置一，松开置零）
#define DOWN				    0x02//按键下降沿
#define UP					    0x04//按键上升沿
#define SINGLE				    0x08//单击事件
#define DOUBLE				    0x10//双击事件
#define LONG				    0x20//长按事件（按下时间超过长按时间之后置一）
#define REPEAT				    0x40//重复时间

#define KEY_TIME_DOUBLE			100//双击时间间隔
#define KEY_TIME_LONG			2000//长按时间间隔
#define KEY_TIME_REPEAT			100//重复执行时间

// 函数声明
//=============================================================================
void Key_Init(void);
void Key_EvenTick(void);
uint8_t Key_Check(uint8_t n,uint8_t Flag);

#endif
