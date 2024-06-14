/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart6;

/* USER CODE BEGIN Private defines */
#define UART_BUFF_SIZE	120
extern uint8_t data_rcv;
extern uint8_t rx_flag;
extern uint8_t rx_half;
extern uint8_t UART6_rxBuffer[UART_BUFF_SIZE];
/* USER CODE END Private defines */

void MX_USART6_UART_Init(void);

/* USER CODE BEGIN Prototypes */
uint32_t* USART6_GET_DATA_RX();
uint8_t USART6_GET_RX_FLAG();
void USART6_RESET_RX_FLAG();
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

