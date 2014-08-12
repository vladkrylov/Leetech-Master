#include "stm32f10x.h"

extern uint8_t dataFromSlaveBoardReceived;
extern char dataFromSlaveBoard[8];


uint8_t IsMoveCommandReceived(char* command);
