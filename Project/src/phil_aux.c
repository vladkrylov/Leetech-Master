#include "phil_aux.h"
#include <string.h>
#include <ctype.h>

char dataFromSlaveBoard[LENGTH_OF_RESPONSE];

const uint16_t sizeOfGlobalArrays = 200;
uint16_t pulseValues[sizeOfGlobalArrays];
uint16_t coordinates[sizeOfGlobalArrays];
uint16_t times[sizeOfGlobalArrays];

uint8_t WhatToDo(const char *command, uint8_t* phraseToSlave, uint16_t* setID)
{
	uint8_t i;
	uint16_t coord;
	uint8_t motorID;
	uint8_t steps2mm;
	
	uint16_t newPulseWidth;
	uint16_t newPulsePeriod;
	uint8_t beginningOfData;
	
	*setID = SearchValue(command, "_setID=");
	
	if (!strncmp(command, "move_motor", 10)) {
		phraseToSlave[3] = MOVE;
				coord = 10000*((uint8_t)command[12] - (uint8_t)'0') + 
				1000*((uint8_t)command[13] - (uint8_t)'0') + 
				100*((uint8_t)command[14] - (uint8_t)'0') + 
				10*((uint8_t)command[15] - (uint8_t)'0') + 
				1*((uint8_t)command[16] - (uint8_t)'0');
		motorID = (uint8_t)command[10] - (uint8_t)'0';
		steps2mm = (uint8_t)command[28] - (uint8_t)'0';
		
		phraseToSlave[1] = coord>>8;
		phraseToSlave[0] = coord - (phraseToSlave[1]<<8);
		phraseToSlave[2] = motorID;
		phraseToSlave[4] = steps2mm;
	}
	else 
	if (!strncmp(command, "getc_motor", 10)) {
		phraseToSlave[3] = GET_COORDINATE;
		
		motorID = (uint8_t)command[10] - (uint8_t)'0';
		steps2mm = (uint8_t)command[21] - (uint8_t)'0';
		
		phraseToSlave[2] = motorID;
		phraseToSlave[4] = steps2mm;
	}
	else
	if (!strncmp(command, "reset_motor", 11)) {
		phraseToSlave[3] = RESET_ONE;
		
		motorID = (uint8_t)command[11] - (uint8_t)'0';
		steps2mm = (uint8_t)command[22] - (uint8_t)'0';
		
		phraseToSlave[2] = motorID;
		phraseToSlave[4] = steps2mm;
	}
	else 
	if (!strncmp(command, "reset_all", 9)) {
		phraseToSlave[3] = RESET_ALL;
	}
	else
	if (!strncmp(command, "test_motor", 10)) {
		phraseToSlave[3] = TEST;
		
		motorID = (uint8_t)command[10] - (uint8_t)'0';
		phraseToSlave[2] = motorID;
	}
	else
	if (!strncmp(command, "test_pulses", 11)) {
		phraseToSlave[3] = TEST_OSCILLOSCOPE;
	}
	else
	if (!strncmp(command, "set_pulses:", 11)) {
		phraseToSlave[3] = SET_PULSES;
		
		// assumption that command looks like:
		// "set_pulses:w=123;T=456"
		//  0123456789012345678901  <-- indexes of string above
		beginningOfData = 13;
		newPulseWidth = 0;
		i = 0;
		while (1) {
			if (isdigit(command[beginningOfData+i])) {
				newPulseWidth = newPulseWidth*10 + (uint8_t)command[beginningOfData+i] - (uint8_t)'0';
				i++;
			} else break;
		}
		
		newPulsePeriod = 0;
		while (1) {
			if (isdigit(command[beginningOfData+i+3])) {
				newPulsePeriod = newPulsePeriod*10 + (uint8_t)command[beginningOfData+i+3] - (uint8_t)'0';
				i++;
			} else break;
		}
		
		motorID = SearchValue(command, "_motorID=");
		phraseToSlave[2] = motorID;
		
		phraseToSlave[5] = newPulseWidth >> 8;
		phraseToSlave[4] = newPulseWidth - (phraseToSlave[5] << 8);
		
		phraseToSlave[7] = newPulsePeriod >> 8;
		phraseToSlave[6] = newPulsePeriod - (phraseToSlave[7] << 8);
	}
	else {
		return 1;
	}
	return 0;
}

uint16_t SearchValue(const char* whereToFind, char* whatToFind)
{
	uint16_t i = 0;
	uint16_t j = 0;
	uint16_t res = 0;
	char ch;
	uint16_t keyLength = strlen(whatToFind);

	while (whereToFind[i]) {
		if (strncmp(whereToFind+i, whatToFind, keyLength) == 0) {
			res = 0;
			ch = whereToFind[i+keyLength + j];
			while (isdigit(ch)) {
				res = AddDigit(res, ch);

				j++;
				ch = whereToFind[i+keyLength + j];
			}
		}
		i++;
	}
	return res;
}

uint16_t AddDigit(uint16_t acc, char digit) 
{
	return acc * 10 + (digit - '0');
}

uint8_t SendCoordinateCommandReceived(const char *command)
{
	if (!strncmp(command, "Get_coordinate", 14))
		return 1;
	else
		return 0;
}

// response array initialization
void InitResponce(void)
{
	uint16_t i;
	strcpy(dataFromSlaveBoard, "response_");
	for(i=9; i<LENGTH_OF_RESPONSE; i++) {
		dataFromSlaveBoard[i] = 0;
	}
}

can_flag GetTypeOfCANData(uint8_t* RxMessageData)
{
	can_flag res = UNKNOWN;
	uint8_t i = 0;
	uint8_t j = 7;
	for(i=SINGLE_COORDINALTE; i<=UNKNOWN; i++) {
		if (RxMessageData[7] == i) {
			res = i;
			break;
		}
	}
	
	if (res == SINGLE_COORDINALTE) {
		for(j=7; j>=5; j--) {
			if (res != RxMessageData[j]) {
				res = UNKNOWN;
				break;
			}
		}
	} else {
		// Check for another flags
	}
	return res;
}

