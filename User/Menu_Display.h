#ifndef __MENU_DISPLAY_H
#define __MENU_DISPLAY_H

#include "stm32f10x.h"

/* ==================== 显示驱动接口（抽象层）==================== */

// 菜单显示需要的功能接口
typedef struct {
    void (*Clear)(void);                                    // 清屏
    void (*DrawString)(uint8_t line, const char* str);      // 在第 line 行显示字符串
    void (*Highlight)(uint8_t line);                        // 高亮第 line 行
    void (*Refresh)(void);                                  // 刷新屏幕
    uint8_t (*GetMaxLines)(void);                           // 获取屏幕最大显示行数
} MenuDisplay_Driver_t;

/* ==================== 注册与使用 ==================== */

// 向菜单系统注册显示驱动（main.c 中调用一次）
void MenuDisplay_Register(const MenuDisplay_Driver_t* driver);

// 获取当前注册的驱动（Menu.c 内部使用）
const MenuDisplay_Driver_t* MenuDisplay_GetDriver(void);

#endif
