#include "stm32f10x.h"
#include "phil_can.h"
#include "phil_can_functions.h"

/**
  * @brief  Configures the CAN.
  * @param  None
  * @retval None
  */
void CAN_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* GPIO clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
#ifdef  __CAN1_USED__
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_CAN1, ENABLE);
#else /*__CAN2_USED__*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_CAN1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_CAN2, ENABLE);
#endif  /* __CAN1_USED__ */
  /* Configure CAN pin: RX */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CAN_RX;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIO_CAN, &GPIO_InitStructure);
  
  /* Configure CAN pin: TX */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CAN_TX;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_CAN, &GPIO_InitStructure);
  
  GPIO_PinRemapConfig(GPIO_Remapping_CAN , ENABLE);
  
  /* CANx Periph clock enable */
#ifdef  __CAN1_USED__
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
#else /*__CAN2_USED__*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
#endif  /* __CAN1_USED__ */
  
  
  /* CAN register init */
  CAN_DeInit(CANx);
  CAN_StructInit(&Phil_CAN_InitStructure);

  /* CAN cell init */
  Phil_CAN_InitStructure.CAN_TTCM = DISABLE;
  Phil_CAN_InitStructure.CAN_ABOM = DISABLE;
  Phil_CAN_InitStructure.CAN_AWUM = DISABLE;
  Phil_CAN_InitStructure.CAN_NART = DISABLE;
  Phil_CAN_InitStructure.CAN_RFLM = DISABLE;
  Phil_CAN_InitStructure.CAN_TXFP = DISABLE;
  Phil_CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
  
  /* CAN Baudrate = 1MBps*/
  Phil_CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
  Phil_CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
  Phil_CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
  Phil_CAN_InitStructure.CAN_Prescaler = 400;
  CAN_Init(CANx, &Phil_CAN_InitStructure);

  /* CAN filter init */
#ifdef  __CAN1_USED__
  Phil_CAN_FilterInitStructure.CAN_FilterNumber = 0;
#else /*__CAN2_USED__*/
  Phil_CAN_FilterInitStructure.CAN_FilterNumber = 14;
#endif  /* __CAN1_USED__ */
  Phil_CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  Phil_CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  Phil_CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
  Phil_CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
  Phil_CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
  Phil_CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
  Phil_CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
  Phil_CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&Phil_CAN_FilterInitStructure);
  
  /* Transmit */
  Phil_TxMessage.StdId = 0x321;
  Phil_TxMessage.ExtId = 0x01;
  Phil_TxMessage.RTR = CAN_RTR_DATA;
  Phil_TxMessage.IDE = CAN_ID_STD;
  Phil_TxMessage.DLC = 8;
	
	CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);
}

/**
  * @brief  Configures the NVIC for CAN.
  * @param  None
  * @retval None
  */
void CAN_NVIC_Config(void)
{
  NVIC_InitTypeDef  NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
//#ifndef STM32F10X_CL
//#ifdef  __CAN1_USED__
//  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
//#else  /*__CAN2_USED__*/
//  /* CAN2 is not implemented in the device */
//   #error "CAN2 is implemented only in Connectivity line devices"
//#endif /*__CAN1_USED__*/
//#else
//#ifdef  __CAN1_USED__ 
//  NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
//#else  /*__CAN2_USED__*/
  NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
//#endif /*__CAN1_USED__*/

//#endif /* STM32F10X_CL*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	
}

/**
  * @brief  Initializes a Rx Message.
  * @param  CanRxMsg *RxMessage
  * @retval None
  */
void Init_RxMes(CanRxMsg *RxMessage)
{
  uint8_t i = 0;

  RxMessage->StdId = 0x00;
  RxMessage->ExtId = 0x00;
  RxMessage->IDE = CAN_ID_STD;
  RxMessage->DLC = 0;
  RxMessage->FMI = 0;
  for (i=0; i<8; i++)
  {
    RxMessage->Data[i] = 0x00;
  }
}

void PhilCANSend(uint8_t setID, uint8_t* data, uint8_t length)
{
	uint8_t message_length;
	uint8_t i;
	
	if (length > 8) {
		message_length = 8;
	} else {
		message_length = length;
	}
	
	Phil_TxMessage.StdId = 0x321 + setID;
	for (i=0; i<message_length; i++) {
		Phil_TxMessage.Data[i] = data[i];
	}
//	Phil_TxMessage.Data[0] = data[0];
//	Phil_TxMessage.Data[1] = data[0];
	CAN_Transmit(CANx, &Phil_TxMessage);
}
