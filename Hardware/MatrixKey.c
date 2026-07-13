#include "stm32f10x.h"
#include "Delay.h"
#include "MatrixKey.h"
#include <stddef.h>

// 引脚配置
//=============================================================================
// 行端口
GPIO_TypeDef* const ROW_PORTS[ROW] = {GPIOB, GPIOB, GPIOB, GPIOA};
// 列端口  
GPIO_TypeDef* const COL_PORTS[COL] = {GPIOA, GPIOB, GPIOA};

// 行引脚
const uint16_t ROW_PINS[ROW] = {GPIO_Pin_0, GPIO_Pin_4, GPIO_Pin_3, GPIO_Pin_12};
// 列引脚
const uint16_t COL_PINS[COL] = {GPIO_Pin_11, GPIO_Pin_1, GPIO_Pin_15};

// 按键映射表
//=============================================================================
const uint8_t Key_Map[ROW][COL] = {
    {Key_1,     Key_2,     Key_3    },
    {Key_4,     Key_5,     Key_6    },
    {Key_7,     Key_8,     Key_9    },
    {Key_STAR,  Key_0,     Key_HASH }
};

uint8_t Key_EvenFlag[Key_COUNT] = {0};

//矩阵键盘初始化
void Key_Init(void)
{   
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_12;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//获取指定按键状态
static uint8_t Key_GetState(uint8_t n)
{
       for(uint8_t row = 0; row < ROW; row++)
    {
        for (uint8_t i = 0; i < ROW; i++)GPIO_SetBits(ROW_PORTS[i], ROW_PINS[i]);

        // 拉低指定行
         GPIO_ResetBits(ROW_PORTS[row], ROW_PINS[row]);
        
         // 等待电平稳定
        for(int i = 0; i < 10; i++) { __NOP(); }

        for(uint8_t col = 0; col < COL; col++)
        {
            if(Key_Map[row][col] == n)  // 查找按键值对应的坐标
            {
                // 检测这个坐标
                if(GPIO_ReadInputDataBit(COL_PORTS[col], COL_PINS[col]) == 0 &&
                   GPIO_ReadInputDataBit(ROW_PORTS[row], ROW_PINS[row]) == 0)
                {
                    return KEY_PRESSED;
                }
            }
        }
    }
    return KEY_UNPRESSED;
}

//获取指定按键的指定标志位
uint8_t Key_Check(uint8_t n,uint8_t Flag)
{
    if (Key_EvenFlag[n] & Flag)//指定位是否为1
    {
        if(Flag != HOLD)//传入标志位为非HOLD
        {
            Key_EvenFlag[n] &= ~Flag;  //检查完对应标志位之后将标志位清零
        }
        return 1;
    }
    return 0;
}

//按键事件状态机，由定时器调用
void Key_EvenTick(void)
{
    
       // 静态变量：保持上次调用后的值
    static uint8_t Count;                    // 计数器，用于20ms定时
    static uint8_t i;                        // 循环变量
    static uint8_t CurrState[Key_COUNT];     // 当前按键状态（按下/释放）
    static uint8_t PrevState[Key_COUNT];     // 上一次按键状态
    static uint8_t S[Key_COUNT];             // 状态机状态（0-4）
    static uint16_t Time[Key_COUNT];         // 定时器数组（单位：ms）
    static uint16_t PowerOnCount = 0;        // 上电计数
    /* ========== 第一部分：递减所有按键的定时器 ========== */
    // 遍历所有按键，将定时器值减到0为止
    for (i = 0; i < Key_COUNT; i ++)
    {
        if (Time[i] > 0)
        {
            Time[i] --;    // 定时器减1
        }
    }
    
    /* ========== 第二部分：20ms定时扫描 ========== */
    Count ++;               // 计数累加（每1ms调用一次）
    if (Count >= 20)        // 达到20ms，执行按键扫描
    {
        Count = 0;          // 重置计数器
        if (PowerOnCount < 20)
        {
            PowerOnCount++;
            for (i = 0; i < Key_COUNT; i++)
            {
                PrevState[i] = KEY_UNPRESSED;
                CurrState[i] = KEY_UNPRESSED;
                S[i] = 0;
                Time[i] = 0;
                Key_EvenFlag[i] = 0;  // 清除所有标志
            }
            return;
        }
        /* ========== 第三部分：扫描所有按键 ========== */
        for (i = 0; i < Key_COUNT; i ++)
        {
            /* ----- 3.1 更新按键状态 ----- */
            PrevState[i] = CurrState[i];                    // 保存上次状态
            CurrState[i] = Key_GetState(i);                 // 读取当前状态
            
            /* ----- 3.2 设置/清除 HOLD 标志（持续按下标志）----- */
            // HOLD标志：表示按键当前处于按下状态
            if (CurrState[i] == KEY_PRESSED)                // 如果按键按下
            {
                Key_EvenFlag[i] |= HOLD;                    // 设置HOLD标志
            }
            else                                             // 如果按键释放
            {
                Key_EvenFlag[i] &= ~HOLD;                   // 清除HOLD标志
            }
            
            /* ----- 3.3 检测 DOWN 事件（下降沿：按下瞬间）----- */
            // 当前按下 且 之前未按下 → 刚按下的瞬间
            if (CurrState[i] == KEY_PRESSED && PrevState[i] == KEY_UNPRESSED)
            {
                Key_EvenFlag[i] |= DOWN;                    // 设置DOWN标志
            }
            
            /* ----- 3.4 检测 UP 事件（上升沿：释放瞬间）----- */
            // 当前释放 且 之前按下 → 刚释放的瞬间
            if (CurrState[i] == KEY_UNPRESSED && PrevState[i] == KEY_PRESSED)
            {
                Key_EvenFlag[i] |= UP;                      // 设置UP标志
            }
            
            /* ========== 第四部分：状态机（实现单击/双击/长按检测）========== */
            
            /* ----- 状态0：空闲状态（等待按键按下）----- */
            if (S[i] == 0)
            {
                // 检测到按键按下
                if (CurrState[i] == KEY_PRESSED)
                {
                    Time[i] = KEY_TIME_LONG;    // 设置长按定时器
                    S[i] = 1;                   // 进入状态1：等待释放或长按
                }
            }
            
            /* ----- 状态1：等待释放或长按 ----- */
            else if (S[i] == 1)
            {
                // 情况1：按键释放 → 可能是单击或双击的开始
                if (CurrState[i] == KEY_UNPRESSED)
                {
                    Time[i] = KEY_TIME_DOUBLE;  // 设置双击等待定时器
                    S[i] = 2;                   // 进入状态2：等待第二次按下
                }
                // 情况2：长按定时器超时 → 触发长按事件
                else if (Time[i] == 0)
                {
                    Time[i] = KEY_TIME_REPEAT;  // 设置重复触发定时器
                    Key_EvenFlag[i] |= LONG;    // 设置长按标志
                    S[i] = 4;                   // 进入状态4：长按重复触发状态
                }
            }
            
            /* ----- 状态2：等待第二次按下（双击检测）----- */
            else if (S[i] == 2)
            {
                // 情况1：再次按下 → 触发双击事件
                if (CurrState[i] == KEY_PRESSED)
                {
                    Key_EvenFlag[i] |= DOUBLE;  // 设置双击标志
                    S[i] = 3;                   // 进入状态3：等待释放
                }
                // 情况2：定时器超时 → 没有第二次按下，触发单击事件
                else if (Time[i] == 0)
                {
                    Key_EvenFlag[i] |= SINGLE;  // 设置单击标志
                    S[i] = 0;                   // 返回空闲状态
                }
            }
            
            /* ----- 状态3：等待释放（双击后的等待）----- */
            else if (S[i] == 3)
            {
                // 等待按键释放后返回空闲状态
                if (CurrState[i] == KEY_UNPRESSED)
                {
                    S[i] = 0;                   // 返回空闲状态
                }
            }
            
            /* ----- 状态4：长按重复触发状态 ----- */
            else if (S[i] == 4)
            {
                // 情况1：按键释放 → 结束长按
                if (CurrState[i] == KEY_UNPRESSED)
                {
                    S[i] = 0;                   // 返回空闲状态
                }
                // 情况2：重复触发定时器超时 → 产生重复触发事件
                else if (Time[i] == 0)
                {
                    Time[i] = KEY_TIME_REPEAT;  // 重置重复定时器
                    Key_EvenFlag[i] |= REPEAT;  // 设置重复触发标志
                    S[i] = 4;                   // 保持在状态4，继续重复
                }
            }
        } 
    } 
}
