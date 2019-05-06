#ifndef _LEDMATRIX_H_
#define _LEDMATRIX_H_
#include "board.h"

#define OE_LED_DIS       GPIOA->BSRR = GPIO_PIN_0;
#define OE_LED_EN        GPIOA->BSRR = (GPIO_PIN_0 << 16);

#define LINE_1 16
#define LINE_2 0

#define MOVE_BUFF 22

#define LED_BUFF_SIZE   768

//ºì ÂÌ À¶ »Æ ×Ï Çà °×
typedef enum
{
    RED = 0,
    GREEN,
    BLUE,
    YELLOW,
    PURPLE,
    CYAN,
    WHITE,
    BLACK
}ColorDef;

#pragma anon_unions
/*
   äºŒç»´æ•°ç»„æŒ‡é’ˆ*(*(a+i)+j)
 */
typedef struct _dat_rgb_t
{
    union
    {
        uint8_t r[32][8];
        uint32_t r32[32][2];
    };

    union
    {
        uint8_t g[32][8];
        uint32_t g32[32][2];
    };

    union
    {
        uint8_t b[32][8];
        uint32_t b32[32][2];
    };
    //2000Äê01ÔÂ01ÈÕ ÐÇÆÚÈý       //21 24 /8 3
    uint8_t move_buf[16][MOVE_BUFF];  //display buff for move
    ColorDef color;    // 0~6 ºì ÂÌ À¶ »Æ ×Ï Çà °×
}dat_rgb_t;
extern dat_rgb_t dat;

typedef struct _disp_time_t
{
    rt_uint8_t year;
    rt_uint8_t mon;
    rt_uint8_t day;
    rt_uint8_t week;
    rt_uint8_t hour;
    rt_uint8_t min;
    rt_uint8_t sec;
}disp_time_t;
extern disp_time_t disp_t;


void led_scan(void);
void GPIO_Init(void);
void disp_background(uint8_t index,uint8_t c);
void draw_pixel(uint8_t x, uint8_t y,uint8_t c);
void draw_line(uint8_t x, uint8_t y,uint8_t deltax, uint8_t deltay, uint8_t c);
void color_init(void);
void color_change(void);
ColorDef get_color(void);
void move_effect(ColorDef color);

#endif

