#ifndef __PHIL_CAN_H
#define __PHIL_CAN_H

#include "stm32f10x.h"

CAN_InitTypeDef        Phil_CAN_InitStructure;
CAN_FilterInitTypeDef  Phil_CAN_FilterInitStructure;
CanTxMsg Phil_TxMessage;

/* Define the STM32F10x hardware depending on the used evaluation board */

  #define RCC_APB2Periph_GPIO_CAN1    RCC_APB2Periph_GPIOD
  #define GPIO_Remapping_CAN1         GPIO_Remap2_CAN1
  #define GPIO_CAN1                   GPIOD  
  #define GPIO_Pin_CAN1_RX            GPIO_Pin_0
  #define GPIO_Pin_CAN1_TX            GPIO_Pin_1
  
  #define RCC_APB2Periph_GPIO_CAN2    RCC_APB2Periph_GPIOB
  #define GPIO_Remapping_CAN2         GPIO_Remap_CAN2
  #define GPIO_CAN2                   GPIOB  
  #define GPIO_Pin_CAN2_RX            GPIO_Pin_5
  #define GPIO_Pin_CAN2_TX            GPIO_Pin_6
	
	
//#define __CAN1_USED__
 #define __CAN2_USED__

#ifdef  __CAN1_USED__
  #define CANx                       CAN1
  #define GPIO_CAN                   GPIO_CAN1
  #define GPIO_Remapping_CAN         GPIO_Remapping_CAN1
  #define GPIO_CAN                   GPIO_CAN1
  #define GPIO_Pin_CAN_RX            GPIO_Pin_CAN1_RX
  #define GPIO_Pin_CAN_TX            GPIO_Pin_CAN1_TX
#else /*__CAN2_USED__*/
  #define CANx                       CAN2
  #define GPIO_CAN                   GPIO_CAN2
  #define GPIO_Remapping_CAN             GPIO_Remap_CAN2
  #define GPIO_CAN                   GPIO_CAN2
  #define GPIO_Pin_CAN_RX            GPIO_Pin_CAN2_RX
  #define GPIO_Pin_CAN_TX            GPIO_Pin_CAN2_TX
#endif  /* __CAN1_USED__ */

#endif /* __PHIL_CAN_H */

