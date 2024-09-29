/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "gpio.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
uint32_t ADC_Value;
uint32_t ADC[5];
float    voltage[5];
float    voltage_x_voltage_sum[5];
float    rms_voltage[5];
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// 重定向c库函数printf到串口USARTx，重定向后可使用printf函数
int fputc(int ch, FILE* f)
{
    /* 发�?�一个字节数据到串口USARTx */
    HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, 0xFFFF);
    return (ch);
}

// 重定向c库函数scanf到串口USARTx，重写向后可使用scanf、getchar等函�???
int fgetc(FILE* f)
{
    int ch;
    /* 等待串口输入数据 */
    while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) == RESET)
        ;
    HAL_UART_Receive(&huart1, (uint8_t*)&ch, 1, 0xFFFF);
    return (ch);
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

    /* USER CODE BEGIN 1 */
    int i;
    int j;
    int k;
    int gesture;
    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_USART1_UART_Init();
    /* USER CODE BEGIN 2 */

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        for (j = 0; j < 500; j++)
        {
            for (i = 0; i < 5; i++)
            {
                k = i - 1;
                if (k < 0)
                {
                    k = 5 - 1;
                }
                if (j == 0 && i == 0)
                {
                    HAL_ADC_Start(&hadc1);
                    while (__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_EOC) == RESET)
                    {
                        NULL;
                    };
                    ADC_Value                = HAL_ADC_GetValue(&hadc1);
                    ADC[i]                   = ADC_Value;
                    voltage[i]               = ADC_Value * 0.00028089887;
                    voltage_x_voltage_sum[i] = voltage_x_voltage_sum[i] + voltage[i] * voltage[i];
                }
                else if (j == 500 - 1 && i == 5 - 1)
                {
                    voltage[k]               = ADC_Value * 0.00028089887;
                    voltage_x_voltage_sum[k] = voltage_x_voltage_sum[k] + voltage[k] * voltage[k];
                    HAL_ADC_Start(&hadc1);
                    while (__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_EOC) == RESET)
                    {
                        NULL;
                    };
                    ADC_Value                = HAL_ADC_GetValue(&hadc1);
                    ADC[i]                   = ADC_Value;
                    voltage[i]               = ADC_Value * 0.00028089887;
                    voltage_x_voltage_sum[i] = voltage_x_voltage_sum[i] + voltage[i] * voltage[i];
                }
                else
                {
                    voltage[k]               = ADC_Value * 0.00028089887;
                    voltage_x_voltage_sum[k] = voltage_x_voltage_sum[k] + voltage[k] * voltage[k];
                    HAL_ADC_Start(&hadc1);
                    while (__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_EOC) == RESET)
                    {
                        NULL;
                    };
                    ADC_Value = HAL_ADC_GetValue(&hadc1);
                }
            }
            HAL_ADC_Stop(&hadc1);
        }
        gesture = 5;
        for (i = 0; i < 5; i++)
        {
            rms_voltage[i]           = sqrt(voltage_x_voltage_sum[i] / 500);
            voltage_x_voltage_sum[i] = 0;
            if (rms_voltage[i] > 0.5)
            {
                gesture--;
            }
        }
        printf("%fV,%fV,%fV,%fV,%fV,gesture=%d\n", rms_voltage[0], rms_voltage[1], rms_voltage[2], rms_voltage[3], rms_voltage[4], gesture);
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef       RCC_OscInitStruct = { 0 };
    RCC_ClkInitTypeDef       RCC_ClkInitStruct = { 0 };
    RCC_PeriphCLKInitTypeDef PeriphClkInit     = { 0 };

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection    = RCC_ADCPCLK2_DIV2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
