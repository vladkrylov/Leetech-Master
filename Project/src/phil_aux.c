#include "phil_aux.h"

uint8_t IsMoveCommandReceived(char* command)
{
	if (command[0] == 'm' &&
			command[1] == 'o' &&
			command[2] == 'v' &&
			command[3] == 'e'
		) {
			return 1;
		}
		return 0;
}
