/**
 ****************************************************************************************
 *
 * @file calendar_display_modified.h
 *
 * @brief Modified calendar display with analog clock functionality header file
 *
 ****************************************************************************************
 */

#ifndef __CALENDAR_DISPLAY_MODIFIED_H
#define __CALENDAR_DISPLAY_MODIFIED_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief 绘制完整的日历页面（原版本，保持兼容性）
 * @param unix_time 当前Unix时间戳
 */
void draw_calendar_page(uint32_t unix_time);

/**
 * @brief 绘制带模拟时钟的日历页面（新版本）
 * @param unix_time 当前Unix时间戳
 * @param force_redraw 是否强制重绘时钟
 */
void draw_calendar_page_with_analog_clock(uint32_t unix_time, bool force_redraw);

#endif // __CALENDAR_DISPLAY_MODIFIED_H

