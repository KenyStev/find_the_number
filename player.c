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
#include <stdio.h>
#include "util.c"

#define SHMSZ     128

pid_t pid_server;

int num, max_num=MAX_NUM, min_num=1;
int shmid, shmid_parent;
key_t key, key_parent = 111111111111111;
char *shm, *s, *shm_parent;
char *my_pid;
char line[1024];

void get_new_number(int*,char*command, int min, int max);

int main(int argc, char *argv[])
{
	key = getpid();
	srand(key);
	create_shm(&shmid,&shm,key);

	my_pid = malloc(10);
	sprintf(my_pid,"%d",key);
	
	locate_shm(&shmid_parent,&shm_parent,key_parent);
	char command[100] = "new ";
	strcat(command,my_pid);
	write_command_in_shm(command,shm_parent);


	while(*shm == ' ')
	{
		sleep(1);
		read_shm(line,shm);
		if(strcmp(line,"ready")==0) break;
	}
	printf("%d recieve: %s\n", key,line);
	*shm=' ';
	get_new_number(&num,command,1,MAX_NUM);
	write_command_in_shm(command,shm);
	while(1)
	{
		// while(*shm==' ')sleep(1);
		read_shm(line,shm);
		strcpy(command," ");
		if(strcmp(line,"down") == 0)
		{
			*shm=' ';
			max_num=num;
			printf("%d recieve: %s\n", key,line);
			get_new_number(&num,command,min_num,max_num);
			write_command_in_shm(command,shm);
		}else if(strcmp(line,"up") == 0)
		{
			*shm=' ';
			min_num=num;
			printf("%d recieve: %s\n", key,line);
			get_new_number(&num,command,min_num,max_num);
			write_command_in_shm(command,shm);
		}else if(strcmp(line,"win") == 0)
		{
			*shm=' ';
			printf("I won!\n");
		}else if(strcmp(line,"kill") == 0)
		{
			printf("%d recieve: %s\n", key,line);
			break;
		}
		
		sleep(2);
	}

	free_shm_dt(shm_parent);
	free_shm(shm,shmid);
	printf("%d: says Bye bye!\n", key);
	return 0;
}

void get_new_number(int* num,char*command, int min, int max)
{
	*num = get_random(min, max);
	sprintf(command,"num %d",*num);
}