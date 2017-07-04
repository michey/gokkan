/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "stm32f1xx_it.h"
#include "cmsis_os.h"

/* USER CODE BEGIN 0 */

#include <stdlib.h>
#include <string.h>
#include "main.h"

extern uint8_t serial_RX_Data[256];
extern uint8_t RX_Data;

extern uint8_t canLocked;
extern uint8_t serialLocked;

extern uint8_t* pt_serialMessage;
//extern uint8_t* pt_CANMessage;

extern osMessageQId CAN_TO_SERIALHandle;
extern osMessageQId SERIAL_TO_CANHandle;

uint8_t serial_RX_iter = 0x00;
uint8_t serial_RX_lm = 0x00;

uint8_t STOP_CHAR = '#';
uint8_t CMD_CHAR = '>';
uint8_t FILTER_CHAR = '^';

uint8_t FILTER_DBG[] = "FILTER_LOL\n";
uint8_t CMD_DBG[] = "CMD_LOL\n";

uint8_t BLYA_DBG[] = "BLAY=((((\n";


void processCanMessage(CAN_HandleTypeDef *can) {
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    CanRxMsgTypeDef *receiveMsg = malloc(sizeof(CanRxMsgTypeDef));
    SMessage sMessage;
    memcpy(receiveMsg, can->pRxMsg, sizeof(CanRxMsgTypeDef));
    sMessage.serialMessageId = 0;
    sMessage.dataPointer = (uint8_t*) receiveMsg;
    xQueueSendFromISR(CAN_TO_SERIALHandle, &sMessage, NULL);
}

void sendCommand(uint8_t *msg, uint8_t length) {
   
    uint32_t sizeOfTxMsg = sizeof(CanTxMsgTypeDef);
    uint8_t *pt_CANMessage = malloc(sizeOfTxMsg);
   
  
    memcpy(pt_CANMessage, msg + 1, sizeOfTxMsg);
    
    CANMessage cMessage;
    cMessage.canMessageId = 0;
    cMessage.dataPointer = pt_CANMessage;    
    xQueueSendFromISR(SERIAL_TO_CANHandle, &cMessage, NULL);
}

void processSerialMessage(UART_HandleTypeDef *uart) {
    HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);

    uint8_t message_length = 0;
    uint32_t sizeofReciverBuffer = sizeof(serial_RX_Data);
    if (serial_RX_lm < serial_RX_iter) {
        message_length = serial_RX_iter - serial_RX_lm;
    } else {
        message_length = (sizeofReciverBuffer - 1) - serial_RX_lm + serial_RX_iter;
    }

    uint8_t *message_buff;
    message_buff = (uint8_t *) malloc(message_length);

    for (uint8_t i = 0; i < message_length; i++) {
        uint8_t buff_idx = serial_RX_lm + i;
        if (buff_idx == (sizeofReciverBuffer - 1)) {
            buff_idx = buff_idx - (sizeofReciverBuffer - 1);
        }
        message_buff[i] = serial_RX_Data[buff_idx];
    }

    if (message_buff[0] == FILTER_CHAR) {
        HAL_UART_Transmit(uart, FILTER_DBG, sizeof(FILTER_DBG) - 1, 100);
    } else if (message_buff[0] == CMD_CHAR) {
        sendCommand(message_buff, message_length);
    }
    
    free(message_buff);

}

void receiveChar(uint8_t ch, UART_HandleTypeDef *uart) {
    if (ch == STOP_CHAR) {
        processSerialMessage(uart);
        serial_RX_lm = serial_RX_iter;
    } else {
        serial_RX_Data[serial_RX_iter] = ch;
        serial_RX_iter++;
        if (serial_RX_iter == (sizeof(serial_RX_Data) - 1)) {
            serial_RX_iter = 0;
        }
    }
}

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern CAN_HandleTypeDef hcan;
extern UART_HandleTypeDef huart3;

extern TIM_HandleTypeDef htim1;

/******************************************************************************/
/*            Cortex-M3 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN HardFault_IRQn 1 */

  /* USER CODE END HardFault_IRQn 1 */
}

/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN MemoryManagement_IRQn 1 */

  /* USER CODE END MemoryManagement_IRQn 1 */
}

/**
* @brief This function handles Prefetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN BusFault_IRQn 1 */

  /* USER CODE END BusFault_IRQn 1 */
}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN UsageFault_IRQn 1 */

  /* USER CODE END UsageFault_IRQn 1 */
}

/**
* @brief This function handles Debug monitor.
*/
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  osSystickHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles RCC global interrupt.
*/
void RCC_IRQHandler(void)
{
  /* USER CODE BEGIN RCC_IRQn 0 */

  /* USER CODE END RCC_IRQn 0 */
  /* USER CODE BEGIN RCC_IRQn 1 */

  /* USER CODE END RCC_IRQn 1 */
}

/**
* @brief This function handles USB high priority or CAN TX interrupts.
*/
void USB_HP_CAN1_TX_IRQHandler(void)
{
  /* USER CODE BEGIN USB_HP_CAN1_TX_IRQn 0 */

  /* USER CODE END USB_HP_CAN1_TX_IRQn 0 */
  HAL_CAN_IRQHandler(&hcan);
  /* USER CODE BEGIN USB_HP_CAN1_TX_IRQn 1 */

  /* USER CODE END USB_HP_CAN1_TX_IRQn 1 */
}

/**
* @brief This function handles USB low priority or CAN RX0 interrupts.
*/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  /* USER CODE BEGIN USB_LP_CAN1_RX0_IRQn 0 */

  /* USER CODE END USB_LP_CAN1_RX0_IRQn 0 */
  HAL_CAN_IRQHandler(&hcan);
  /* USER CODE BEGIN USB_LP_CAN1_RX0_IRQn 1 */

  /* USER CODE END USB_LP_CAN1_RX0_IRQn 1 */
}

/**
* @brief This function handles CAN RX1 interrupt.
*/
void CAN1_RX1_IRQHandler(void)
{
  /* USER CODE BEGIN CAN1_RX1_IRQn 0 */

  /* USER CODE END CAN1_RX1_IRQn 0 */
  HAL_CAN_IRQHandler(&hcan);
  /* USER CODE BEGIN CAN1_RX1_IRQn 1 */

  /* USER CODE END CAN1_RX1_IRQn 1 */
}

/**
* @brief This function handles CAN SCE interrupt.
*/
void CAN1_SCE_IRQHandler(void)
{
  /* USER CODE BEGIN CAN1_SCE_IRQn 0 */

  /* USER CODE END CAN1_SCE_IRQn 0 */
  HAL_CAN_IRQHandler(&hcan);
  /* USER CODE BEGIN CAN1_SCE_IRQn 1 */

  /* USER CODE END CAN1_SCE_IRQn 1 */
}

/**
* @brief This function handles TIM1 update interrupt.
*/
void TIM1_UP_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_IRQn 0 */

  /* USER CODE END TIM1_UP_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_UP_IRQn 1 */

  /* USER CODE END TIM1_UP_IRQn 1 */
}

/**
* @brief This function handles USART3 global interrupt.
*/
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */

  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART3_IRQn 1 */


  /* USER CODE END USART3_IRQn 1 */
}

/* USER CODE BEGIN 1 */

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    serialLocked = 0;
    SMessage sMessage;
    xQueueReceiveFromISR(CAN_TO_SERIALHandle, &sMessage, NULL);
    free(pt_serialMessage);
}


void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef *hcan) {
    canLocked = 0;
    CANMessage canMsg;
    xQueueReceiveFromISR(SERIAL_TO_CANHandle, &canMsg, NULL);
    free(canMsg.dataPointer);
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    receiveChar(RX_Data, &huart3);
    HAL_UART_Receive_IT(&huart3, &RX_Data, 1);
}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef *can) {
    processCanMessage(can);
    HAL_CAN_Receive_IT(&hcan, CAN_FIFO1);
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan) {
    HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    HAL_Delay(100);
}

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
