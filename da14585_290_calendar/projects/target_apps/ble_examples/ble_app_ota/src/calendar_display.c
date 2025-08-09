/**
 ****************************************************************************************
 *
 * @file calendar_display_modified.c
 *
 * @brief Modified calendar display with analog clock functionality
 *
 ****************************************************************************************
 */

#include "calendar_display.h"
#include "analog_clock.h"
#include "GUI_Paint.h"
#include "etime.h"
#include "fonts.h"

// 月份天数表（非闰年和闰年）
static const uint8_t days_in_month[2][12] = {
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}, // 非闰年
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}  // 闰年
};

// 星期名称
static const char* week_names_cn[] = {"日", "一", "二", "三", "四", "五", "六"};

/**
 * @brief 获取某月第一天是星期几
 * @param year 年份
 * @param month 月份 (1-12)
 * @return 星期几 (0=星期日, 1=星期一, ..., 6=星期六)
 */
static uint8_t get_first_day_of_month(uint16_t year, uint8_t month)
{
    return day_of_week_get(month, 1, year);
}

/**
 * @brief 获取某月的天数
 * @param year 年份
 * @param month 月份 (1-12)
 * @return 该月的天数
 */
static uint8_t get_days_in_month(uint16_t year, uint8_t month)
{
    uint8_t leap = is_leap(year);
    return days_in_month[leap][month - 1];
}

/**
 * @brief 绘制日历标题（年月）
 * @param year 年份
 * @param month 月份
 */
static void draw_calendar_title(uint16_t year, uint8_t month)
{
    char title_buf[20];
    sprintf(title_buf, "%d年%d月", year, month);
    
    // 在屏幕顶部居中显示标题
    EPD_DrawUTF8(50, 2, 1, title_buf, EPD_ASCII_11X16, EPD_FontUTF8_16x16, BLACK, WHITE);
}

/**
 * @brief 绘制星期标题行
 */
static void draw_week_header(void)
{
    uint8_t x_start = 10;
    uint8_t y_pos = 25;
    uint8_t cell_width = 28;
    
    for (uint8_t i = 0; i < 7; i++)
    {
        uint8_t x_pos = x_start + i * cell_width;
        EPD_DrawUTF8(x_pos + 8, y_pos, 0, week_names_cn[i], EPD_ASCII_7X12, EPD_FontUTF8_16x16, BLACK, WHITE);
    }
}
 
/**
 * @brief 绘制日期数字
 * @param year 年份
 * @param month 月份
 * @param current_day 当前日期（用于高亮显示）
 */
static void draw_calendar_dates(uint16_t year, uint8_t month, uint8_t current_day)
{
    uint8_t x_start = 10;
    uint8_t y_start = 40;
    uint8_t cell_width = 28;
    uint8_t cell_height = 12;
    
    uint8_t first_day = get_first_day_of_month(year, month);
    uint8_t days_count = get_days_in_month(year, month);
    
    char day_buf[3];
    uint8_t row = 0;
    uint8_t col = first_day; // 从第一天对应的星期开始
    
    for (uint8_t day = 1; day <= days_count; day++)
    {
        uint8_t x_pos = x_start + col * cell_width + 8;
        uint8_t y_pos = y_start + row * cell_height + 2;
        
        sprintf(day_buf, "%d", day);
        
        // 如果是当前日期，使用反色显示
        if (day == current_day)
        {
            // 绘制背景矩形
            Paint_DrawRectangle(x_start + col * cell_width + 1, 
                              y_start + row * cell_height + 3,
                              x_start + (col + 1) * cell_width - 3,
                              y_start + (row + 1) * cell_height+ 3,
                              BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
            
            // 白色字体
            EPD_DrawUTF8(x_pos, y_pos, 0, day_buf, EPD_ASCII_7X12, 0, WHITE, BLACK);
        }
        else
        {
            // 普通黑色字体
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

/**
 * @brief 绘制完整的日历页面（原版本，保持兼容性）
 * @param unix_time 当前Unix时间戳
 */
void draw_calendar_page(uint32_t unix_time)
{
    tm_t tm;
    transformTime(unix_time, &tm);
    
    uint16_t year = tm.tm_year + YEAR0;
    uint8_t month = tm.tm_mon + 1;
    uint8_t current_day = tm.tm_mday;
    
    // 清空画布
    Paint_Clear(WHITE);
    
    // 绘制日历标题
    draw_calendar_title(year, month);
    
    // 绘制星期标题行
    draw_week_header();
    
    // 绘制日历网格
    //draw_calendar_grid();
    
    // 绘制日期数字
    draw_calendar_dates(year, month, current_day);
    
    // 在底部显示当前时间（数字格式）
    char time_buf[20];
    sprintf(time_buf, "%02d:%02d", tm.tm_hour, tm.tm_min);
    EPD_DrawUTF8(150, 110, 1, time_buf, EPD_ASCII_11X16, EPD_FontUTF8_16x16, BLACK, WHITE);
}

/**
 * @brief 绘制带模拟时钟的日历页面（新版本）
 * @param unix_time 当前Unix时间戳
 * @param force_redraw 是否强制重绘时钟
 */
void draw_calendar_page_with_analog_clock(uint32_t unix_time, bool force_redraw)
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
        draw_calendar_title(year, month);
        
        // 绘制星期标题行
        draw_week_header();
        
        // 绘制日历网格
        //draw_calendar_grid();
        
        // 绘制日期数字
        draw_calendar_dates(year, month, current_day);
    }
    
    // 在右侧绘制模拟时钟（120x120像素）
    // 根据屏幕尺寸调整位置：290宽度屏幕
    uint16_t clock_x = 170; // 右侧位置
    uint16_t clock_y = 12;  // 垂直居中
    uint16_t clock_size = 120;
    
    draw_analog_clock(clock_x, clock_y, clock_size, unix_time, force_redraw);
}

