#include "stm32f10x.h"

#define TIME_START_INDEX 5+1
#define USIGNAL_START_INDEX 7+1
#define COORDS_START_INDEX 6+1

static const uint16_t LENGTH_OF_RESPONSE = 50;

extern uint8_t dataFromSlaveBoardReceived;
extern char dataFromSlaveBoard[LENGTH_OF_RESPONSE];
extern uint8_t TRAJ_RECEIVED;

typedef enum {NOTHING, MOVE, RESET_ONE, RESET_ALL, GET_COORDINATE, TEST, TEST_OSCILLOSCOPE, SET_PULSES} commands_t;
typedef enum {SINGLE_COORDINALTE = 8,
							TIME_START,
							TIME,
							U_SIGNAL_START,
							U_SIGNAL,
							COOORDINATES_START,
							COOORDINATES,
							FINISH,
							TRAJECTORY_TRANSMITTED,
							UNKNOWN
						} can_flag;

uint8_t WhatToDo(const char *command, uint8_t* phraseToSlave, uint16_t* setID);
uint16_t SearchValue(const char* whereToFind, char* whatToFind);
uint16_t AddDigit(uint16_t acc, char digit);

uint8_t SendCoordinateCommandReceived(const char *command);

void SerialPutChar(USART_TypeDef* USARTx, uint8_t c);
void SerialPutString(USART_TypeDef* USARTx, uint8_t *s);

void InitResponce(void);
void WriteResponce(uint8_t* RxMessageData);

/*-----------------------------------------------------------------------------------*/
uint16_t SendDataToComp(uint8_t* message, uint16_t len);
void SendTrajectory(void);
void SendTrajectoryToComp(uint8_t setID, uint8_t motorID,
													uint16_t destination,
													uint8_t *t, uint8_t *u, uint8_t *x, uint16_t len);

can_flag GetTypeOfCANData(uint8_t *RxMessageData);
void AccumulateArray(can_flag f, uint8_t* RxMessageData);
void ResetTrajectoryData(void);

