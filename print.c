/*
print.c //A reader
========
Purpose:
Prints the contents of the shared memory to the screen with 
proper formatting.

To execute:
usage(linux): ./Print
No arguments accepted
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <pwd.h>
#include <ctype.h>
#include <unistd.h>
#include "header.h"

int main()
{
  int i,id,rwid;
  struct StudentList *infoptr;
  struct ReadCount *read_count;
  int sema_set;

  //Get the ids of the shared memory segments created in Load.c
/* get the id of the shared memory segment with key "KEY" */
/* note that this is the segment where the data is stored */
  id = shmget(KEY,SEGSIZE, 0);
  if (id <0){
    perror("print: database shmget failed 1");
    exit(1);
  }
  rwid = shmget(RWKEY,RWSEGSIZE,0);
  if (rwid <0) {
	perror("print: readers shmget failed");
	exit(1);
  }

//Attach the shared memory segments;
/* attach the already created shared memory segment to infoptr so the
   shared memory segment can be accessed through 'infoptr'
   */
  infoptr=(struct StudentList *)shmat(id,0,0);
  if (infoptr <= (struct StudentList *) (0)) {
    perror("print: database shmat failed");
    exit(2);
  }
  read_count=(struct ReadCount *)shmat(rwid,0,0);/*attach the shared memory segment 
						to the process's address space */
  if (read_count <= (struct ReadCount *) (0)) {
     perror("print: readers shmat failed");
	 exit(2);
  }
/* get the id of the semaphore set associated with SEMA_KEY */ 
  sema_set=semget(SEMA_KEY, 0,0); 
  if ((sema_set < 0) ){
    perror("semget failed");
    exit(2);
  } 

  Wait(sema_set,1);
  
//increment read_count in shared memory
  read_count->readers++;
 
//if value of read_count==1 then Wait(semaset, 0);
  if((read_count->readers) == 1){
    Wait(sema_set,0);
  }
  Signal(sema_set,1);  

//Print full database to screen
  int size = infoptr->size;
  for(i=0;i<size;i++){
	printf("--------------------------------------\n");
    printf("Name: %s\nSID: %s\nAddress: %s\nPhone Number: %s\n",
	   (infoptr->Student[i]).Name,(infoptr->Student[i]).SID,(infoptr->Student[i]).address,(infoptr->Student[i]).telNumber);
  }
  
  printf("Sleeping 10s for debug and testing...\n");
  sleep(10); // for debugging and testing
  
  Wait(sema_set,1);
//decrement read_count in shared memory
  read_count->readers--;

//if value of read_count==0 then Signal(semaset, 0);
  if((read_count->readers) == 0){
    Signal(sema_set,0);
  }
  Signal(sema_set,1);  

  exit(0);
}