/**
  ******************************************************************************
  * @file    SysTick/TimeBase/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "usb_lib.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "sdcard.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// Timer variable
volatile uint16_t TimerCount = 0;

// UART1 Messages buffer
char RxBuffer[RxBufferSize];
volatile unsigned char RxCounter = 0;
volatile unsigned char ParseUSART1;

// struct data container from stm32f10x_it.h
AXISDATA mAxisData;
AXISBUF  mAxisBuf;

// GLCD Graph container
unsigned char mode = 0, i, j, x, y , offset_start = 0, offset_finish = 0;
int BitCount = 15, index = 0, RbitFlag = false, flag_uart = 0;

// systick variable
__IO uint32_t TimingDelay = 0;

// DEBUG
int HENDHz = 0, TENKHZ = 0;

//DEBUG
bool HardFaultFlag = true;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void) {
  if(HardFaultFlag) {
    HardFaultFlag = false;
    printf("\r\nHardFault Error Occured!"); 
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void) {
  TimingDelay_Decrement();
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void delay_ms(__IO uint32_t nTime) {
  TimingDelay = nTime * 1000;

  while(TimingDelay != 0);
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in microseconds.
  * @retval None
  */
void delay_us(__IO uint32_t nTime) {
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void) {
  if (TimingDelay != 0x00) {
    TimingDelay--;
  }
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles TIM2 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void) {
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    
    TimerCount++;
  }
}

/**
  * @brief  This function handles USART1 global interrupt request.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void) {
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
    RxBuffer[RxCounter] = USART_ReceiveData(USART1) & 0xFF;
    if(RxBuffer[RxCounter] == '\n') {
      while(RxBuffer[RxCounter + 1] != 0) {
          RxBuffer[RxCounter + 1] = 0;
      }
      ParseUSART1 = true;
      RxCounter = 0;
    } else {
      RxCounter++;
    }
  }
}

/**
  * @brief  This function handles USB High Priority or CAN TX interrupts requests requests.
  * @param  None
  * @retval None
  */
void USB_HP_CAN1_TX_IRQHandler(void) {
  CTR_HP();
}

/**
  * @brief  This function handles USB Low Priority or CAN RX0 interrupts requests.
  * @param  None
  * @retval None
  */
void USB_LP_CAN1_RX0_IRQHandler(void) {
  USB_Istr();
}

/**
  * @brief  This function handles SDIO interrupt request.
  * @param  None
  * @retval None
  */
void SDIO_IRQHandler(void) {
  /* Process All SDIO Interrupt Sources */
  SD_ProcessIRQSrc();
}

/**
  * @brief  This function handles EXTI1 (F_SYNC, PA01) interrupt request.
  * @param  None
  * @retval None
  */
void EXTI1_IRQHandler(void) {
  /* EXTI1(PA01 -> F_SYNC : 100Hz) */
  if(EXTI_GetITStatus(EXTI_Line1) != RESET) {
    /* Clear the  EXTI line 1 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line1);
    
    // Code to deal with F_SYNC (100Hz impulse)
    if(BitCount >= 99) {
      BitCount = 0;
      if(index >= 99) {
        index = 0;
      } else {
        // On every impulse out of 100Hz do the work
        // index increases inside while routine of main
        RbitFlag = true;
      }
    }
    
    HENDHz++;
  }
}

/**
  * @brief  This function handles EXTI2 (F_SCLK, PA02) interrupt request.
  * @param  None
  * @retval None
  */
void EXTI2_IRQHandler(void) {
  /* EXTI2(PA02 -> F_SCLK : 10KHz) */
  if(EXTI_GetITStatus(EXTI_Line2) != RESET) {
    /* Clear the  EXTI line 2 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line2);
    
    // Code to deal with F_SCLK (10KHz Squarewave)
    /* 14bit out of 100bit used to contain each axis data */
    if(BitCount < 14) {
      // shift bits so that be ready to receive newer incoming bit
      mAxisData.data_x[index] = ((mAxisData.data_x[index] << 1) & 0x3FFF);
      mAxisData.data_y[index] = ((mAxisData.data_y[index] << 1) & 0x3FFF);
      mAxisData.data_z[index] = ((mAxisData.data_z[index] << 1) & 0x3FFF);
      
      // read x, y, z data
      if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5)) // if PB5 is set add 1 to data_x[start]
         mAxisData.data_x[index] |= 1;
      if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)) // if PB0 is set add 1 to data_y[start]
         mAxisData.data_y[index] |= 1;
      if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)) // if PB1 is set add 1 to data_z[start]
         mAxisData.data_z[index] |= 1;
      flag_uart = 0;
    } else {
      // other than flag set activity, rest of 86bit is vacant so leave here blank
      flag_uart = 1;
    }
    // increase BitCount entire time
    BitCount++;
    TENKHZ++;
  }
}

void EXTI9_5_IRQHandler(void) {
  /* EXTI8 (PD08 USER SWITCH 1) */
  if(EXTI_GetITStatus(EXTI_Line8) != RESET) {
    /* Clear the  EXTI line 8 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line8);

    mode = SELECT_AXIS_X;    
  }
  
  /* EXTI9 (PD09 USER SWITCH 2) */
  if(EXTI_GetITStatus(EXTI_Line9) != RESET) {
    /* Clear the  EXTI line 9 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line9);
    
    mode = SELECT_AXIS_Y;
  }
  
  /* EXTI10 (PD10 USER SWITCH 3) */
  if(EXTI_GetITStatus(EXTI_Line10) != RESET) {
    /* Clear the  EXTI line 10 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line10);
    
    mode = SELECT_AXIS_Z;
  }
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
