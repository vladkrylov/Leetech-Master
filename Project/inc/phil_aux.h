#include "stm32f10x.h"

#define LENGTH_OF_RESPONSE 18

extern uint8_t dataFromSlaveBoardReceived;
extern char dataFromSlaveBoard[LENGTH_OF_RESPONSE];

typedef enum {NOTHING, MOVE, RESET_ONE, RESET_ALL, GET_COORDINATE, TEST, TEST_OSCILLOSCOPE, SET_PULSES} commands_t;

uint8_t WhatToDo(const char *command, char* phraseToSlave, uint16_t* setID);
uint16_t SearchValue(const char* whereToFind, char* whatToFind);
uint16_t AddDigit(uint16_t acc, char digit);

uint8_t SendCoordinateCommandReceived(const char *command);

void SerialPutChar(USART_TypeDef* USARTx, uint8_t c);
void SerialPutString(USART_TypeDef* USARTx, uint8_t *s);
