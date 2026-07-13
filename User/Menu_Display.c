#include "Menu_Display.h"
#include <stddef.h>     


/* ==================== 静态驱动实例 ==================== */
static const MenuDisplay_Driver_t* s_driver = NULL;

/* ==================== 注册函数 ==================== */
void MenuDisplay_Register(const MenuDisplay_Driver_t* driver)
{
    s_driver = driver;
}

const MenuDisplay_Driver_t* MenuDisplay_GetDriver(void)
{
    return s_driver;
}
