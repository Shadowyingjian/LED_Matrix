

#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "ledmatrix.h"
#include "morphdigit.h"
#include "calendar.h"
#include "dot_matrix.h"

#define ROW_D GPIO_PIN_0
#define ROW_C GPIO_PIN_1
#define ROW_B GPIO_PIN_2
#define ROW_A GPIO_PIN_3

#define ROW_0       GPIOC->BSRR = (ROW_D << 16) | (ROW_C << 16) | (ROW_B << 16) | (ROW_A << 16);
#define ROW_1       GPIOC->BSRR = (ROW_D << 16) | (ROW_C << 16) | (ROW_B << 16) | ROW_A;
#define ROW_2       GPIOC->BSRR = (ROW_D << 16) | (ROW_C << 16) | ROW_B         | (ROW_A << 16);
#define ROW_3       GPIOC->BSRR = (ROW_D << 16) | (ROW_C << 16) | ROW_B         | ROW_A;
#define ROW_4       GPIOC->BSRR = (ROW_D << 16) | ROW_C         | (ROW_B << 16) | (ROW_A << 16);
#define ROW_5       GPIOC->BSRR = (ROW_D << 16) | ROW_C         | (ROW_B << 16) | ROW_A;
#define ROW_6       GPIOC->BSRR = (ROW_D << 16) | ROW_C         | ROW_B         | (ROW_A << 16);
#define ROW_7       GPIOC->BSRR = (ROW_D << 16) | ROW_C         | ROW_B         | ROW_A;
#define ROW_8       GPIOC->BSRR = ROW_D         | (ROW_C << 16) | (ROW_B << 16) | (ROW_A << 16);
#define ROW_9       GPIOC->BSRR = ROW_D         | (ROW_C << 16) | (ROW_B << 16) | ROW_A;
#define ROW_10      GPIOC->BSRR = ROW_D         | (ROW_C << 16) | ROW_B         | (ROW_A << 16);
#define ROW_11      GPIOC->BSRR = ROW_D         | (ROW_C << 16) | ROW_B         | ROW_A;
#define ROW_12      GPIOC->BSRR = ROW_D         | ROW_C         | (ROW_B << 16) | (ROW_A << 16);
#define ROW_13      GPIOC->BSRR = ROW_D         | ROW_C         | (ROW_B << 16) | ROW_A;
#define ROW_14      GPIOC->BSRR = ROW_D         | ROW_C         | ROW_B         | (ROW_A << 16);
#define ROW_15      GPIOC->BSRR = ROW_D         | ROW_C         | ROW_B         | ROW_A;

#define LAT_LE_H    GPIOA->BSRR = GPIO_PIN_1;
#define LAT_LOCK    GPIOA->BSRR = (GPIO_PIN_1 << 16);

#define DAT_R1(x)    {HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,x);}
#define DAT_G1(x)    {HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,x);}
#define DAT_B1(x)    {HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,x);}

#define DAT_R2(x)    {HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,x);}
#define DAT_G2(x)    {HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,x);}
#define DAT_B2(x)    {HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,x);}

#define CTRL_CLK(x)  {HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,x);}

dat_rgb_t dat;

/*
x=0~63 y=0~31
鐢荤偣
*/
void draw_pixel(uint8_t x, uint8_t y,ColorDef c)
{
    switch(c)
    {
        case RED:
            if(x < 32)
            {
                dat.r32[31 - y][0] |= (1 << x);
            }
            else
            {
                dat.r32[31 - y][1] |= (1 << (x - 32));
            }
            break;

        case GREEN:
            if(x < 32)
            {
                dat.g32[31 - y][0] |= (1 << x);
            }
            else
            {
                dat.g32[31 - y][1] |= (1 << (x - 32));
            }
            break;

        case BLUE:
            if(x < 32)
            {
                dat.b32[31 - y][0] |= (1 << x);
            }
            else
            {
                dat.b32[31 - y][1] |= (1 << (x - 32));
            }
            break;

        case YELLOW:
            if(x < 32)
            {
                dat.r32[31 - y][0] |= (1 << x);
                dat.g32[31 - y][0] |= (1 << x);
            }
            else
            {
                dat.r32[31 - y][1] |= (1 << (x - 32));
                dat.g32[31 - y][1] |= (1 << (x - 32));
            }
            break;

        case PURPLE:
            if(x < 32)
            {
                dat.r32[31 - y][0] |= (1 << x);
                dat.b32[31 - y][0] |= (1 << x);
            }
            else
            {
                dat.r32[31 - y][1] |= (1 << (x - 32));
                dat.b32[31 - y][1] |= (1 << (x - 32));
            }
            break;

        case CYAN:
            if(x < 32)
            {
                dat.b32[31 - y][0] |= (1 << x);
                dat.g32[31 - y][0] |= (1 << x);
            }
            else
            {
                dat.b32[31 - y][1] |= (1 << (x - 32));
                dat.g32[31 - y][1] |= (1 << (x - 32));
            }
            break;

        case WHITE:
            if(x < 32)
            {
                dat.r32[31 - y][0] |= (1 << x);
                dat.b32[31 - y][0] |= (1 << x);
                dat.g32[31 - y][0] |= (1 << x);
            }
            else
            {
                dat.r32[31 - y][1] |= (1 << (x - 32));
                dat.b32[31 - y][1] |= (1 << (x - 32));
                dat.g32[31 - y][1] |= (1 << (x - 32));
            }
            break;

        case BLACK:
            if(x < 32)
            {
                dat.r32[31 - y][0] &= ~(1 << x);
                dat.b32[31 - y][0] &= ~(1 << x);
                dat.g32[31 - y][0] &= ~(1 << x);
            }
            else
            {
                dat.r32[31 - y][1] &= ~(1 << (x - 32));
                dat.b32[31 - y][1] &= ~(1 << (x - 32));
                dat.g32[31 - y][1] &= ~(1 << (x - 32));
            }
            break;

        default:
            break;
    }
}

/*
x2>x,y2>y
鐢荤嚎
*/
void draw_line(uint8_t x, uint8_t y,uint8_t deltax, uint8_t deltay, ColorDef c)
{
    uint8_t i;
    if(deltax == 0)
    {
        for(i=0; i<deltay; i++)
        {
            draw_pixel(x, y + i, c);
        }
    }
    else if(deltay == 0)
    {
        for(i=0; i<deltax; i++)
        {
            draw_pixel(x + i, y, c);
        }
    }
}

void move_effect(ColorDef color)
{
    uint8_t i,j;
    uint8_t temp_bit;

    for(i=0;i<16;i++)
    {
        temp_bit = dat.move_buf[i][0] & 0x1;
        for(j=0;j<MOVE_BUFF-1;j++)
        {
            dat.move_buf[i][j] >>= 1;
            if(dat.move_buf[i][j + 1] & 0x1)
            {
                dat.move_buf[i][j] |= 0x80;
            }
            //rt_kprintf("%d %d=0x%x\n",i,j,dat.move_buf[i][j]);
        }
        dat.move_buf[i][MOVE_BUFF-1] >>= 1;
        if(temp_bit)
        {
            dat.move_buf[i][MOVE_BUFF-1] |= 0x80;
        }
    }


    for(i=0;i<16;i++)
    {
        for(j=0;j<8;j++)
        {
            switch(color)
            {
                case RED:
                    for(i=0; i<16; i++)
                    {
                        dat.r[i][j] = dat.move_buf[i][j];
                    }
                    break;

                case GREEN:
                    for(i=0; i<16; i++)
                    {
                        dat.g[i][j] = dat.move_buf[i][j];
                    }
                    break;

                case BLUE:
                    for(i=0; i<16; i++)
                    {
                        dat.b[i][j] = dat.move_buf[i][j];
                    }
                    break;

                case YELLOW:
                    for(i=0; i<16; i++)
                    {
                        dat.r[i][j] = dat.move_buf[i][j];
                        dat.g[i][j] = dat.move_buf[i][j];
                    }
                    break;

                case PURPLE:
                    for(i=0; i<16; i++)
                    {
                        dat.r[i][j] = dat.move_buf[i][j];
                        dat.b[i][j] = dat.move_buf[i][j];
                    }
                    break;

                case CYAN:
                    for(i=0; i<16; i++)
                    {
                        dat.g[i][j] = dat.move_buf[i][j];
                        dat.b[i][j] = dat.move_buf[i][j];
                    }
                    break;

                case WHITE:
                    for(i=0; i<16; i++)
                    {
                        dat.r[i][j] = dat.move_buf[i][j];
                        dat.g[i][j] = dat.move_buf[i][j];
                        dat.b[i][j] = dat.move_buf[i][j];
                    }
                    break;

                default:
                    break;

            }
        }
    }
}

/* x=0~7 y=0~16 */
static void disp8x16(uint8_t x, uint8_t y, uint8_t *pVal,ColorDef color)
{
    uint8_t i;

    switch(color)
    {
        case RED:
            for(i=0; i<16; i++)
            {
                dat.r[y + i][x] = *(pVal + i);
            }
            break;

        case GREEN:
            for(i=0; i<16; i++)
            {
                dat.g[y + i][x] = *(pVal + i);
            }
            break;

        case BLUE:
            for(i=0; i<16; i++)
            {
                dat.b[y + i][x] = *(pVal + i);
            }
            break;

        case YELLOW:
            for(i=0; i<16; i++)
            {
                dat.r[y + i][x] = *(pVal + i);
                dat.g[y + i][x] = *(pVal + i);
            }
            break;

        case PURPLE:
            for(i=0; i<16; i++)
            {
                dat.r[y + i][x] = *(pVal + i);
                dat.b[y + i][x] = *(pVal + i);
            }
            break;

        case CYAN:
            for(i=0; i<16; i++)
            {
                dat.g[y + i][x] = *(pVal + i);
                dat.b[y + i][x] = *(pVal + i);
            }
            break;

        case WHITE:
            for(i=0; i<16; i++)
            {
                dat.r[y + i][x] = *(pVal + i);
                dat.g[y + i][x] = *(pVal + i);
                dat.b[y + i][x] = *(pVal + i);
            }
            break;

        default:
            break;

    }
}

static void shift_dat(uint8_t row)
{
    static uint8_t i,j;
    static uint32_t datr_shit1,datr_shit2;
    static uint32_t datg_shit1,datg_shit2;
    static uint32_t datb_shit1,datb_shit2;

    for(i=0; i<2; i++)
    {
        datr_shit1 = dat.r32[row][i];
        datr_shit2 = dat.r32[row + 16][i];
        datg_shit1 = dat.g32[row][i];
        datg_shit2 = dat.g32[row + 16][i];
        datb_shit1 = dat.b32[row][i];
        datb_shit2 = dat.b32[row + 16][i];
        for(j=0; j<32; j++)
        {
            CTRL_CLK(GPIO_PIN_RESET);
            if(datr_shit1 & 0x1)
            {
                DAT_R1(GPIO_PIN_SET);
            }
            else
            {
                DAT_R1(GPIO_PIN_RESET);
            }

            if(datg_shit1 & 0x1)
            {
                DAT_G1(GPIO_PIN_SET);
            }
            else
            {
                DAT_G1(GPIO_PIN_RESET);
            }

            if(datb_shit1 & 0x1)
            {
                DAT_B1(GPIO_PIN_SET);
            }
            else
            {
                DAT_B1(GPIO_PIN_RESET);
            }

            if(datr_shit2 & 0x1)
            {
                DAT_R2(GPIO_PIN_SET);
            }
            else
            {
                DAT_R2(GPIO_PIN_RESET);
            }

            if(datg_shit2 & 0x1)
            {
                DAT_G2(GPIO_PIN_SET);
            }
            else
            {
                DAT_G2(GPIO_PIN_RESET);
            }

            if(datb_shit2 & 0x1)
            {
                DAT_B2(GPIO_PIN_SET);
            }
            else
            {
                DAT_B2(GPIO_PIN_RESET);
            }
            CTRL_CLK(GPIO_PIN_SET);
            datr_shit1 >>= 1;
            datg_shit1 >>= 1;
            datb_shit1 >>= 1;
            datr_shit2 >>= 1;
            datg_shit2 >>= 1;
            datb_shit2 >>= 1;
        }
    }
    CTRL_CLK(GPIO_PIN_RESET);
}

/* TIM中断，0.8ms调用，用于扫描 */
void led_scan(void)
{
    static uint8_t index = 0;

    shift_dat(index);
    //在锁存输出显示之前，将显示屏关闭，行切换后再打开，解决拖影问题
    LAT_LE_H;
    switch(index)
    {
        case 0x0:
            ROW_0;
            break;

        case 0x1:
            ROW_1;
            break;

        case 0x2:
            ROW_2;
            break;

        case 0x3:
            ROW_3;
            break;

        case 0x4:
            ROW_4;
            break;

        case 0x5:
            ROW_5;
            break;

        case 0x6:
            ROW_6;
            break;

        case 0x7:
            ROW_7;
            break;

        case 0x8:
            ROW_8;
            break;

        case 0x9:
            ROW_9;
            break;

        case 0xa:
            ROW_10;
            break;

        case 0xb:
            ROW_11;
            break;

        case 0xc:
            ROW_12;
            break;

        case 0xd:
            ROW_13;
            break;

        case 0xe:
            ROW_14;
            break;

        case 0xf:
            ROW_15;
            break;

        default:
            break;
    }
    LAT_LOCK;

    index++;
    if(index > 15)
    {
        index = 0;
    }
}

void color_init(void)
{
    dat.color = RED;
}

void color_change(void)
{
    dat.color++;
    if(dat.color == BLACK)
    {
        dat.color = RED;
    }
}

ColorDef get_color(void)
{
    return dat.color;
}

void GPIO_Init(void)
{
    static GPIO_InitTypeDef  GPIO_InitStruct;

    /*##-1- Enable GPIOX Clock (to be able to program the configuration registers) */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /*##-2- Configure IO in output push-pull mode to drive ###*/
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2; //LED_OE LAT CLK
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_10 | GPIO_PIN_14;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
    rt_thread_delay(10);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);

    // led
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);

    /*##-2- Configure IO in input mode to key ###*/
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

void disp_background(uint8_t index,ColorDef c)
{
    uint8_t *pNum;
    if(index == 0)  //led clr
    {
        memset(dat.r32,0,LED_BUFF_SIZE);
    }
    else if(index == 1)  //disp symbol
    {
        pNum = &gap[0 << 4];
        disp8x16(2,LINE_1,pNum,c);
        disp8x16(5,LINE_1,pNum,c);
    }
    else if(index == 6) //disp zhufu
    {
        switch(c)
        {
            case 0:
                memset(dat.r32,0,LED_BUFF_SIZE);
                memcpy(dat.r32,(const void *)&zhufu[0],sizeof(dat.r32));
                break;

            case 1:
                memset(dat.r32,0,LED_BUFF_SIZE);
                memcpy(dat.g32,(const void *)&zhufu[0],sizeof(dat.r32));
                break;

            case 2:
                memset(dat.r32,0,LED_BUFF_SIZE);
                memcpy(dat.b32,(const void *)&zhufu[0],sizeof(dat.r32));
                break;

            case 3:
                memset(dat.r32,0,LED_BUFF_SIZE);
                memcpy(dat.r32,(const void *)&zhufu[0],sizeof(dat.r32));
                memcpy(dat.g32,(const void *)&zhufu[0],sizeof(dat.r32));
                break;

            case 4:
                memset(dat.r32,0,LED_BUFF_SIZE);
                memcpy(dat.r32,(const void *)&zhufu[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&zhufu[0],sizeof(dat.r32));
                break;

            case 5:
                memset(dat.r32,0,LED_BUFF_SIZE);
                memcpy(dat.g32,(const void *)&zhufu[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&zhufu[0],sizeof(dat.r32));
                break;

            case 6:
                memset(dat.r32,0,LED_BUFF_SIZE);
                memcpy(dat.r32,(const void *)&zhufu[0],sizeof(dat.r32));
                memcpy(dat.g32,(const void *)&zhufu[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&zhufu[0],sizeof(dat.r32));
                break;

            default:
                break;
        }
    }
    else if(index == 7) //disp happy
    {
        switch(c)
        {
            case 0:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&happy[0],sizeof(dat.r32));
                break;

            case 1:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.g32,(const void *)&happy[0],sizeof(dat.r32));
                break;

            case 2:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.b32,(const void *)&happy[0],sizeof(dat.r32));
                break;

            case 3:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&happy[0],sizeof(dat.r32));
                memcpy(dat.g32,(const void *)&happy[0],sizeof(dat.r32));
                break;

            case 4:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&happy[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&happy[0],sizeof(dat.r32));
                break;

            case 5:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.g32,(const void *)&happy[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&happy[0],sizeof(dat.r32));
                break;

            case 6:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&happy[0],sizeof(dat.r32));
                memcpy(dat.g32,(const void *)&happy[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&happy[0],sizeof(dat.r32));
                break;

            default:
                break;
        }
    }
    else if(index == 8) //disp happy
    {
        switch(c)
        {
            case 0:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&kands[0],sizeof(dat.r32));
                break;

            case 1:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.g32,(const void *)&kands[0],sizeof(dat.r32));
                break;

            case 2:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.b32,(const void *)&kands[0],sizeof(dat.r32));
                break;

            case 3:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&kands[0],sizeof(dat.r32));
                memcpy(dat.g32,(const void *)&kands[0],sizeof(dat.r32));
                break;

            case 4:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&kands[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&kands[0],sizeof(dat.r32));
                break;

            case 5:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.g32,(const void *)&kands[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&kands[0],sizeof(dat.r32));
                break;

            case 6:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&kands[0],sizeof(dat.r32));
                memcpy(dat.g32,(const void *)&kands[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&kands[0],sizeof(dat.r32));
                break;

            default:
                break;
        }
    }
    else if(index == 9) //disp ship
    {
        switch(c)
        {
            case 0:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&ship[0],sizeof(dat.r32));
                break;

            case 1:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.g32,(const void *)&ship[0],sizeof(dat.r32));
                break;

            case 2:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.b32,(const void *)&ship[0],sizeof(dat.r32));
                break;

            case 3:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&ship[0],sizeof(dat.r32));
                memcpy(dat.g32,(const void *)&ship[0],sizeof(dat.r32));
                break;

            case 4:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&ship[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&ship[0],sizeof(dat.r32));
                break;

            case 5:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.g32,(const void *)&ship[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&ship[0],sizeof(dat.r32));
                break;

            case 6:
                memset(dat.r32,0,sizeof(dat));
                memcpy(dat.r32,(const void *)&ship[0],sizeof(dat.r32));
                memcpy(dat.g32,(const void *)&ship[0],sizeof(dat.r32));
                memcpy(dat.b32,(const void *)&ship[0],sizeof(dat.r32));
                break;

            default:
                break;
        }
    }
}


/* *************************************** */
/* MSH CMD */
extern void led_disp_reinit(void);
long led(uint8_t argc, char **argv)
{
    static uint8_t x,y,v,c;
    uint8_t *pNum;

    if(argc == 2)
    {
        const char *cx = argv[1];
        if (!strcmp(cx, "clr"))
        {
            rt_kprintf("clr\r\n");
            memset(dat.r32,0,sizeof(dat));
        }
        else if (!strcmp(cx, "rectr"))
        {
            rt_kprintf("rectr\r\n");
            memcpy(dat.r32,(const void *)&rect[0],sizeof(dat.r32));
        }
        else if (!strcmp(cx, "rectg"))
        {
            rt_kprintf("rectg\r\n");
            memcpy(dat.g32,(const void *)&rect[0],sizeof(dat.r32));
        }
        else if (!strcmp(cx, "rectb"))
        {
            rt_kprintf("rectb\r\n");
            memcpy(dat.b32,(const void *)&rect[0],sizeof(dat.r32));
        }
        else if (!strcmp(cx, "ship"))
        {
            rt_kprintf("ship\r\n");
            memcpy(dat.b32,(const void *)&ship[0],sizeof(dat.r32));
        }
        else if (!strcmp(cx, "move"))
        {
            rt_kprintf("move\r\n");
            move_effect(RED);
        }
        else if (!strcmp(cx, "size"))
        {
            rt_kprintf("dat=%d\n",sizeof(dat));
            rt_kprintf("dat.r=%d\n",sizeof(dat.r));
            rt_kprintf("dat.r32=%d\n",sizeof(dat.r32));
            rt_kprintf("dat.move=%d\n",sizeof(dat.move_buf));
            rt_kprintf("dat.color=%d\n",sizeof(dat.color));
        }
    }

    if(argc == 3)
    {
        const char *cx = argv[1];
        const char *cc = argv[2];

        c = atoi((const char *)cc);

        if(!strcmp(cx, "c"))
        {
            rt_kprintf("color=%d\r\n",c);
            dat.color = (ColorDef)c;
            led_disp_reinit();
        }
        return 0;
    }

    if(argc == 5)
    {
        const char *cx = argv[1];
        const char *cy = argv[2];
        const char *cc = argv[3];
        const char *cv = argv[4];

        x = atoi((const char *)cx);
        y = atoi((const char *)cy);
        c = atoi((const char *)cc);
        v = atoi((const char *)cv);

        rt_kprintf("%d,%d,%d,%d ",x,y,c,v);

        if(v <= 10)
        {
            pNum = &num[v << 4];
            disp8x16(x,y,pNum,(ColorDef)c);
            rt_kprintf("num\r\n");
        }
        return 0;
    }
    return 0;
}
FINSH_FUNCTION_EXPORT(led, led test);
MSH_CMD_EXPORT(led, led test);

