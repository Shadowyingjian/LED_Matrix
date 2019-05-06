#include <stdlib.h>
#include <string.h>

#include "board.h"

#include "ledmatrix.h"
#include "dot_matrix.h"

#define LINE 0

static void disp8x16_buf(uint8_t x, uint8_t y, uint8_t *pVal)
{
    uint8_t i;
    for(i=0; i<16; i++)
    {
        dat.move_buf[y + i][x] = *(pVal + i);
    }
}

void prepare_buf(disp_time_t *pdt)
{
    uint8_t *pNum;
    uint8_t offset = 0;

    static uint8_t dat_array[7];
    pdt->year -= 100;
    dat_array[0] = pdt->year / 10;
    dat_array[1] = pdt->year % 10;
    dat_array[2] = pdt->mon / 10;
    dat_array[3] = pdt->mon % 10;
    dat_array[4] = pdt->day / 10;
    dat_array[5] = pdt->day % 10;
    dat_array[6] = pdt->week % 10;

    memset(dat.move_buf,0,sizeof(dat.move_buf));

    pNum = &num[2 << 4];
    disp8x16_buf(0,LINE,pNum);  //2

    pNum = &num[0 << 4];
    disp8x16_buf(1,LINE,pNum);  //0

    pNum = &num[dat_array[0] << 4];
    disp8x16_buf(2,LINE,pNum);     //1

    pNum = &num[dat_array[1] << 4];
    disp8x16_buf(3,LINE,pNum);      //8

    pNum = &calendar[0 << 4];
    disp8x16_buf(4,LINE,pNum);     //年

    pNum = &calendar[1 << 4];
    disp8x16_buf(5,LINE,pNum);      //年

    if(dat_array[2] != 0)
    {
        pNum = &num[dat_array[2] << 4];
        disp8x16_buf(6,LINE,pNum);              //x
        offset++;
    }

    pNum = &num[dat_array[3] << 4];               //x
    disp8x16_buf(6 + offset,LINE,pNum);

    pNum = &calendar[2 << 4];               //月
    disp8x16_buf(7 + offset,LINE,pNum);

    pNum = &calendar[3 << 4];               //月
    disp8x16_buf(8 + offset,LINE,pNum);

    if(dat_array[4] != 0)
    {
        pNum = &num[dat_array[4] << 4];               //x
        disp8x16_buf(9 + offset,LINE,pNum);
        offset++;
    }

    pNum = &num[dat_array[5] << 4];               //x
    disp8x16_buf(9 + offset,LINE,pNum);

    pNum = &calendar[4 << 4];               //日
    disp8x16_buf(10 + offset,LINE,pNum);

    pNum = &calendar[5 << 4];               //日
    disp8x16_buf(11 + offset,LINE,pNum);

    pNum = &calendar[6 << 4];               //星
    disp8x16_buf(12 + offset,LINE,pNum);

    pNum = &calendar[7 << 4];               //星
    disp8x16_buf(13 + offset,LINE,pNum);

    pNum = &calendar[8 << 4];               //期
    disp8x16_buf(14 + offset,LINE,pNum);

    pNum = &calendar[9 << 4];               //期
    disp8x16_buf(15 + offset,LINE,pNum);

    dat_array[6] <<= 1;
    pNum = &weekday[dat_array[6] << 4];
    disp8x16_buf(16 + offset,LINE,pNum);

    pNum = &weekday[(dat_array[6] + 1) << 4];
    disp8x16_buf(17 + offset,LINE,pNum);


#if 0
    pNum = &zhongqiu[0];
    disp8x16_buf(20 + dat_array[2],LINE,pNum);
    pNum = &zhongqiu[1 << 4];
    disp8x16_buf(21 + dat_array[2],LINE,pNum);
    pNum = &zhongqiu[2 << 4];
    disp8x16_buf(22 + dat_array[2],LINE,pNum);
    pNum = &zhongqiu[3 << 4];
    disp8x16_buf(23 + dat_array[2],LINE,pNum);
    pNum = &zhongqiu[4 << 4];
    disp8x16_buf(24 + dat_array[2],LINE,pNum);
    pNum = &zhongqiu[5 << 4];
    disp8x16_buf(25 + dat_array[2],LINE,pNum);
    pNum = &zhongqiu[6 << 4];
    disp8x16_buf(26 + dat_array[2],LINE,pNum);
    pNum = &zhongqiu[7 << 4];
    disp8x16_buf(27 + dat_array[2],LINE,pNum);
#endif
#if 0
    for(i=0;i<16;i++)
    {
        for(j=0;j<MOVE_BUFF-1;j++)
        {
            rt_kprintf("o%d %d=0x%x\n",i,j,dat.move_buf[i][j]);
        }
    }
#endif
}
