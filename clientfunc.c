/*********************************************
 * clientfunc.c
 * COMP30023: Computer Systems
 * Project 2
 * Kemble Song (584999) (kembles)
 */

#include "client.h"

void read_message(int source) {
	int len;
	char* buffer;
	read(source, &len, sizeof(int));
	if (len > 0) {
		buffer = (char*)malloc((len+1)*sizeof(char));
		buffer[len] = 0;
		read(source, buffer, len);
		printf("%s\n", buffer);
		free(buffer);
	}
}
