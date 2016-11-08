#include <stdlib.h>

#define SHMSZ     128
#define key_p 111111111111111
#define MAX_NUM 100
struct sigaction sigchld_action;

void free_shm(char *shm, int shmid)
{
     shmdt(shm);
     shmctl(shmid, IPC_RMID, NULL);
}

void free_shm_dt(char *shm)
{
     shmdt(shm);
}

int  parse(char *line, char **argU)
{
     int cant=0;
     while (*line != '\0') {       /* if not the end of line ....... */ 
          while (*line == ' ' || *line == '\t' || *line == '\n')
          {
                    *line++ = '\0';     /* replace white spaces with 0    */
                    cant++;
          }
          *argU++ = line;          /* save the argvent position     */
          while (*line != '\0' && *line != ' ' && 
                 *line != '\t' && *line != '\n') 
               line++;             /* skip the argvent until ...    */
     }
    *argU = '\0';                 /* mark the end of argvent list  */
     cant++;
     return cant;
}

void locate_shm(int *shmid,char **shm, key_t key)
{
     /*
     * Locate the segment.
     */
     if ((*shmid = shmget(key, SHMSZ, 0666)) < 0) {
          perror("shmget");
          exit(1);
     }

     /*
     * Now we attach the segment to our data space.
     */
     if ((*shm = shmat(*shmid, NULL, 0)) == (char *) -1) {
          perror("shmat");
          exit(1);
     }
}

void create_shm(int *shmid,char **shm, key_t key)
{
     /*
     * Create the segment.
     */
     if ((*shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
          perror("shmget");
          exit(1);
     }

     /*
     * Now we attach the segment to our data space.
     */
     if ((*shm = shmat(*shmid, NULL, 0)) == (char *) -1) {
          perror("shmat");
          exit(1);
     }

     **shm = ' ';
}

void write_command_in_shm(char *line,char *shm)
{
     printf("send: %s\n", line);
     char* send = line;
     char *s = shm;
     while(*send!=0)
          *s++ = *send++;
     *s = '\0';
}

void read_shm(char *line,char *shm)
{
     int i=0;
     char *s = shm;
     while((*s) != 0)
          *line++ = *s++;
     *line = '\0';
}

int get_random(int min, int max)
{
     double scaled = (double)rand()/RAND_MAX;

     return (max - min + 1)*scaled + min;
}