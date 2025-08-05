/**
 ****************************************************************************************
 *
 * @file analog_clock.h
 *
 * @brief Analog clock display functionality header file
 *
 ****************************************************************************************
 */

#ifndef __ANALOG_CLOCK_H
#define __ANALOG_CLOCK_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief 绘制模拟时钟
 * @param x 时钟左上角X坐标
 * @param y 时钟左上角Y坐标
 * @param size 时钟正方形边长
 * @param unix_time 当前Unix时间戳
 * @param force_redraw 是否强制重绘所有元素（true：完全重绘，false：仅更新指针）
 */
void draw_analog_clock(uint16_t x, uint16_t y, uint16_t size, uint32_t unix_time, bool force_redraw);

/**
 * @brief 绘制带模拟时钟的日历页面
 * @param unix_time 当前Unix时间戳
 * @param force_redraw 是否强制重绘时钟
 */
void draw_calendar_with_analog_clock(uint32_t unix_time, bool force_redraw);

#endif // __ANALOG_CLOCK_H

