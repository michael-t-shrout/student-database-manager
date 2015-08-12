/* header.h */
/*Taken from given sample code but updated with a few changes:
  -defines a new struct StudentList which is an array for storing up to 20 StudentInfo structs
  -Update SEGSIZE to size of StudentList 
  -defines a new struct ReadCount for keeping track of readers in shared data
  -decrease Number of Semaphores to 2 since only 2 are needed
*/
#define KEY  ((key_t)(50744)) /*change it to last five digits of your SSN*/
#define SEGSIZE  sizeof(struct StudentList)

#define NUM_SEMAPHS 2
#define SEMA_KEY   ((key_t)(0744)) /* change this to last four digits of SSN */

#define RWKEY  ((key_t)(744)) /* last 3 of my SSN */
#define RWSEGSIZE  sizeof(struct ReadCount)

typedef struct StudentInfo{
  char Name[40];
  char SID[40];
  char address[100];
  char telNumber[40];
}Student;

typedef struct StudentList{
  struct StudentInfo Student[20];
  int size;
}infoptr;

typedef struct ReadCount{
   int readers;
}read_count;

void Wait(int semaph, int n);
void Signal(int semaph, int n);
int GetSemaphs(key_t k, int n);