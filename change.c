/*
change.c //A writer
========
Purpose:
Lets a student advisor change the data related to any student with the 
given Student ID. A student advisor is required to have a password to 
change the database. Password is assumed to be “000”,for simplicity.

To execute:
MUST called with 4 additional arguments pertaining to the attributes to be changed, order is specific.
usage(linux): ./Change <Name> <SID> <address> <phone number>

If Advisor wishes to only change certain attributes they can elect not to change others, 
for example if a student only needs to update their address:
./Change NOCHANGE NOCHANGE "123 America way" NOCHANGE

Advisor will be prompted for Password and specific student ID 
for which to update entry during program execution. 
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
  char NoChange[20] = "NOCHANGE";
  char sid[20];
  char upwd[10];
  
  //validate input arguments
  if (argc != 5){
    fprintf(stderr, "usage(linux): ./Change <Name> <SID> <address> <phone number>\nTo leave attribute unchanged enter <NOCHANGE>\n");
    exit(3);
  } 

//Get the ids of the shared memory segments created in Load.c
/* get the id of the shared memory segment with key "KEY" */
/* note that this is the segment where the data is stored */
  id = shmget(KEY,SEGSIZE, 0);
  if (id <0){
    perror("change: database shmget failed");
    exit(1);
  }
  rwid = shmget(RWKEY,RWSEGSIZE,0);
  if (rwid <0) {
	perror("change: readers shmget failed");
	exit(1);
}

//Attach the shared memory segments;
/* attach the already created shared memory segment to infoptr so the
   shared memory segment can be accessed through 'infoptr'
   */
  infoptr=(struct StudentList *)shmat(id,0,0);
  if (infoptr <= (struct StudentList *) (0)) {
    perror("change: database shmat failed");
    exit(2);
  }
  read_count=(struct ReadCount *)shmat(rwid,0,0);/*attach the shared memory segment 
						to the process's address space */
  if (read_count <= (struct ReadCount *) (0)) {
     perror("change: readers shmat failed");
	 exit(2);
}

/* get the id of the semaphore set associated with SEMA_KEY */ 
  sema_set=semget(SEMA_KEY, 0,0); 
  if ((sema_set < 0) ){
    perror("semget failed");
    exit(2);
  } 
  
//Prompt the user for password;
  printf("Please enter your password: ");
  scanf("%s", upwd);
  
//Validate the password // just check if the user types 000;
  if (strcmp(upwd, "000")){
    fprintf(stderr, "Unrecognized Password. Change aborted\n");
	exit(3);
  }
  
//Get the id of the student, the user wants to modify;
// No need to get the new information about the student, handled as arguments;
  printf("Please enter your Student ID of record to change: ");
  scanf("%s", sid);
  
  Wait(sema_set, 0); // assuming semaset is the id of the semaphore set
  
//Search the shared memory for the student record with
//  the student id typed by the user;
  int recordIndex = -1;
  for(i=0;i<20;i++){
	if (!(strcmp((infoptr->Student[i]).SID, sid)))
	{
		recordIndex = i;		  
	}	  
  }
  //if the id is not found, print an error message;
  if (recordIndex == -1){
	fprintf(stderr, "Search completed. Student ID not found. No Changes made.\n");
  }
  else{
  
    //update the student record with new information;
    if (strcmp(NoChange, argv[1])) 
    strcpy((infoptr->Student[recordIndex]).Name,argv[1]);
    if (strcmp(NoChange, argv[2]))
    strcpy((infoptr->Student[recordIndex]).SID, argv[2]);
    if (strcmp(NoChange, argv[3]))
    strcpy((infoptr->Student[recordIndex]).address, argv[3]);
    if (strcmp(NoChange, argv[4]))
    strcpy((infoptr->Student[recordIndex]).telNumber, argv[4]);
	printf("Record found and updated\n");
  }
  
  printf("Sleeping 10s for debug and testing...\n");
  sleep(10); // for debugging and testing
  
  Signal(sema_set,0); 
  
  exit(0);

}