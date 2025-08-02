/**
 ****************************************************************************************
 *
 * @file calendar_display.c
 *
 * @brief Calendar display functionality for e-paper display
 *
 ****************************************************************************************
 */

#include "calendar_display.h"
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
 * @brief 绘制日历网格
 */
static void draw_calendar_grid(void)
{
    uint8_t x_start = 10;
    uint8_t y_start = 40;
    uint8_t cell_width = 28;
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
                              y_start + row * cell_height + 1,
                              x_start + (col + 1) * cell_width - 1,
                              y_start + (row + 1) * cell_height - 1,
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
 * @brief 绘制完整的日历页面
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
    draw_calendar_grid();
    
    // 绘制日期数字
    draw_calendar_dates(year, month, current_day);
    
    // 在底部显示当前时间
    char time_buf[20];
    sprintf(time_buf, "%02d:%02d", tm.tm_hour, tm.tm_min);
    EPD_DrawUTF8(150, 115, 1, time_buf, EPD_ASCII_11X16, EPD_FontUTF8_16x16, BLACK, WHITE);
}

