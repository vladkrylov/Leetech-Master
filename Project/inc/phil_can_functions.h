#ifndef __PHIL_CAN_FUNCTIONS_H
#define __PHIL_CAN_FUNCTIONS_H

#include "stm32f10x.h"

/* Private function prototypes -----------------------------------------------*/
void CAN_NVIC_Config(void);
void CAN_Config(void);
void LED_Display(uint8_t Ledstatus);
void Init_RxMes(CanRxMsg *RxMessage);

void PhilCANSend(uint8_t setID, uint8_t* data, uint8_t length);

#endif /* __PHIL_CAN_FUNCTIONS_H */

