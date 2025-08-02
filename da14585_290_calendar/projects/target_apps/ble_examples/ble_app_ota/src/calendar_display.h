/**
 ****************************************************************************************
 *
 * @file calendar_display.h
 *
 * @brief Calendar display functionality header file
 *
 ****************************************************************************************
 */

#ifndef __CALENDAR_DISPLAY_H
#define __CALENDAR_DISPLAY_H

#include <stdint.h>

/**
 * @brief 绘制完整的日历页面
 * @param unix_time 当前Unix时间戳
 */
void draw_calendar_page(uint32_t unix_time);

#endif // __CALENDAR_DISPLAY_H

