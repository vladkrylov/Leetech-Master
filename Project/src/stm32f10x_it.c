/**
  ******************************************************************************
  * @file    stm32f10x_it.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11/20/2009
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "stm32_eth.h"
#include "main.h"
#include "phil_aux.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
CanRxMsg RxMessage;
static can_flag FLAG = UNKNOWN;


/* Private function prototypes -----------------------------------------------*/
extern void LwIP_Pkt_Handle(void);


/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
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
  {}
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
  {}
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
  {}
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  /* Update the LocalTime by adding SYSTEMTICK_PERIOD_MS each SysTick interrupt */
  Time_Update();
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles ETH interrupt request.
  * @param  None
  * @retval None
  */
void ETH_IRQHandler(void)
{
  /* Handles all the received frames */
  while(ETH_GetRxPktSize() != 0) 
  {		
    LwIP_Pkt_Handle();
  }

  /* Clear the Eth DMA Rx IT pending bits */
  ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
  ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
}


/**
  * @brief  This function handles External lines 15 to 10 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{}

/**
  * @brief  This function handles CAN2 Handler.
  * @param  None
  * @retval None
  */
void CAN2_RX0_IRQHandler(void)
{
	uint8_t i = 0;
	can_flag newFlag = UNKNOWN;
	
  CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);
  if ((RxMessage.StdId == 0x122)&&(RxMessage.IDE == CAN_ID_STD))
  {
		newFlag = GetTypeOfCANData(RxMessage.Data);
		
		if (newFlag == TRAJECTORY_TRANSMITTED) {
			SendTrajectory();
			return;
		}

		if (newFlag == FINISH) {
			FLAG = newFlag;
			AccumulateArray(FLAG, RxMessage.Data);
			// exit and wait for new data
			return;
		}
		
		// if the actual flag means nothing get the new flag
		if ((FLAG == FINISH) || (FLAG == UNKNOWN)) {
			FLAG = newFlag;
		}
		
		switch (FLAG) {
			case SINGLE_COORDINALTE:
				for(i=0; i<8; i++) {
					dataFromSlaveBoard[9+i] = 0;
				}
				for(i=0; i<RxMessage.DLC; i++) {
					dataFromSlaveBoard[9+i] = RxMessage.Data[i];
				}
				SendDataToComp((uint8_t *)dataFromSlaveBoard, LENGTH_OF_RESPONSE);
				FLAG = FINISH;
				break;
			case TIME_START:
				FLAG = TIME;
				break;
			case U_SIGNAL_START:
				FLAG = U_SIGNAL;
				break;
			case COOORDINATES_START:
				FLAG = COOORDINATES;
				break;
			case TIME:
			case U_SIGNAL:
			case COOORDINATES:
				AccumulateArray(FLAG, RxMessage.Data);
				break;
			default:
				FLAG = UNKNOWN;
		}
	}
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
