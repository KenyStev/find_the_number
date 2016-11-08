#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include "util.c"

#define SHMSZ     128

int shmid;
key_t key;
char *shm, *s;
char *argU[64];

typedef struct match
{
	int turn;
	int num;
	pid_t player1;
	pid_t player2;
	pthread_t *thread;
} match;

int cant_players=0;
int *players[1024];

void add_player(char**argU);
void create_match();
void get_command(char*command,int base,int recieved_num);
void* playing(void*);

int main(int argc, char *argv[])
{
	srand(getpid());
	key = 111111111111111;
	printf("My pid: %d\n", key);
	create_shm(&shmid,&shm,key);
	char *line = malloc(sizeof(char)*SHMSZ);

	while(1)
	{
		while(*shm == ' ')sleep(1);
		read_shm(line,shm);
		printf("Recieve: %s\n", line);
		*shm = ' ';
		parse(line,argU);

		if(strcmp(argU[0],"new") == 0)
		{
			add_player(argU);
			
			if(cant_players>0 && cant_players%2==0)
			{
				create_match(players[cant_players-1],players[cant_players-2]);
			}
		}else if(strcmp(argU[0],"exit") == 0)
		{
			free_shm(shm,shmid);
			break;
		}
	}

	return 0;
}

void add_player(char**argU)
{
	players[cant_players++] = atoi(argU[1]);
}

void create_match(pid_t player1, pid_t player2)
{
	struct match *x = (struct match *)malloc(sizeof(struct match));
	pthread_t *thread = malloc(sizeof(pthread_t));

	x->turn=0;
	x->player1=player1;
	x->player2=player2;
	x->num = get_random(1,MAX_NUM);
	x->thread = thread;

	printf("Num to find: %d\n",x->num);

	pthread_create(thread, NULL, playing, (void*) x);
}

void* playing(void*match)
{
	int recieved_num;
	int shmid_p1,shmid_p2;
	pid_t c_player;
	char *shm_p1,*shm_p2, *c_turn;
	char *command = "nada";
	char *args[10];
	char *line = malloc(128);

	struct match *x = (struct match *)match;
	locate_shm(&shmid_p1,&shm_p1,x->player1);
	locate_shm(&shmid_p2,&shm_p2,x->player2);

	command = "ready";
	write_command_in_shm(command,shm_p1);
	write_command_in_shm(command,shm_p2);

	while(1)
	{
		if(x->turn==0)
		{
			c_turn = shm_p1;
			c_player = x->player1;
		}else{
			c_turn = shm_p2;
			c_player = x->player2;
		}
		sleep(1);
		while(*c_turn == ' ')sleep(1);
		read_shm(line,c_turn);
		printf("%d sends to server: %s\n", c_player, line);
		parse(line,args);

		if(strcmp(args[0],"num") == 0)
			recieved_num = atoi(args[1]);
		if(x->num==recieved_num)
			command="win";
		else if(recieved_num>x->num)
			command="down";
		else
			command="up";
		write_command_in_shm(command,c_turn);

		if(strcmp(command,"win") == 0)
		{
			printf("Win: %d\n", (x->turn==0)?x->player1:x->player2);
			sleep(3);
			break;
		}

		x->turn = 1 - x->turn;
		sleep(2);
	}

	command = "kill";
	write_command_in_shm(command,shm_p1);
	write_command_in_shm(command,shm_p2);
	sleep(2);

	free(line);
	free(x);

	free_shm(shm_p1,shmid_p1);
	free_shm(shm_p2,shmid_p2);
	pthread_cancel(*x->thread);
	printf("Bye bye\n");
	return NULL;
}

void get_command(char*command,int base,int recieved_num)
{
	if(base==recieved_num)
		command="win";
	else if(recieved_num>base)
		command="down";
	else
		command="up";
}