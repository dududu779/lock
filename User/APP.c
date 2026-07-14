#include "stm32f10x.h"
#include "APP.h"
#include "Menu.h"
#include "MatrixKey.h"

void Key_Nav(void)
{
    // 按键导航
    if(Key_Check(Key_2, SINGLE)) Menu_Up();       // 上
    if(Key_Check(Key_8, SINGLE)) Menu_Down();     // 下
    if(Key_Check(Key_HASH, SINGLE)) Menu_Enter();    // 确认
    if(Key_Check(Key_STAR, SINGLE)) Menu_Back();  // 返回
}
/* ==================== 功能函数 ==================== */
void Func_PwdUnlock(void) {}
void Func_CardUnlock(void) {}
void Func_FpUnlock(void) {}
void Func_OpenDoor(void) {}
void Func_ChangePwd(void) {}
void Func_Setting(void) {}
void Func_About(void) {}
void Func_Volume(void) {}
void Func_UnlockSound(void) {}
void Func_ErrLimit(void) {}
void Func_AddCard(void) {}
void Func_DelCard(void) {}
void Func_AddFp(void) {}
void Func_DelFp(void) {}
