/**
 ****************************************************************************************
 *
 * @file analog_clock.c
 *
 * @brief Analog clock display functionality for e-paper display
 *
 ****************************************************************************************
 */

#include "analog_clock.h"
#include "GUI_Paint.h"
#include "etime.h"
#include "fonts.h"
#include <math.h>

// 数学常量
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @brief 绘制时钟外框和刻度
 * @param center_x 时钟中心X坐标
 * @param center_y 时钟中心Y坐标
 * @param radius 时钟半径
 * @param force_redraw 是否强制重绘所有元素
 */
static void draw_clock_frame(uint16_t center_x, uint16_t center_y, uint16_t radius, bool force_redraw)
{
    if (!force_redraw) return; // 非强制重绘时跳过框架绘制
    
    // 绘制外圆框
    Paint_DrawCircle(center_x, center_y, radius, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    
    // 绘制12个小时数字和刻度点
    for (uint8_t hour = 1; hour <= 12; hour++)
    {
        double angle = (hour * 30 - 90) * M_PI / 180.0; // 12点为0度，顺时针
        
        // 计算数字位置（稍微内缩）
        uint16_t num_x = center_x + (radius - 15) * cos(angle);
        uint16_t num_y = center_y + (radius - 15) * sin(angle);
        
        // 绘制小时数字
        char hour_str[3];
        sprintf(hour_str, "%d", hour);
        
        // 根据数字位置调整显示位置，使数字居中
        uint16_t text_x = num_x - (hour >= 10 ? 7 : 3);
        uint16_t text_y = num_y - 6;
        
        EPD_DrawUTF8(text_x, text_y, 0, hour_str, EPD_ASCII_7X12, 0, BLACK, WHITE);
        
        // 绘制小时刻度点（在数字外侧）
        uint16_t tick_x = center_x + (radius - 8) * cos(angle);
        uint16_t tick_y = center_y + (radius - 8) * sin(angle);
        Paint_DrawPoint(tick_x, tick_y, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
    }
    
    // 绘制分钟刻度点（小点）
    for (uint8_t minute = 0; minute < 60; minute++)
    {
        if (minute % 5 != 0) // 跳过小时刻度位置
        {
            double angle = (minute * 6 - 90) * M_PI / 180.0;
            uint16_t tick_x = center_x + (radius - 5) * cos(angle);
            uint16_t tick_y = center_y + (radius - 5) * sin(angle);
            Paint_DrawPoint(tick_x, tick_y, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
        }
    }
    
    // 绘制中心点
    Paint_DrawPoint(center_x, center_y, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
}

/**
 * @brief 清除指针区域（用于局部刷新前清除旧指针）
 * @param center_x 时钟中心X坐标
 * @param center_y 时钟中心Y坐标
 * @param radius 时钟半径
 */
static void clear_hands_area(uint16_t center_x, uint16_t center_y, uint16_t radius)
{
    // 清除指针可能经过的圆形区域（不包括外框）
    Paint_DrawCircle(center_x, center_y, radius - 10, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
}

/**
 * @brief 绘制时钟指针
 * @param center_x 时钟中心X坐标
 * @param center_y 时钟中心Y坐标
 * @param hour 小时 (0-23)
 * @param minute 分钟 (0-59)
 * @param radius 时钟半径
 */
static void draw_clock_hands(uint16_t center_x, uint16_t center_y, uint8_t hour, uint8_t minute, uint16_t radius)
{
    // 计算指针角度（12点为0度，顺时针为正）
    double hour_angle = ((hour % 12) * 30 + minute * 0.5 - 90) * M_PI / 180.0;
    double minute_angle = (minute * 6 - 90) * M_PI / 180.0;
    
    // 指针长度
    uint16_t hour_hand_length = radius * 0.5;   // 时针长度为半径的50%
    uint16_t minute_hand_length = radius * 0.7; // 分针长度为半径的70%
    
    // 计算时针终点
    uint16_t hour_end_x = center_x + hour_hand_length * cos(hour_angle);
    uint16_t hour_end_y = center_y + hour_hand_length * sin(hour_angle);
    
    // 计算分针终点
    uint16_t minute_end_x = center_x + minute_hand_length * cos(minute_angle);
    uint16_t minute_end_y = center_y + minute_hand_length * sin(minute_angle);
    
    // 绘制时针（较粗）
    Paint_DrawLine(center_x, center_y, hour_end_x, hour_end_y, BLACK, DOT_PIXEL_3X3, LINE_STYLE_SOLID);
    
    // 绘制分针（较细）
    Paint_DrawLine(center_x, center_y, minute_end_x, minute_end_y, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    
    // 重新绘制中心点（确保在指针之上）
    Paint_DrawPoint(center_x, center_y, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
}

/**
 * @brief 绘制模拟时钟
 * @param x 时钟左上角X坐标
 * @param y 时钟左上角Y坐标
 * @param size 时钟正方形边长
 * @param unix_time 当前Unix时间戳
 * @param force_redraw 是否强制重绘所有元素（true：完全重绘，false：仅更新指针）
 */
void draw_analog_clock(uint16_t x, uint16_t y, uint16_t size, uint32_t unix_time, bool force_redraw)
{
    tm_t tm;
    transformTime(unix_time, &tm);
    
    // 计算时钟中心和半径
    uint16_t center_x = x + size / 2;
    uint16_t center_y = y + size / 2;
    uint16_t radius = size / 2 - 2; // 留出边距
    
    if (force_redraw)
    {
        // 强制重绘：清空整个时钟区域
        Paint_DrawRectangle(x, y, x + size, y + size, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
        
        // 绘制时钟框架和数字
        draw_clock_frame(center_x, center_y, radius, true);
    }
    else
    {
        // 局部刷新：仅清除指针区域
        clear_hands_area(center_x, center_y, radius);
    }
    
    // 绘制指针
    draw_clock_hands(center_x, center_y, tm.tm_hour, tm.tm_min, radius);
}

/**
 * @brief 绘制带模拟时钟的日历页面
 * @param unix_time 当前Unix时间戳
 * @param force_redraw 是否强制重绘时钟
 */
void draw_calendar_with_analog_clock(uint32_t unix_time, bool force_redraw)
{
    tm_t tm;
    transformTime(unix_time, &tm);
    
    uint16_t year = tm.tm_year + YEAR0;
    uint8_t month = tm.tm_mon + 1;
    uint8_t current_day = tm.tm_mday;
    
    if (force_redraw)
    {
        // 完全重绘时清空画布
        Paint_Clear(WHITE);
        
        // 绘制日历标题
        char title_buf[20];
        sprintf(title_buf, "%d年%d月", year, month);
        EPD_DrawUTF8(50, 2, 1, title_buf, EPD_ASCII_11X16, EPD_FontUTF8_16x16, BLACK, WHITE);
        
        // 绘制星期标题行
        const char* week_names_cn[] = {"日", "一", "二", "三", "四", "五", "六"};
        uint8_t x_start = 10;
        uint8_t y_pos = 25;
        uint8_t cell_width = 28;
        
        for (uint8_t i = 0; i < 7; i++)
        {
            uint8_t x_pos = x_start + i * cell_width;
            EPD_DrawUTF8(x_pos + 8, y_pos, 0, week_names_cn[i], EPD_ASCII_7X12, EPD_FontUTF8_16x16, BLACK, WHITE);
        }
        
        // 绘制日历网格
        uint8_t y_start = 40;
        uint8_t cell_height = 12;
        uint8_t grid_width = 7 * cell_width;
        uint8_t grid_height = 6 * cell_height;
        
        // 绘制水平线
        for (uint8_t i = 0; i <= 6; i++)
        {
            uint8_t y = y_start + i * cell_height;
            Paint_DrawLine(x_start, y, x_start + grid_width, y, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
        }
        
        // 绘制垂直线
        for (uint8_t i = 0; i <= 7; i++)
        {
            uint8_t x = x_start + i * cell_width;
            Paint_DrawLine(x, y_start, x, y_start + grid_height, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
        }
        
        // 绘制日期数字
        const uint8_t days_in_month[2][12] = {
            {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}, // 非闰年
            {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}  // 闰年
        };
        
        uint8_t leap = is_leap(year);
        uint8_t first_day = day_of_week_get(month, 1, year);
        uint8_t days_count = days_in_month[leap][month - 1];
        
        char day_buf[3];
        uint8_t row = 0;
        uint8_t col = first_day;
        
        for (uint8_t day = 1; day <= days_count; day++)
        {
            uint8_t x_pos = x_start + col * cell_width + 8;
            uint8_t y_pos = y_start + row * cell_height + 2;
            
            sprintf(day_buf, "%d", day);
            
            if (day == current_day)
            {
                // 当前日期反色显示
                Paint_DrawRectangle(x_start + col * cell_width + 1, 
                                  y_start + row * cell_height + 1,
                                  x_start + (col + 1) * cell_width - 1,
                                  y_start + (row + 1) * cell_height - 1,
                                  BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
                EPD_DrawUTF8(x_pos, y_pos, 0, day_buf, EPD_ASCII_7X12, 0, WHITE, BLACK);
            }
            else
            {
                EPD_DrawUTF8(x_pos, y_pos, 0, day_buf, EPD_ASCII_7X12, 0, BLACK, WHITE);
            }
            
            col++;
            if (col >= 7)
            {
                col = 0;
                row++;
            }
        }
    }
    
    // 在右下角绘制模拟时钟（120x120像素）
    uint16_t clock_x = 170; // 适合290宽度屏幕的位置
    uint16_t clock_y = 12;  // 垂直居中位置
    uint16_t clock_size = 120;
    
    draw_analog_clock(clock_x, clock_y, clock_size, unix_time, force_redraw);
}

