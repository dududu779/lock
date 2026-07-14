#include "Menu_Display.h"
#include "OLED.h"
#include <stddef.h>     


/* ==================== 静态驱动实例 ==================== */
// 静态指针：保存当前注册的显示驱动实例
// static = 仅本文件可见，防止外部直接修改
// 初始值为 NULL，表示尚未注册任何驱动
static const MenuDisplay_Driver_t* s_driver = NULL;

/* ==================== 注册函数 ==================== */
// 向菜单系统注册一个显示驱动
// driver: 指向驱动结构体的指针（如 &OLED_MenuDriver 或 OLED_MenuDriver）
// 调用一次即可，通常在 main() 初始化时完成
void MenuDisplay_Register(const MenuDisplay_Driver_t* driver)
{
    s_driver = driver;  // 保存指针，后续菜单系统通过它调用显示功能
}

// 获取当前已注册的显示驱动
// 返回: 驱动指针，如果未注册则返回 NULL
// 菜单系统内部使用，不直接暴露给业务代码
const MenuDisplay_Driver_t* MenuDisplay_GetDriver(void)
{
    return s_driver;  // 返回保存的指针
}
/* ==================== 接入驱动函数 ==================== */
void OLED_Adapter_Init(void){OLED_Init();}

void OLED_Adapter_Clear(void) { OLED_Clear(); }

void OLED_Adapter_DrawString(uint8_t line, const char* str){OLED_ShowString(0, line * OLED_LINE_HEIGHT, (char*)str, OLED_FONT_SIZE);}

void OLED_Adapter_Highlight(uint8_t line){OLED_ReverseArea(0, line * OLED_LINE_HEIGHT, OLED_SCREEN_WIDTH, OLED_LINE_HEIGHT);}

void OLED_Adapter_Refresh(void) {OLED_Update();}

uint8_t OLED_Adapter_GetMaxLines(void) { return OLED_MAX_LINES; }

const MenuDisplay_Driver_t OLED_MenuDriver = {
    .Init        = OLED_Adapter_Init,
    .Clear       = OLED_Adapter_Clear,
    .DrawString  = OLED_Adapter_DrawString,
    .Highlight   = OLED_Adapter_Highlight,
    .Refresh     = OLED_Adapter_Refresh,
    .GetMaxLines = OLED_Adapter_GetMaxLines
};
