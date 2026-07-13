#ifndef __MENU_H
#define __MENU_H

#include "stm32f10x.h"
#include <stdbool.h>

/* ==================== 宏定义 ==================== */

#define NO_PARENT               0xFF
#define NO_SUBMENU              0


#define PWD_UNLOCK_SUBMENU_START    OPEN_DOOR
#define PWD_UNLOCK_SUBMENU_COUNT    4
#define SETTING_SUBMENU_START       VOLUME
#define SETTING_SUBMENU_COUNT       7

/* ==================== 枚举定义 ==================== */

typedef enum {
    PWD_UNLOCK = 0,
    CARD_UNLOCK,
    FP_UNLOCK,
    OPEN_DOOR,
    CHANGE_PWD,
    SETTING,
    ABOUT,
    VOLUME,
    UNLOCK_SOUND,
    ERR_LIMIT,
    ADD_CARD,
    DEL_CARD,
    ADD_FP,
    DEL_FP,
    MENU_TOTAL_COUNT
} Menu_t;

typedef enum {
    MENU_ROOT,
    MENU_FUNC
} MenuType_t;

/* ==================== 结构体定义 ==================== */

typedef struct {
    char* name;
    MenuType_t Type;
    void (*Func)(void);
    uint8_t parent_id;
    uint8_t Sub_start;
    uint8_t Sub_cnt;
} Menustruct_t;

/* ==================== 外部变量 ==================== */

extern const Menustruct_t All_Menu[];
extern uint8_t Current_Parent;
extern uint8_t Current_Index;
extern uint8_t Current_Offset;      // 当前选中在当前页的位置
extern uint8_t Current_Base;      // 页面基地址

/* ==================== 函数声明 ==================== */

// 必须先调用 MenuDisplay_Register 注册驱动，才能调用以下函数
void Menu_Display(uint8_t parent);
void Menu_Up(void);
void Menu_Down(void);
void Menu_Enter(void);
void Menu_Back(void);
uint8_t Menu_Next(uint8_t select, uint8_t max);
uint8_t Menu_Prev(uint8_t select, uint8_t max);

// 功能函数
void Func_PwdUnlock(void);
void Func_CardUnlock(void);
void Func_FpUnlock(void);
void Func_OpenDoor(void);
void Func_ChangePwd(void);
void Func_Setting(void);
void Func_About(void);
void Func_Volume(void);
void Func_UnlockSound(void);
void Func_ErrLimit(void);
void Func_AddCard(void);
void Func_DelCard(void);
void Func_AddFp(void);
void Func_DelFp(void);

#endif
