#include "stm32f10x.h"                  // Device header

/**
  * 函    数：定时中断初始化（使用TIM2）
  * 参    数：无
  * 返 回 值：无
  */
void Timer_Init(void)
{
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);            // 开启TIM2的时钟  

    /*配置时钟源*/
    TIM_InternalClockConfig(TIM2);      // 选择TIM2为内部时钟 

    /*时基单元初始化*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;                // ARR
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;               // PSC
    //删掉了 TIM_RepetitionCounter
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);            

    /*中断输出配置*/
    TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);                 

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);                    

    /*NVIC配置*/
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;                
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

    /*TIM使能*/
    TIM_Cmd(TIM2, ENABLE);                                       
}


/**
  * 函    数：TIM2中断函数
  * 参    数：无
  * 返 回 值：无
  * 注意事项：此函数为中断函数，无需调用，中断触发后自动执行
  *           函数名为启动文件的默认名，也可以到启动文件修改
  */
// void TIM2_IRQHandler(void)                                          // 不是 TIM2_UP_IRQHandler
// {
//     if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)                
//     {
//         // 在这里添加定时任务代码

//         TIM_ClearITPendingBit(TIM2, TIM_IT_Update);                 //  ← 改这里
//     }
// }
