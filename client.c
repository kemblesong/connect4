/*********************************************
 * client.c
 * COMP30023: Computer Systems
 * Project 2
 * Kemble Song (584999) (kembles)
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "client.h"


int main(int argc, char ** argv)
{
	int port;
	int sock = -1;
	struct sockaddr_in address;
	struct hostent * host;


	/* checking commandline parameter */
	if (argc != 3)
	{
		printf("usage: %s hostname port\n", argv[0]);
		return -1;
	}

	/* obtain port number */
	if (sscanf(argv[2], "%d", &port) <= 0)
	{
		fprintf(stderr, "%s: error: wrong parameter: port\n", argv[0]);
		return -2;
	}

	/* create socket */
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock <= 0)
	{
		fprintf(stderr, "%s: error: cannot create socket\n", argv[0]);
		return -3;
	}

	/* connect to server */
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	host = gethostbyname(argv[1]);
	if (!host)
	{
		fprintf(stderr, "%s: error: unknown host %s\n", argv[0], argv[1]);
		return -4;
	}
	memcpy(&address.sin_addr, host->h_addr_list[0], host->h_length);
	if (connect(sock, (struct sockaddr *)&address, sizeof(address)))
	{
		fprintf(stderr, "%s: error: cannot connect to host %s\n", argv[0], argv[1]);
		return -5;
	}

	int valid = 0;
	int move;
	int done = 0;
	/* play game with server */
	read_message(sock); /* read welcome message */
	read_message(sock); /* prints the board */
	/* MAIN LOOP */
	while (!done) {
		/* Your move */
		valid = 0;
		read_message(sock); /* Enter column number */
		while (!valid) {
			scanf("%d", &move);
			write(sock, &move, sizeof(int)); /* send move to server */
			read_message(sock);
			read(sock, &valid, sizeof(int)); /* check if valid */
		}
		read_message(sock); /* prints the board */
		read_message(sock);
		read(sock, &done, sizeof(int)); /* check for end game */
		if (done) break;
		/* Computer's move */
		read_message(sock);
		read_message(sock); /* prints the board */
		read_message(sock);
		read(sock, &done, sizeof(int)); /* check for end game */
	}

	/* close socket */
	close(sock);

	return 0;
}
