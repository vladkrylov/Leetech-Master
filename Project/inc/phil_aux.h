#include "stm32f10x.h"

extern uint8_t dataFromSlaveBoardReceived;
extern char dataFromSlaveBoard[8];

typedef enum {NOTHING, MOVE, RESET_ONE, RESET_ALL, GET_COORDINATE, TEST, TEST_OSCILLOSCOPE, SET_PULSES} commands_t;

uint8_t WhatToDo(const char *command, char* phraseToSlave);
