#include "phil_aux.h"
#include <string.h>

uint8_t WhatToDo(const char *command, char* phraseToSlave)
{
//	if (command[0] == 'm' &&
//			command[1] == 'o' &&
//			command[2] == 'v' &&
//			command[3] == 'e'
//		) {
//			return 1;
//		}
//		return 0;
	uint16_t coord;
	uint8_t motorID;
	uint8_t steps2mm;
	
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
	else {
		return 1;
	}
	return 0;
}
