#include "stm32f10x.h"
#include <string.h>
#include "Timer.h"
#include "OLED.h"
#include "MatrixKey.h"
#include "Delay.h"
#include "Menu_Display.h"
#include "Menu.h"          

// OLED 适配器参数
#define OLED_LINE_HEIGHT    16
#define OLED_FONT_SIZE      OLED_8X16
#define OLED_SCREEN_WIDTH   128
#define OLED_MAX_LINES      4

static void OLED_Adapter_Clear(void) { OLED_Clear(); }

static void OLED_Adapter_DrawString(uint8_t line, const char* str)
{
    OLED_ShowString(0, line * OLED_LINE_HEIGHT, (char*)str, OLED_FONT_SIZE);
}

static void OLED_Adapter_Highlight(uint8_t line)
{
    OLED_ReverseArea(0, line * OLED_LINE_HEIGHT, OLED_SCREEN_WIDTH, OLED_LINE_HEIGHT);
}

static void OLED_Adapter_Refresh(void) { OLED_Update(); }

static uint8_t OLED_Adapter_GetMaxLines(void) { return OLED_MAX_LINES; }

const MenuDisplay_Driver_t OLED_MenuDriver = {
    .Clear       = OLED_Adapter_Clear,
    .DrawString  = OLED_Adapter_DrawString,
    .Highlight   = OLED_Adapter_Highlight,
    .Refresh     = OLED_Adapter_Refresh,
    .GetMaxLines = OLED_Adapter_GetMaxLines
};

int main(void)
{
    Timer_Init();
    OLED_Init();
    Key_Init();
    
    // ← 注册驱动 + 显示初始菜单
    MenuDisplay_Register(&OLED_MenuDriver);
    Menu_Display(NO_PARENT);  // 显示根菜单
    
    while (1)
    {   
        // 按键导航（根据你的矩阵键盘布局调整按键）
        if(Key_Check(Key_2, SINGLE)) Menu_Up();       // 上
        if(Key_Check(Key_8, SINGLE)) Menu_Down();     // 下
        if(Key_Check(Key_HASH, SINGLE)) Menu_Enter();    // 确认
        if(Key_Check(Key_STAR, SINGLE)) Menu_Back();  // 返回
    }
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        Key_EvenTick();
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

