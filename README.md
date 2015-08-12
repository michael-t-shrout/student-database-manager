# student-database-manager
##An Operating Systems class assignment for a shared memory segment student database manager
###Purpose: 
		Implement several programs for creating and maintaining a Student Database. 
		Once loaded the data will be stored in shared memory, along with 2 semaphores
		and a readers count for managing concurrent access.
		 
###Included in repo: 
		change.c clean.c header.c header.h load.c print.c query.c 
		Sampledatafile.txt README makefile
###To build: 
		type 'make' in the directory containing the above files.
		the following executables will be created: Load Print Query Change Clean
###To execute: 
```
			1. type './Load'  
								->Will create shared memory segments and other constructs. 
								  The database will be populated with data from 'Sampledatafile.txt'
 								  User can specify a different datafile, see "Load" details below.
			
			2. type './Print' 
								->Will print out a list of all database entries.
			
			3. type './Query 111224466'
								->Will print out only data for the entry pertaining to 
								  "Helen Higginbotham" 
								  *This assumes you are using the 'Sampledatafile.txt'
								  User can specify a different SID, see "Query" details below.
			4. type './Change "Example Student" 123456789 "123 America way" 8595551111'
								->Will prompt for password (assumed to be '000')
								  Will then prompt for SID to change. Enter 111224466
								  This will change the entry for Helen Higginbotham include 
								  information entered as arguments. 
								  *This assumes you are using the 'Sampledatafile.txt'
								  User can specify which information to change, see "Change" details below.
			5. type './Clean'
								->Will store current database state into 'Sampledatafile.txt'
								  then delete shared memory segments and semaphores.
								  User can specify a different datafile, see "Clean" details below.
```
Details:
========
###load.c //A writer
**Purpose:**
```
Loads the database. It reads the initial database from a file 
and load it into shared memory.
```
**To execute:**
```
if called with an argument:
usage(linux): ./Load <Filename>
otherwise <Filename> defaults to "Sampledatafile.txt"
```


###print.c //A reader
**Purpose:**
```
Prints the contents of the shared memory to the screen with 
proper formatting.
```
**To execute:**
```
usage(linux): ./Print
No arguments accepted
```


###query.c //A reader
**Purpose:**
```
Lets anyone query the database. Anyone should be able to 
retrieve the record of any student by typing in the student’s ID.
```
**To execute:**
```
MUST called with an argument signifying the SID to be queried for:
usage(linux): ./Query <SID>
```


###change.c //A writer
**Purpose:**
```
Lets a student advisor change the data related to any student with the 
given Student ID. A student advisor is required to have a password to 
change the database. Password is assumed to be “000”,for simplicity.
```
**To execute:**
```
MUST called with 4 additional arguments pertaining to the attributes to be changed, order is specific.
usage(linux): ./Change <Name> <SID> <address> <phone number>
```
**Further details:**
```
If Advisor wishes to only change certain attributes they can elect not to change others, 
for example if a student only needs to update their address:
./Change NOCHANGE NOCHANGE "123 America way" NOCHANGE
Advisor will be prompted for Password and specific student ID 
for which to update entry during program execution.
```


###clean.c //A writer
**Purpose:**
```
Lets a student advisor save the contents of the shared memory to 
a file in a format that is suitable for loading into memory later 
and also deletes the shared memory segments and the semaphores created
for synchronization.
```
**To execute:**
```
if called with an argument:
usage(linux): ./Clean <Filename>
otherwise <Filename> defaults to "Sampledatafile.txt"
```

===
###Name: Michael Tyler Shrout
###Date: 4/20/2015
###Professor: Dr. D. Manivannan
###Programming Language: C
