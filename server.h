/*********************************************
 * server.h
 * COMP30023: Computer Systems
 * Project 2
 * Kemble Song (584999) (kembles)
 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>

/* number of columns in the game */
#define WIDTH		7

/* number of slots in each column */
#define HEIGHT		6

/* number in row required for victory */
#define STRAIGHT	4

/* sign that a cell is still empty */
#define EMPTY		' '

/* the two colours used in the game */
#define RED		'R'
#define YELLOW		'Y'

/* horizontal size of each cell in the display grid */
#define WGRID	5

/* vertical size of each cell in the display grid */
#define HGRID	3

#define RSEED	876545678
/* end of string */
#define eos(s) ((s)+strlen(s))

typedef char c4_t[HEIGHT][WIDTH];

typedef struct
{
int sock;
struct sockaddr address;
int addr_len;
} connection_t;

void send_message(int destination, char* message);
void send_board(int destination, c4_t board);
void send_ack(int destination);
void print_config(c4_t,int);
void init_empty(c4_t);
int do_move(c4_t, int, char);
void undo_move(c4_t, int);
int get_move(int,c4_t);
int move_possible(c4_t);
char winner_found(c4_t);
int rowformed(c4_t,  int r, int c);
int explore(c4_t, int r_fix, int c_fix, int r_off, int c_off);
int suggest_move(c4_t board, char colour);
