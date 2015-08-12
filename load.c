/*
load.c //A writer
========
Purpose:
Loads the database. It reads the initial database from a file 
and load it into shared memory.

To execute:
if called with an argument:
usage(linux): ./Load <Filename>
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
  FILE *infile;
  char *mode = "r";
  char filename[40] = "Sampledatafile.txt";
  char user_input[3];
  char filedata[256];
  int length = 0;
  
  //validate input arguments
  if (argc == 2){
	strcpy(filename,argv[1]);
    printf("Load database file: %s? (Y or N):",filename);
	scanf("%s", user_input);
	if (strcmp(user_input, "Y")){
	  strcpy(filename,"Sampledatafile.txt");
	  printf("Loading default database file: %s...\n",filename);
	}
	else printf("Loading user-designated database file: %s...\n",filename);
  }
  else{
    printf("Loading default database file: %s...\nTo use a different database file enter a file name as argument\nusage(linux): ./Load <Filename>\n",filename);
  }
  
  //Attempt to open file
  infile = fopen(filename,mode);
  if (infile == NULL) {
  fprintf(stderr, "Unable to open database file\n");
  exit(1);
}
  //Create a shared memory segment to store the students records ;
  id = shmget(KEY,SEGSIZE,IPC_CREAT|0666);/* get shared memory to store data*/
  if (id <0){
    perror("load: database shmget failed");
    exit(1);
  }
  
  //Create a shared memory segment to store the shared variable read_count;
  rwid = shmget(RWKEY,RWSEGSIZE,IPC_CREAT|0666);
  if (rwid <0) {
	perror("load: readers shmget failed");
	exit(1);
  }
  

//Create a semaphore set consisting of enough semaphores (actually 2 will do);
  sema_set = GetSemaphs(SEMA_KEY, NUM_SEMAPHS); /* get a set of NUM_SEMAPHS
						   semaphores*/
  if ((sema_set < 0) ){
    perror("load: semget failed");
    exit(2);
  }


  //Attach the two shared memory segments created;  
  infoptr=(struct StudentList *)shmat(id,0,0);/*attach the shared memory segment
				       to the process's address space */
  if (infoptr <= (struct StudentList *) (0)) {
    perror("load: database shmat failed");
    exit(2);
  }
  read_count=(struct ReadCount *)shmat(rwid,0,0);/*attach the shared memory segment 
						to the process's address space */
  if (read_count <= (struct ReadCount *) (0)) {
    perror("load: readers shmat failed");
	exit(2);
  }
//Set initial read_count to 0
  read_count->readers = 0;

//Wait for semaphore set
  Wait(sema_set, 0);
  
//Initialize read_count to 0 (the shared memory allocated for storing this var);
  read_count->readers = 0;
  


//Load the shared memory segment with data from the file;
  i = 0;
  length = 0;
  while (fgets(filedata,sizeof(filedata),infile)!= NULL){
    strtok(filedata, "\r\t\n");
	if (i<20){
		if (((length + 1) % 4) == 1){
		  strcpy((infoptr->Student[i]).Name,filedata);
		  length++;
		}
		else if (((length + 1) % 4) == 2){
		  strcpy((infoptr->Student[i]).SID,filedata);
		  length++;
		}
		else if (((length + 1) % 4) == 3){
		  strcpy((infoptr->Student[i]).address,filedata);
		  length++;
		}
		else if (((length + 1) % 4) == 0){
		  strcpy((infoptr->Student[i]).telNumber,filedata);
		  length++;
		  i++;
		}
	}		
  }
  
  //set size of database
  infoptr->size = i;
	
  fclose(infile);  
  
  printf("Sleeping 10s for debug and testing...\n");  
  sleep(10); // for debugging and testing
  
  Signal(sema_set, 0);
  
  printf("...Loading Complete.\n");
  
  exit(0);
}