/*
 * File      : main.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and 
 distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015-07-29     Arda.Fu      first implementation
 */
#include <rtthread.h>
#include <board.h>

#include "mytimer.h"
#include "ledmatrix.h"
#include "morphdigit.h"
#include "calendar.h"
#include "drv_rtc.h"
#include "fal.h"
#define EVENT_1S    (0x1 << 3)

//添加版本信息
#define APP_VERSION "1.0.0"

rt_sem_t sem_con;
static rt_event_t event_time = RT_NULL;
static rt_mutex_t mutex_move = RT_NULL;

static rt_bool_t b_disp_reinit = 0;
static disp_time_t disp_t;

void led_disp_reinit(void)
{
    b_disp_reinit = 0;
    morph_reinit();
}

static void led_thread_entry(void *parameter)
{
    while(1)
    {
        rt_mutex_take(mutex_move, RT_WAITING_FOREVER);
        move_effect((ColorDef)get_color());
        rt_mutex_release(mutex_move);
        rt_thread_delay(10);

        if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15) == GPIO_PIN_SET)
        {
            // led
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
        }
        else
        {
            // led
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
        }
    }
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    //rt_kprintf("rtc wackup!\n");
    rt_event_send(event_time,EVENT_1S);
}

/**
 * Function    ota_app_vtor_reconfig
 * Description Set Vector Table base location to the start addr of app(RT_APP_PART_ADDR).
*/
static int ota_app_vtor_reconfig(void)
{
    #define NVIC_VTOR_MASK   0x3FFFFF80
    /* Set the Vector Table base location by user application firmware definition */
    SCB->VTOR = RT_APP_PART_ADDR & NVIC_VTOR_MASK;

    return 0;
}
INIT_BOARD_EXPORT(ota_app_vtor_reconfig);


extern void rt_hw_rtc_wakeup_enable(void);
int main(void)
{
    uint8_t i;
    rt_uint32_t e;
	  static struct tm *tmp;
    time_t now;
    rt_thread_t tid_shift;

    rt_kprintf("SystemCoreClock = %dHz\r\n",SystemCoreClock);

    /* create thread */
    tid_shift = rt_thread_create("ledmatrix", led_thread_entry, RT_NULL,
                                    1024, 6, 20);
    RT_ASSERT(tid_shift != RT_NULL);
    
    

    /* init GPIOs & TIM5 */
    GPIO_Init();
    MX_TIM5_Init();
    
    fal_init();
   rt_kprintf("The current version of APP firmware is %s\n", APP_VERSION);


    color_init();
    MX_TIM5_set_pwm(780);  //200~800 from light to dark
	
    rt_kprintf("Wait for pair wifi\n");
		disp_background(6,1);
    rt_sem_take(sem_con, RT_WAITING_FOREVER);
    rt_kprintf("main.Pass\n");
		disp_background(7,1);
    rt_thread_delay(1000);
    disp_background(0,0);

    mutex_move = rt_mutex_create("mutex_m", RT_IPC_FLAG_FIFO);
    RT_ASSERT(mutex_move != RT_NULL);

    /* creat event */
    event_time = rt_event_create("event_time", RT_IPC_FLAG_FIFO);
    RT_ASSERT(event_time != RT_NULL);

    rt_hw_rtc_wakeup_enable();

    rt_thread_startup(tid_shift);

    while(1)
    {
        if(rt_event_recv(event_time,EVENT_1S,
           RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
           RT_WAITING_FOREVER, &e) == RT_EOK)
        {
            now = time(RT_NULL);
            tmp = localtime(&now);

            //change color everyday
            if((disp_t.day != tmp->tm_mday) || (b_disp_reinit == 0))
            {
                b_disp_reinit = 1;
                disp_t.year = tmp->tm_year;
                disp_t.mon = tmp->tm_mon + 1;
                disp_t.day = tmp->tm_mday;
                disp_t.week = tmp->tm_wday;
                morph_reinit();
                color_change();
                rt_mutex_take(mutex_move, RT_WAITING_FOREVER);
                prepare_buf(&disp_t);
                rt_mutex_release(mutex_move);
            }

            disp_t.hour = tmp->tm_hour;
            disp_t.min = tmp->tm_min;
            disp_t.sec = tmp->tm_sec;
            morph_update(&disp_t,(ColorDef)get_color());
        }
    }
}

