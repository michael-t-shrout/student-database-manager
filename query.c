/*
query.c //A reader
========
Purpose:
Lets anyone query the database. Anyone should be able to 
retrieve the record of any student by typing in the student’s ID.

To execute:
MUST called with an argument signifying the SID to be queried for:
usage(linux): ./Query <SID>
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

int main(int argc, char* argv[])
{
  int i,id,rwid;
  struct StudentList *infoptr;
  struct ReadCount *read_count;
  int sema_set;
  char sid[20];
  
  //validate input arguments and store input ID
  if (argc != 2){
    fprintf(stderr, "Please enter a Student ID to search for as an argument.\nusage(linux): ./Query <SID>\n");
    exit(3);
  } 
  strcpy(sid, argv[1]);
  
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

//No need to Get the id of the student the user wants to query, handled as an argument

  Wait(sema_set,1);
  
//increment read_count in shared memory
  read_count->readers++;
 
//if value of read_count==1 then Wait(semaset, 0);
  if((read_count->readers) == 1){
    Wait(sema_set,0);
  }
  Signal(sema_set,1);
  
//search the shared memory for the record of the student with the
//id, the user typed;
  int recordIndex = -1;
  for(i=0;i<20;i++){
	if (!(strcmp((infoptr->Student[i]).SID, sid))){
	  recordIndex = i;		  
	}	  
  }
//if not found print "record not found" to the screen and release semaphore;
  if (recordIndex == -1){
	fprintf(stderr, "Search completed. Student ID not found\n");
  }
  else{
//if found print the results to the screen;  
  printf("Result: Name: %s\nSID: %s\nAddress: %s\nPhone Number: %s\n",
	infoptr->Student[recordIndex].Name,
	infoptr->Student[recordIndex].SID,
	infoptr->Student[recordIndex].address,
	infoptr->Student[recordIndex].telNumber);
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
};