#include "stm32f10x.h"

static const uint16_t LENGTH_OF_RESPONSE = 30;

extern uint8_t dataFromSlaveBoardReceived;
extern char dataFromSlaveBoard[LENGTH_OF_RESPONSE];

typedef enum {NOTHING, MOVE, RESET_ONE, RESET_ALL, GET_COORDINATE, TEST, TEST_OSCILLOSCOPE, SET_PULSES} commands_t;
typedef enum {SINGLE_COORDINALTE = 8, TIME, U_SIGNAL, COOORDINATES, FINISH, UNKNOWN} can_flag;

uint8_t WhatToDo(const char *command, uint8_t* phraseToSlave, uint16_t* setID);
uint16_t SearchValue(const char* whereToFind, char* whatToFind);
uint16_t AddDigit(uint16_t acc, char digit);

uint8_t SendCoordinateCommandReceived(const char *command);

void SerialPutChar(USART_TypeDef* USARTx, uint8_t c);
void SerialPutString(USART_TypeDef* USARTx, uint8_t *s);

void InitResponce(void);

/*-----------------------------------------------------------------------------------*/
uint16_t SendDataToComp(uint8_t* message, uint16_t len);

can_flag GetTypeOfCANData(uint8_t *RxMessageData);