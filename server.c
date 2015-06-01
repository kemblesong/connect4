/*********************************************
 * server.c
 * COMP30023: Computer Systems
 * Project 2
 * Kemble Song (584999) (kembles)
 */



#include <netinet/in.h>
#include <pthread.h>
#include <time.h>
#include "server.h"

void * process(void * ptr)
{
	time_t now;
	char time_stamp[32];
	char message[64];
	connection_t * conn;
	long addr = 0;
	FILE *f;


	if (!ptr) pthread_exit(0);
	conn = (connection_t *)ptr;
	addr = (long)((struct sockaddr_in *)&conn->address)->sin_addr.s_addr;
	/*** START LOG THINGS ***/
	f = fopen("log.txt", "a");
	if (f == NULL) {
		printf("Error opening log file!\n");
		exit(1);
	}
	time(&now);
	strftime(time_stamp, 32, "%d %m %Y %H:%M:%S", localtime(&now));
	fprintf(f, "[%s](%d.%d.%d.%d)(soc_id %d) client connected\n",
					time_stamp,
					(int)((addr      ) & 0xff),
					(int)((addr >>  8) & 0xff),
					(int)((addr >> 16) & 0xff),
					(int)((addr >> 24) & 0xff),
					conn->sock);
	fclose(f);
	/*** END LOG THINGS ***/
	/* game playing logic */
	c4_t board;
	int move;
	int done;
	srand(RSEED);
	init_empty(board);
	strcpy(message, "Welcome to connect-4 \n\n");
	send_message(conn->sock, message);
	print_config(board, conn->sock);

		/* main loop does two moves each iteration, one from the human
		 * playing, and then one from the computer program (or game server)
		 */
		while ((move = get_move(conn->sock,board)) != EOF) {
			/* process the person's move */
			if (do_move(board, move, YELLOW)!=1) {
				strcpy(message, "panic\n");
				send_message(conn->sock, message);
				break;
			}
			/*** START LOG THINGS ***/
			f = fopen("log.txt", "a");
			if (f == NULL) {
		    printf("Error opening log file!\n");
		    exit(1);
			}
			time(&now);
			strftime(time_stamp, 32, "%d %m %Y %H:%M:%S", localtime(&now));
			fprintf(f, "[%s](%d.%d.%d.%d)(soc_id %d) client's move = %d\n",
							time_stamp,
							(int)((addr      ) & 0xff),
							(int)((addr >>  8) & 0xff),
							(int)((addr >> 16) & 0xff),
							(int)((addr >> 24) & 0xff),
							conn->sock,
							move);
			fclose(f);
			/*** END LOG THINGS ***/
			print_config(board, conn->sock);
			/* and did they win??? */
			if (winner_found(board) == YELLOW) {
				/* rats, the person beat us! */
				strcpy(message, "Ok, you beat me, beginner's luck!\n");
				send_message(conn->sock, message);
				break;
			}
			/* was that the last possible move? */
			if (!move_possible(board)) {
				/* yes, looks like it was */
				strcpy(message, "An honourable draw\n");
				send_message(conn->sock, message);
				break;
			}
			/* otherwise, look for a move from the computer */
			strcpy(message, "My turn. Let's see...");
			send_message(conn->sock, message);
			done = 0;
			write(conn->sock, &done, sizeof(int));
			move = suggest_move(board, RED);
			sleep(1);
			/* then play the move */

			strcpy(message, "I play in column ");
			sprintf(eos(message), "%d\n", move);
			send_message(conn->sock, message);

			if (do_move(board, move, RED)!=1) {
				strcpy(message, "panic\n");
				send_message(conn->sock, message);
				break;
			}
			/*** START LOG THINGS ***/
			f = fopen("log.txt", "a");
			if (f == NULL) {
		    printf("Error opening log file!\n");
		    exit(1);
			}
			time(&now);
			strftime(time_stamp, 32, "%d %m %Y %H:%M:%S", localtime(&now));
			fprintf(f, "[%s](0.0.0.0) server's move = %d\n", time_stamp, move);
			fclose(f);
			/*** END LOG THINGS ***/
			print_config(board, conn->sock);
			/* and did we win??? */
			if (winner_found(board) == RED) {
				/* yes!!! */
				strcpy(message, "I guess I have your measure!\n");
				send_message(conn->sock, message);
				break;
			}
			/* otherwise, the game goes on */
			strcpy(message, "Your turn.");
			send_message(conn->sock, message);
			done = 0;
			write(conn->sock, &done, sizeof(int));
		}
		done = 1;
		write(conn->sock, &done, sizeof(int));
		/*** START LOG THINGS ***/
		f = fopen("log.txt", "a");
		if (f == NULL) {
			printf("Error opening log file!\n");
			exit(1);
		}
		time(&now);
		strftime(time_stamp, 32, "%d %m %Y %H:%M:%S", localtime(&now));
		fprintf(f, "[%s](%d.%d.%d.%d)(soc_id %d) game over\n",
						time_stamp,
						(int)((addr      ) & 0xff),
						(int)((addr >>  8) & 0xff),
						(int)((addr >> 16) & 0xff),
						(int)((addr >> 24) & 0xff),
						conn->sock);
		fclose(f);
		/*** END LOG THINGS ***/

	/* close socket and clean up */
	close(conn->sock);
	free(conn);
	pthread_exit(0);
}

void send_message(int destination, char* message)
{
	int len = strlen(message);
	write(destination, &len, sizeof(int));
	write(destination, message, len);
}



int main(int argc, char ** argv)
{
	int sock = -1;
	struct sockaddr_in address;
	int port;
	connection_t * connection;
	pthread_t thread;

	/* check for command line arguments */
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s port\n", argv[0]);
		return -1;
	}

	/* obtain port number */
	if (sscanf(argv[1], "%d", &port) <= 0)
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

	/* bind socket to port */
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	if (bind(sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) < 0)
	{
		fprintf(stderr, "%s: error: cannot bind socket to port %d\n", argv[0], port);
		return -4;
	}

	/* listen on port */
	if (listen(sock, 5) < 0)
	{
		fprintf(stderr, "%s: error: cannot listen on port\n", argv[0]);
		return -5;
	}

	printf("%s: ready and listening\n", argv[0]);

	while (1)
	{
		/* accept incoming connections */
		connection = (connection_t *)malloc(sizeof(connection_t));
		connection->sock = accept(sock, &connection->address, &connection->addr_len);
		if (connection->sock <= 0)
		{
			free(connection);
		}
		else
		{
			/* start a new thread but do not wait for it */
			pthread_create(&thread, 0, process, (void *)connection);
			pthread_detach(thread);
		}
	}

	return 0;
}
