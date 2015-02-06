#include "phil_aux.h"
#include <string.h>
#include <ctype.h>

char dataFromSlaveBoard[LENGTH_OF_RESPONSE];

const uint16_t sizeOfGlobalArrays = 2*500;

uint8_t pulseValues[sizeOfGlobalArrays];
uint8_t coordinates[sizeOfGlobalArrays];
uint8_t times[sizeOfGlobalArrays];

uint8_t TRAJ_RECEIVED = 0;

uint8_t WhatToDo(const char *command, uint8_t* phraseToSlave, uint16_t* setID)
{
	uint16_t coord;
	uint8_t motorID;
	uint8_t steps2mm;
	
	uint16_t newPulseWidth;
	uint16_t newPulsePeriod;
	
	uint8_t getTrajectory;
	
	*setID = SearchValue(command, "_setID=");
	
	if (!strncmp(command, "move_motor", 10)) {
		phraseToSlave[3] = MOVE;

		motorID   		= SearchValue(command, "move_motor");
		coord     		= SearchValue(command, "tocoord=");
		steps2mm  		= SearchValue(command, "steps2mm=");
		getTrajectory = SearchValue(command, "getTrajectory=");
		
		phraseToSlave[1] = coord>>8;
		phraseToSlave[0] = coord - (phraseToSlave[1]<<8);
		phraseToSlave[2] = motorID;
		phraseToSlave[4] = steps2mm;
		phraseToSlave[5] = getTrajectory;
	}
	else 
	if (!strncmp(command, "getc_motor", 10)) {
		phraseToSlave[3] = GET_COORDINATE;
		
		motorID   = SearchValue(command, "getc_motor");
		steps2mm  = SearchValue(command, "steps2mm=");
		
		phraseToSlave[2] = motorID;
		phraseToSlave[4] = steps2mm;
	}
	else
	if (!strncmp(command, "reset_motor", 11)) {
		phraseToSlave[3] = RESET_ONE;
		
		motorID   = SearchValue(command, "reset_motor");
		steps2mm  = SearchValue(command, "steps2mm=");
		
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
		
		motorID   = SearchValue(command, "test_motor");
		
		phraseToSlave[2] = motorID;
	}
	else
	if (!strncmp(command, "test_pulses", 11)) {
		phraseToSlave[3] = TEST_OSCILLOSCOPE;
	}
	else
	if (!strncmp(command, "set_pulses:", 11)) {
		phraseToSlave[3] = SET_PULSES;
		
		motorID = SearchValue(command, "motorID=");
		newPulseWidth = SearchValue(command, "w=");
		newPulsePeriod = SearchValue(command, "T=");
		
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
		for(j=5; j<8; j++) {
			if (res != RxMessageData[j]) {
				res = UNKNOWN;
				break;
			}
		}
	} else {
		for(j=5; j<8; j++) {
			if (res != RxMessageData[j]) {
				res = UNKNOWN;
				break;
			}
		}
	}
	return res;
}

void AccumulateArray(can_flag f, uint8_t* RxMessageData)
{
	static uint16_t startIndex = 0;
	uint8_t* array = 0;
	uint16_t i = 0;
	
	switch (f) {
		case FINISH:
			startIndex = 0;
			return;
		case TIME:
			if (startIndex == 0)
				startIndex = TIME_START_INDEX;
			array = times;
			break;
		case U_SIGNAL:
			if (startIndex == 0)
				startIndex = USIGNAL_START_INDEX;
			array = pulseValues;
			break;
		case COOORDINATES:
			if (startIndex == 0)
				startIndex = COORDS_START_INDEX;
			array = coordinates;
			break;
		default:
			break;
	}
	
	if (array != 0)
		for(i=0; i<8; i++)
			if (startIndex < sizeOfGlobalArrays)
				array[startIndex++] = RxMessageData[i];	
}


void SendTrajectory(void)
{
	strcpy(times, "times");
	strcpy(pulseValues, "usignal");
	strcpy(coordinates, "coords");
	SendTrajectoryToComp(times, pulseValues, coordinates, sizeOfGlobalArrays);
}




