#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "mytimer.h"
#include "ledmatrix.h"

TIM_HandleTypeDef htim5;        //tim5
TIM_OC_InitTypeDef sConfigOC;   //for TIM9

void TIM5_IRQHandler(void);

void MX_TIM5_set_pwm(uint16_t val)
{
    sConfigOC.Pulse = val;
    if (HAL_TIM_OC_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        rt_kprintf("HAL_TIM_OC_ConfigChannel1 error\n");
    }
}

HAL_StatusTypeDef MX_TIM5_Init(void)
{
    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable TIM clock */
    __HAL_RCC_TIM5_CLK_ENABLE();

    /*##-2- Configure the NVIC for TIMx ########################################*/
    /* Set the TIM priority */
    HAL_NVIC_SetPriority(TIM5_IRQn, 0x0FU, 0);

    /* Enable the TIM global Interrupt */
    HAL_NVIC_EnableIRQ(TIM5_IRQn);

    /* Initialize TIM5 */
    htim5.Instance = TIM5;

    //htim5.Init.Period = 1000000;
    htim5.Init.Period = 800;       // 800
    htim5.Init.Prescaler = 64;       // 32
    htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
    if(HAL_TIM_Base_Init(&htim5) != HAL_OK)
    {
        rt_kprintf("HAL_TIM5_Base_Init error\n");
    }

    if (HAL_TIM_OC_Init(&htim5) != HAL_OK)
    {
        rt_kprintf("HAL_TIM_OC_Init error\n");
    }

    sConfigOC.OCMode = TIM_OCMODE_TIMING;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    MX_TIM5_set_pwm(400);
    HAL_TIM_OC_Start_IT(&htim5,TIM_CHANNEL_1);

    /* Start the TIM time Base generation in interrupt mode */
    HAL_TIM_Base_Start_IT(&htim5);

    /* Return function status */
    return HAL_OK;

}


/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM5 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM5)
    {
        //HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);
        OE_LED_DIS;
        led_scan();
        //HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);
    }
}

/**
  * @brief  Output Compare callback in non blocking mode
  * @param  htim pointer to a TIM_HandleTypeDef structure that contains
  *                the configuration information for TIM module.
  * @retval None
  */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM5)
    {
        OE_LED_EN;
    }
}

/**
  * @brief  This function handles TIM interrupt request.
  * @param  None
  * @retval None
  */
void TIM5_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim5);
}

/* *************************************** */
/* MSH CMD */
long pwm(uint8_t argc, char **argv)
{
    static uint16_t val;

    if(argc == 2)
    {
        const char *cval = argv[1];
        val = atoi((const char *)cval);
        MX_TIM5_set_pwm(val);
    }
    return 0;
}
FINSH_FUNCTION_EXPORT(pwm, pwm test);
MSH_CMD_EXPORT(pwm, pwm test);


