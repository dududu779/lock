#include "stm32f10x.h"
#include <string.h>
#include "Timer.h"
#include "MatrixKey.h"
#include "Delay.h"
#include "Menu_Display.h"
#include "Menu.h"          
#include "APP.h"


int main(void)
{
    Timer_Init();
    OLED_MenuDriver.Init();
    Key_Init();
    
    //  注册驱动 + 显示初始菜单
    MenuDisplay_Register(&OLED_MenuDriver);
    Menu_Display(NO_PARENT);  // 显示根菜单
    
    while (1)
    {   
        Key_Nav();  // 按键导航
        
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

