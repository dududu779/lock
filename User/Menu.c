#include "stm32f10x.h"
#include <stddef.h>
#include "Menu.h"
#include "APP.h"
#include "Menu_Display.h"   // 只依赖抽象接口，不依赖 OLED.h

/* ==================== 全局菜单表 ==================== */
const Menustruct_t All_Menu[] = 
{
    [PWD_UNLOCK]   = {"密码解锁",   MENU_FUNC, Func_PwdUnlock,  NO_PARENT, OPEN_DOOR, PWD_UNLOCK_SUBMENU_COUNT},  
    [CARD_UNLOCK]  = {"刷卡解锁",   MENU_FUNC, NULL,            NO_PARENT, 0, 0},
    [FP_UNLOCK]    = {"指纹解锁",   MENU_FUNC, NULL,            NO_PARENT, 0, 0},

    [OPEN_DOOR]    = {"开门",       MENU_FUNC, Func_OpenDoor,   PWD_UNLOCK, NO_SUBMENU, NO_SUBMENU},
    [CHANGE_PWD]   = {"修改密码",   MENU_FUNC, Func_ChangePwd,  PWD_UNLOCK, NO_SUBMENU, NO_SUBMENU},
    [SETTING]      = {"设置",       MENU_ROOT, Func_Setting,    PWD_UNLOCK, VOLUME, SETTING_SUBMENU_COUNT},  
    [ABOUT]        = {"关于本机",   MENU_FUNC, Func_About,      PWD_UNLOCK, NO_SUBMENU, NO_SUBMENU},

    [VOLUME]       = {"音量大小",     MENU_FUNC, Func_Volume,      SETTING, NO_SUBMENU, NO_SUBMENU},
    [UNLOCK_SOUND] = {"开锁提示音",   MENU_FUNC, Func_UnlockSound, SETTING, NO_SUBMENU, NO_SUBMENU},
    [ERR_LIMIT]    = {"错误次数限制", MENU_FUNC, Func_ErrLimit,    SETTING, NO_SUBMENU, NO_SUBMENU},
    [ADD_CARD]     = {"添加卡片",     MENU_FUNC, Func_AddCard,     SETTING, NO_SUBMENU, NO_SUBMENU},
    [DEL_CARD]     = {"删除卡片",     MENU_FUNC, Func_DelCard,     SETTING, NO_SUBMENU, NO_SUBMENU},
    [ADD_FP]       = {"添加指纹",     MENU_FUNC, Func_AddFp,       SETTING, NO_SUBMENU, NO_SUBMENU}, 
    [DEL_FP]       = {"删除指纹",     MENU_FUNC, Func_DelFp,       SETTING, NO_SUBMENU, NO_SUBMENU},
};

uint8_t Current_Parent = NO_PARENT;// 当前父菜单索引
uint8_t Current_Index = PWD_UNLOCK;// 当前选中菜单索引（全局索引）
uint8_t Current_Offset = 0;// 当前选中菜单在屏幕上的行位置（窗口内索引）
uint8_t Current_Base = 0;   // 当前窗口在父菜单下的偏移量（窗口起始位置在父菜单下的索引）


// 获取父菜单下的子菜单总数
uint8_t Menu_GetSubCount(uint8_t parent_id)
{
    if(parent_id == NO_PARENT)
    {
        uint8_t count = 0;
        for(uint8_t i = 0; i < MENU_TOTAL_COUNT; i++)
        {
            if(All_Menu[i].parent_id == NO_PARENT)
                count++;
        }
        return count;
    }
    else
    {
        return All_Menu[parent_id].Sub_cnt;
    }
}

// 获取指定父菜单下第 n 个子菜单的全局索引
uint8_t Menu_GetSubIndex(uint8_t parent_id, uint8_t n)
{
    if(parent_id == NO_PARENT)
    {
        uint8_t count = 0;
        for(uint8_t i = 0; i < MENU_TOTAL_COUNT; i++)
        {
            if(All_Menu[i].parent_id == NO_PARENT)
            {
                if(count == n)
                    return i;
                count++;
            }
        }
    }
    else
    {
        if(n < All_Menu[parent_id].Sub_cnt)
            return All_Menu[parent_id].Sub_start + n;
    }
    return Current_Index;
}

/* ==================== Menu_Display 使用抽象接口 ==================== */
// 显示指定父菜单下的子菜单
void Menu_Display(uint8_t parent)
{
    const MenuDisplay_Driver_t* drv = MenuDisplay_GetDriver();
    if(drv == NULL) return;  // 未注册驱动，不显示
    
    uint8_t Sub_Cnt = Menu_GetSubCount(parent);
    uint8_t Sub_First = Menu_GetSubIndex(parent, 0);
    uint8_t max_lines = drv->GetMaxLines();  // 动态获取屏幕容量
    
    if(Sub_Cnt == 0) return;
    
    drv->Clear();  // 清屏
    
    // 从 Current_Base 开始显示，最多显示 max_lines 行
    for(uint8_t i = 0; i < max_lines; i++)
    {
        uint8_t real_index = Current_Base + i;
        if(real_index < Sub_Cnt)
        {
            uint8_t menu_id = Sub_First + real_index;
            drv->DrawString(i, All_Menu[menu_id].name);  // 在第 i 行显示
        }
    }
    
    // 高亮当前选中项（屏幕上的行位置）
    drv->Highlight(Current_Offset);
    drv->Refresh();  // 刷新
}

/* ==================== 导航函数 ==================== */
void Menu_Up(void)
{
    // 获取显示最大行数，否则默认为4行
    const MenuDisplay_Driver_t* drv = MenuDisplay_GetDriver();
    uint8_t max_lines;
    if (drv != NULL) {
        max_lines = drv->GetMaxLines();
    } else {
        max_lines = 4;
    }

    // 获取当前父菜单下的子菜单项总数
    uint8_t Sub_Cnt = Menu_GetSubCount(Current_Parent);
    
    // 空菜单，直接返回
    if(Sub_Cnt == 0) return;
    
    //选中项已在窗口最顶部（Current_Offset == 0）
    if(Current_Offset == 0)
    {
        //窗口还可以继续向上滚动（Offset > 0）,则窗口整体上移一行，保持选中在顶部
        if(Current_Base > 0)
        {
            Current_Base--;  // 窗口上移
        }
        //否则窗口已经到顶（Offset == 0），需要循环回到底部
        else
        {
            // 菜单项超过屏幕容量：窗口跳到最后，选中最后一项
            if(Sub_Cnt > max_lines)
            {
                Current_Base = Sub_Cnt - max_lines;  // 窗口定位到最后可见区域
                Current_Offset = max_lines - 1;     // 选中窗口最后一行
            }
            // 菜单项不超过屏幕容量：直接选中最后一项
            else
            {
                Current_Offset = Sub_Cnt - 1;  // 选中最后一项（无需滚动窗口）
            }
        }
    }
    //选中项不在窗口顶部，直接上移选中
    else
    {
        Current_Offset--;  // 选中上移一行
    }
    
    // 刷新显示
    Menu_Display(Current_Parent);
}

void Menu_Down(void)
{
    // 获取显示最大行数，否则默认为4行
    const MenuDisplay_Driver_t* drv = MenuDisplay_GetDriver();
    uint8_t max_lines;
    if (drv != NULL) {
        max_lines = drv->GetMaxLines();
    } else {
        max_lines = 4;
    }
    
    // 获取子菜单总数
    uint8_t Sub_Cnt = Menu_GetSubCount(Current_Parent);
    
    if(Sub_Cnt == 0) return;

    
    Current_Offset++;//先尝试下移选中项 
    
    // 选中超出窗口底部，需要滚动窗口
    if(Current_Offset >= max_lines)
    {
        // 将选中固定在窗口底部
        Current_Offset = max_lines - 1;
        // 窗口向下滚动
        Current_Base++;
        
        if(Current_Base + max_lines > Sub_Cnt)
        {
            if (Sub_Cnt > max_lines)// 如果菜单项超过屏幕一页的容量
            {
                Current_Base = Sub_Cnt - max_lines;//回到窗口能偏移的最大值
            } 
            else 
            {
                Current_Base = 0;// 如果菜单项不超过屏幕容量，窗口偏移量为0
            }
        }
    }
    

    if(Current_Base + Current_Offset >= Sub_Cnt)//如果选中项超出菜单总数，则回到顶部
    {
        Current_Offset = 0;
        Current_Base = 0;
    }
    
    Menu_Display(Current_Parent);
}

void Menu_Enter(void)
{
    // 计算实际选中的菜单项在全局数组中的索引
    // 在当前父菜单的子菜单列表中的位置序号（类似内存地址的基地址和偏移量）
    uint8_t real_index = Current_Base + Current_Offset;
    
    // 通过父菜单和实际索引，获取选中项的全局ID
    uint8_t real_id = Menu_GetSubIndex(Current_Parent, real_index);
    
    // ========== 进入子菜单 ==========
    if(All_Menu[real_id].Type == MENU_ROOT)//如果选中项类型是子菜单，则进入该子菜单
    {
        // 切换当前父菜单为选中的子菜单
        Current_Parent = real_id;
        // 重置选中状态和窗口
        Current_Offset = 0;
        Current_Base = 0;
        // 显示新菜单
        Menu_Display(Current_Parent);
    }
    // ========== 执行功能函数 ==========
    else if(All_Menu[real_id].Type == MENU_FUNC)
    {
        // 检查函数指针有效
        if(All_Menu[real_id].Func != NULL)
        {
            // 执行功能回调
            All_Menu[real_id].Func();
            // 功能执行完毕后，返回当前菜单显示

        //  Menu_Display(Current_Parent);
        }
    }
}

void Menu_Back(void)
{
    // ========== 检查是否有上级菜单 ==========
    // NO_PARENT 表示当前已在根菜单，无法再返回
    if(Current_Parent != NO_PARENT)
    {
        // 保存当前菜单ID（即将成为"旧子菜单"）
        uint8_t old_parent = Current_Parent;
        
        // 切换到上级菜单
        // All_Menu[old_parent].parent_id 存储了当前菜单的父节点ID
        Current_Parent = All_Menu[old_parent].parent_id;
        
        // ========== 恢复上次选中位置 ==========
        // 遍历上级菜单的所有子项，找到刚才那个子菜单的位置
        for(uint8_t i = 0; i < Menu_GetSubCount(Current_Parent); i++)
        {
            // Menu_GetSubIndex(parent, index) 获取父菜单第i个子项的全局ID
            if(Menu_GetSubIndex(Current_Parent, i) == old_parent)
            {
                // 找到旧菜单在新父菜单中的位置 i
                
                // 获取屏幕显示行数
                const MenuDisplay_Driver_t* drv = MenuDisplay_GetDriver();
                uint8_t max_lines = (drv != NULL) ? drv->GetMaxLines() : 4;
                
                // ========== 计算窗口位置和选中项 ==========
                if(i < max_lines)
                {
                    // 旧菜单在前 max_lines 项内，无需滚动
                    // 窗口从顶部开始
                    Current_Base = 0;
                    // 选中第 i 项
                    Current_Offset = i;
                }
                else
                {
                    // 旧菜单在屏幕下方，需要滚动窗口使其可见
                    // 窗口起始位置：让旧菜单出现在窗口最底部
                    Current_Base = i - max_lines + 1;
                    // 选中窗口最后一行
                    Current_Offset = max_lines - 1;
                }
                break;  // 找到即退出循环
            }
        }
        
        // 显示上级菜单，光标定位在刚才进入的那个子菜单上
        Menu_Display(Current_Parent);
    }
    // else: 已在根菜单，无操作（或可做提示音等）
}



