#include "phil_aux.h"
#include <string.h>
#include <ctype.h>

uint8_t WhatToDo(const char *command, char* phraseToSlave)
{
	uint8_t i;
	uint16_t coord;
	uint8_t motorID;
	uint8_t steps2mm;
	
	uint16_t newPulseWidth;
	uint16_t newPulsePeriod;
	uint8_t beginningOfData;
	
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
		
		// assumption that command is look like:
		// "set_pulses:w=123;T=456"
		//  0123456789012345678901  <-- numering
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

uint8_t SendCoordinateCommandReceived(const char *command)
{
	if (!strncmp(command, "Get_coordinate", 14)) {
		return 1;
	}
	return 0;
}
