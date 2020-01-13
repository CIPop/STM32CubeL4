/**
  ******************************************************************************
  * @file    PWR/PWR_RUN_SMPS/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use SMPS BSP API to enter
  *          SMPS in RUN mode.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"


/** @addtogroup STM32L4xx_HAL_Examples
  * @{
  */

/** @addtogroup PWR_RUN_SMPS
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LED_TOGGLE_DELAY         100
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;
__IO uint32_t button_pressed = 0;
/* Private function prototypes -----------------------------------------------*/
void SystemClock_80MHz(void);
void SystemClock_24MHz(void);
void LowPower_Setting(void);

/* Default 0x1FE for 0x200 alignement. Best result with 0x200 ie alignement 0x202*/
#if defined(__CC_ARM)
#pragma arm section code = ".ROM_While1_section"
#pragma no_inline
void whileloop1(void)
#elif defined(__ICCARM__)
#pragma section="ROM_While1_region"
void whileloop1(void) @ "While1Section";
void whileloop1(void)
#elif defined(__GNUC__)
void __attribute__((section(".ROM_While1_section"), noinline)) whileloop1(void)
#endif
{
while(1);
}
#if defined(__CC_ARM)
#pragma arm section code
#endif

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

    int main(void)
{
  /* STM32L4xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure LED1, and LED2 */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);

  /* Configure the system clock to 80 MHz */
  SystemClock_80MHz();

  /* User push-button (External line 13) will be used to wakeup the system from STOP mode */
  BSP_JOY_Init(JOY_MODE_EXTI);

  /* ------------------------------------------ */

  /* Insert 5 seconds delay */
  HAL_Delay(5000);

  /********************************/
  /* 80 MHZ                       */
  /* PWR_REGULATOR_VOLTAGE_SCALE1 */
  /* SMPS IS OFF                  */
  /********************************/

  /* ------------------ GPIO ------------------ */
  BSP_LED_Off(LED1);
  BSP_LED_Off(LED2);
  BSP_JOY_DeInit();
  LowPower_Setting();

  if (button_pressed == 1)
  {
    test_run_range1_80mhz_SMPS();
  }

  whileloop1();
}

void test_run_range1_80mhz_SMPS(void)
{
  uint32_t SMPS_status_local = 0;

  SMPS_status_local = BSP_SMPS_DeInit();
  if (SMPS_status_local != SMPS_OK)
  {
    Error_Handler();
  }
  SMPS_status_local = BSP_SMPS_Init();
  if (SMPS_status_local != SMPS_OK)
  {
    Error_Handler();
  }

  /* ------------------------------------------ */


  __HAL_FLASH_PREFETCH_BUFFER_DISABLE();
  /* Set the System clock to 24 MHz */
  /* so that SMPS can be enabled */
  SystemClock_24MHz();

  /* Insert 1 second delay */
  HAL_Delay(1000);

  /********************************/
  /* 24 MHZ                       */
  /* PWR_REGULATOR_VOLTAGE_SCALE1 */
  /* SMPS IS OFF                  */
  /********************************/

  /* ------------------------------------------ */

  /* PWR_REGULATOR_VOLTAGE_SCALE2 */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE2);
  if (SMPS_status_local != SMPS_OK)
  {
    Error_Handler();
  }

  /* Insert 1 second delay */
  HAL_Delay(1000);

  /********************************/
  /* 24 MHZ                       */
  /* PWR_REGULATOR_VOLTAGE_SCALE2 */
  /* SMPS IS OFF                  */
  /********************************/

  /* ------------------------------------------ */

  /* Enable SMPS */
  /* Check of PG but not blocking */
  SMPS_status_local = BSP_SMPS_Enable (0 , 1);

  /* Check of PG */
  SMPS_status_local = BSP_SMPS_Supply_Enable (4 , 1);


  if (SMPS_status_local != SMPS_OK)
  {
    Error_Handler();
  }

  /* Insert 2 seconds delay */
  HAL_Delay(2000);

  /********************************/
  /* 24 MHZ                       */
  /* PWR_REGULATOR_VOLTAGE_SCALE2 */
  /* SMPS IS ON                   */
  /********************************/

  /* ------------------------------------------ */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /* Set System clock to 80 MHz (MSI) */
  SystemClock_80MHz();

  /********************************/
  /* 80 MHZ                       */
  /* PWR_REGULATOR_VOLTAGE_SCALE1 */
  /* SMPS IS ON                   */
  /********************************/

  __HAL_RCC_GPIOH_CLK_DISABLE(); /* For power consumption savings */

}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 80000000
  *            HCLK(Hz)                       = 80000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 1
  *            PLL_N                          = 40
  *            PLL_R                          = 2
  *            PLL_P                          = 7
  *            PLL_Q                          = 4
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
void SystemClock_80MHz(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_3);
  HAL_RCC_DeInit();

  /* MSI is enabled after System reset, activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLP = 7;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}

/**
  * @brief  System Clock Configuration to 24 MHz
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 24000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  * @param  None
  * @retval None
  */
void SystemClock_24MHz(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_3);
  HAL_RCC_DeInit();

  /* MSI is enabled after System reset, update MSI to 24Mhz (RCC_MSIRANGE_9) */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_9;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Select MSI as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct,__HAL_FLASH_GET_LATENCY()) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LPUART1;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  Low power setting Configuration
  *         Set in analog modes all LED and joystick IOs
  * @param  None
  * @retval None
  */
void LowPower_Setting(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Pull = GPIO_NOPULL;

  GPIO_InitStruct.Pin = SEL_JOY_PIN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = RIGHT_JOY_PIN | LEFT_JOY_PIN | UP_JOY_PIN | DOWN_JOY_PIN;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = LED1_PIN | LED2_PIN;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_GPIOG_CLK_DISABLE();
  __HAL_RCC_GPIOI_CLK_DISABLE();
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* Suspend tick */
  HAL_SuspendTick();

  /* Turn LED2 */
  BSP_LED_On(LED2);
  while (1)
  {
  }
}

/**
  * @brief SYSTICK callback
  * @param None
  * @retval None
  */

void HAL_SYSTICK_Callback(void)
{
  HAL_IncTick();

  if (TimingDelay != 0)
  {
    TimingDelay--;
  }
  else
  {
    /* Toggle LED1 */

    BSP_LED_Toggle(LED1);

    if (button_pressed == 1)
    {
      BSP_LED_Toggle(LED2);
    }
    TimingDelay = LED_TOGGLE_DELAY;
  }
}

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if ((GPIO_Pin & SEL_JOY_PIN) != 0U)
  {
    /* whatever the joystick button pressed */
    button_pressed = 1;
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/