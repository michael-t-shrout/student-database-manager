/*
clean.c //A writer
========
Purpose:
Lets a student advisor save the contents of the shared memory to 
a file in a format that is suitable for loading into memory later 
and also deletes the shared memory segments and the semaphores created
for synchronization.

To execute:
if called with an argument:
usage(linux): ./Clean <Filename>
otherwise <Filename> defaults to "Sampledatafile.txt"
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include "header.h"

int main(int argc, char* argv[])
{
  int i,id,rwid;
  struct StudentList *infoptr;
  struct ReadCount *read_count;
  int sema_set;
  FILE *outfile;
  char *mode = "w";
  char filename[40] = "Sampledatafile.txt";
  char user_input[3];
  int size = 0;
  
  //validate input arguments
  if (argc == 2){
	strcpy(filename,argv[1]);
    printf("Save data to database file: %s? (Y or N):",filename);
	scanf("%s", user_input);
	if (strcmp(user_input, "Y")){
	  strcpy(filename,"Sampledatafile.txt");
	  printf("Writing to default database file: %s...\n",filename);
	}
	else printf("Writing to user-designated database file: %s...\n",filename);
  }
  else{
    printf("Writing to default database file: %s...\nTo use a different database file enter a file name as argument\nusage(linux): ./Clean <Filename>\n",filename);
  }

  //Attempt to open file
  outfile = fopen(filename,mode);
  if (outfile == NULL) {
    fprintf(stderr, "Unable to open database file\n");
    exit(1);
  }
  
  //Get the ids of the shared memory segments created in Load.c
/* get the id of the shared memory segment with key "KEY" */
/* note that this is the segment where the data is stored */
  id = shmget(KEY,SEGSIZE, 0);
  if (id <0){
    perror("clean: database shmget failed");
    exit(1);
  }
  rwid = shmget(RWKEY,RWSEGSIZE,0);
  if (rwid <0) {
	perror("clean: readers shmget failed");
	exit(1);
  }

//Attach the shared memory segments;
/* attach the already created shared memory segment to infoptr so the
   shared memory segment can be accessed through 'infoptr'
   */
  infoptr=(struct StudentList *)shmat(id,0,0);
  if (infoptr <= (struct StudentList *) (0)) {
    perror("clean: database shmat failed");
    exit(2);
  }
  read_count=(struct ReadCount *)shmat(rwid,0,0);/*attach the shared memory segment 
						to the process's address space */
  if (read_count <= (struct ReadCount *) (0)) {
     perror("clean: readers shmat failed");
	 exit(2);
  }

  /* get the id of the semaphore set associated with SEMA_KEY */ 
  sema_set=semget(SEMA_KEY, 0,0); 
  if ((sema_set < 0) ){
    perror("semget failed");
    exit(2);
  }
  
  Wait(sema_set, 0); // assuming semaset is the id of the semaphore set 
  
  //write the contents of the shared memory to file in the format of the input file;
  size = infoptr->size;
  for (i=0;i<size;i++){
    fprintf(outfile, "%s\n",(infoptr->Student[i]).Name);
	fprintf(outfile, "%s\n",(infoptr->Student[i]).SID);
	fprintf(outfile, "%s\n",(infoptr->Student[i]).address);
	fprintf(outfile, "%s\n",(infoptr->Student[i]).telNumber);
  }
  
  fclose(outfile);
  

//delete the shared memory segments
  shmdt((char  *)infoptr); /* detach the shared memory segments */
  shmdt((char  *)read_count);
  shmctl(id, IPC_RMID,(struct shmid_ds *)0); /* destroy the shared memory segments*/
  shmctl(rwid, IPC_RMID,(struct shmid_ds *)0);
  
  printf("Sleeping 10s for debug and testing...\n");
  sleep(10); // for debugging and testing
  
  Signal(sema_set, 0);
  
//delete the semaphores
  semctl(sema_set,0,IPC_RMID); /*Remove the semaphore set */
  printf("...Data saved.\n");
  
  exit(0);
}