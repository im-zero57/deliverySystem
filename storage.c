#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"



/* 
  definition of storage cell structure ----
  members :
  int building : building number of the destination
  int room : room number of the destination
  int cnt : number of packages in the cell
  char passwd[] : password setting (4 characters)
  char *context : package context (message string)
*/
typedef struct {
	int building;
	int room;
	int cnt;
	char passwd[PASSWD_LEN+1];
	
	char *context;
} storage_t;


static storage_t** deliverySystem; 			//deliverySystem
static int storedCnt = 0;					//number of cells occupied
int systemSize[2] = {0, 0};  				//row/column of the delivery system
static int masterPassword[PASSWD_LEN+1];	//master password
static int password[PASSWD_LEN+1];			//array for password that person input to find that person's package



// ------- inner functions ---------------

//print the inside context of a specific cell
//int x, int y : cell to print the context
static void printStorageInside(int x, int y) {
	printf("\n------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n");
	if (deliverySystem[x][y].cnt > 0)
		printf("<<<<<<<<<<<<<<<<<<<<<<<< : %s >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", deliverySystem[x][y].context);
	else
		printf("<<<<<<<<<<<<<<<<<<<<<<<< empty >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
		
	printf("------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n\n");
}

//initialize the storage
//set all the member variable as an initial value
//and allocate memory to the context pointer
//int x, int y : cell coordinate to be initialized
static void initStorage(int x, int y) {
	
	int i;									//integer for 'for'
	deliverySystem[x][y].building = 0;		//initialize building
	deliverySystem[x][y].cnt = 0;			//initialize cnt
	for(i=0;i<PASSWD_LEN+1;i++)				//initialize password
	{
		deliverySystem[x][y].passwd[i]= '\0';
	}
	deliverySystem[x][y].room = 0;			//initialize room
	
}

//get password input and check if it is correct for the cell (x,y)
//int x, int y : cell for password check
//return : 0 - password is matching, -1 - password is not matching
static int inputPasswd(int x, int y) {
	
	int i;								//integer for 'for'
	int number_check=0;					//integer to check the password that you input whether that match the real password
	int number_master=0;				//integer to check the password that you input whether that match the master password
 
	printf("input the password : ");	//print the sentence that mean input the password
	scanf("%s",&password);				//scanf the sentence
	
	for(i=0;i<PASSWD_LEN;i++)			//'for' to matck the real password
	{
		if(password[i]==deliverySystem[x][y].passwd[i])
			number_check++;				//if the character is right the password element, number_check is up one by one.
		
	}
	
	if(number_check==4)					//if the number_check is 4, that means the password is right, so return 0
		return 0;
	else								//if the number_chck isn't 4, that means the password isn't right so check the password is master password or is wrong password
		
		for(i=0;i<PASSWD_LEN;i++)		//'for' to match the master password
		{
			if(password[i]==masterPassword[i])
				number_master++;		//if the character is right the masterPassword element, number_master is up one by one.
		}
		
		if(number_master==4)			//if the number_master is 4, that means the password match the master password, so return 0
			return 0;
		else
			return -1;					//that means the password you input is wrong!
}





// ------- API function for main.c file ---------------

//backup the delivery system context to the file system
//char* filepath : filepath and name to write
//return : 0 - backup was successfully done, -1 - failed to backup
int str_backupSystem(char* filepath) {
	FILE *fp;							//define file pointer to write 
	FILE *fp_ch;						//define file pointer to check that the system context is well recorded
	int i, j, k, t;						//define number used "for" 
	fp=fopen(filepath,"wt");			//rewrite the all context 
	
	fprintf(fp,"%d %d\n",systemSize[0] ,systemSize[1]);	//rewrite the system Size
	fprintf(fp,"%s",masterPassword);					//rewrite the masterPassword
	fprintf(fp,"\n");									//to distinguish the delivery element that can change between never changed information

	for(i=0;i<systemSize[0];i++)				//record structure related with delivery
	{
		for(j=0;j<systemSize[1];j++)
		{
			if(deliverySystem[i][j].cnt>0)		//check if that cell has information. if there not information, it need not to record information 
			{
				//record the information
				fprintf(fp, "%d %d %d %d %s %s",i,j,deliverySystem[i][j].building,deliverySystem[i][j].room,deliverySystem[i][j].passwd,deliverySystem[i][j].context);			
				fprintf(fp,"\n");				//to distinguish information
			}
		}
	}
	
	fclose(fp);
	
	fp_ch = fopen(filepath,"r");
	//check the information is recorded
	if(fp_ch==NULL)				//if it is null return -1
		return -1;
	else						//if the file has a information return 0; that means well recorded
		return 0;
		
	fclose(fp_ch);	
}
 

//create delivery system on the double pointer deliverySystem
//char* filepath : filepath and name to read config parameters (row, column, master password, past contexts of the delivery system
//return : 0 - successfully created, -1 - failed to create the system
int str_createSystem(char* filepath) {
	
	FILE *fp;					//define file pointer
	fp = fopen(filepath,"r");	//file pointer that means reading file
	int i,j;					//integer used for 'for'
	char c;						//character that used to check the end of the file
	int row,cell;				//integer to use row and cell
	char pack[MAX_MSG_SIZE+1];	//array to use to get the context temporally
	
	fscanf(fp,"%d",&systemSize[0]);	//scanf the total row that is in the file
	fscanf(fp,"%d",&systemSize[1]);	//scanf the total cell that is in the file
	fscanf(fp,"%s",masterPassword);	//scanf the master password that is in the file
	deliverySystem=(storage_t**)malloc(sizeof(storage_t*)*systemSize[0]);	//make the memory for deliverySystem.
	
	for(i=0;i<systemSize[0];i++)	//make the deliverySytem's cell memory
	{
		deliverySystem[i]=(storage_t*)malloc(sizeof(storage_t)*systemSize[1]);
	}
	

	for(i=0;i<systemSize[0];i++)		//initialize the deliverySystem
	{
		for(j=0;j<systemSize[1];j++)
		{
			initStorage(i,j);
		}
	}
	
	while((c=fgetc(fp))!=EOF)		//get the information to operate system related with delivery
	{
		fscanf(fp,"%d",&row);		//first, get information related with row
		fscanf(fp,"%d",&cell);		//next, get information related with cell
		fscanf(fp,"%d",&deliverySystem[row][cell].building);	//next. get information related with building
		fscanf(fp,"%d",&deliverySystem[row][cell].room);		//next, get information related with room
		fscanf(fp,"%s",deliverySystem[row][cell].passwd);		//next, get information related with password that deliverySystem row, and cell have to contain
		fscanf(fp,"%s",pack);									//next, get information realted with context and record in array 
		deliverySystem[row][cell].context = (char*)malloc(sizeof(char)*(strlen(pack)+1));	//make the memory for context, and the memory is sized by the length of pack
		strcpy(deliverySystem[row][cell].context,pack);	//and get the information that is in the array to context pointer.
	}
	
	fclose(fp);		//close the file

	for(i=0;i<systemSize[0];i++)		//allocate the cnt
	{
		for(j=0;j<systemSize[1];j++)
		{
			if(strlen(deliverySystem[i][j].passwd)>0)		//it means there is the package in that cell{i,j}, so change that cell cnt 0 to 1
				deliverySystem[i][j].cnt = 1;
		}
	}
	
	if(deliverySystem==NULL)		//it means deliverySystem doesn't create
	{
		return -1;
	}
	return 0;		//else , that means deliverySystem is well created
}

//free the memory of the deliverySystem 
void str_freeSystem(void) {
	
	int i,j;
	
	for(i=0;i<systemSize[0];i++)
	{
		for(j=0;j<systemSize[1];j++)
		{
			if(deliverySystem[i][j].cnt>0)			//not all cell don't allocated memory, so check which cell's context has memory
				free(deliverySystem[i][j].context);		//if that cell has memory, free that context memory.
		}
	}
	
	for(i=0;i<systemSize[0];i++)		//free the cell memory
	{
		free(deliverySystem[i]);
	}
	
	free(deliverySystem);		//free the row memory
	
}



//print the current state of the whole delivery system (which cells are occupied and the destination of the each occupied cells)
void str_printStorageStatus(void) {
	int i, j;
	
	
	printf("----------------------------- Delivery Storage System Status (%i occupied out of %i )-----------------------------\n\n", storedCnt, systemSize[0]*systemSize[1]);
	
	printf("\t");
	for (j=0;j<systemSize[1];j++)
	{
		printf(" %i\t\t",j);
	}
	printf("\n-----------------------------------------------------------------------------------------------------------------\n");
	
	for (i=0;i<systemSize[0];i++)
	{
		printf("%i|\t",i);
		for (j=0;j<systemSize[1];j++)
		{
			if (deliverySystem[i][j].cnt > 0)
			{
				printf("%i,%i\t|\t", deliverySystem[i][j].building, deliverySystem[i][j].room);
			}
			else
			{
				printf(" -  \t|\t");
			}
		}
		printf("\n");
	}
	printf("--------------------------------------- Delivery Storage System Status --------------------------------------------\n\n");
}



//check if the input cell (x,y) is valid and whether it is occupied or not
int str_checkStorage(int x, int y) {
	if (x < 0 || x >= systemSize[0])
	{
		return -1;
	}
	
	if (y < 0 || y >= systemSize[1])
	{
		return -1;
	}
	
	return deliverySystem[x][y].cnt;	
}


//put a package (msg) to the cell
//input parameters
//int x, int y : coordinate of the cell to put the package
//int nBuilding, int nRoom : building and room numbers of the destination
//char msg[] : package context (message string)
//char passwd[] : password string (4 characters)
//return : 0 - successfully put the package, -1 - failed to put
int str_pushToStorage(int x, int y, int nBuilding, int nRoom, char msg[MAX_MSG_SIZE+1], char passwd[PASSWD_LEN+1]) {
	
	int i;											//integer for 'for'
	deliverySystem[x][y].building = nBuilding;		//allocate the building number that cell which you chose
	deliverySystem[x][y].room = nRoom;				//allocate the room number that cell which you chose
	 
	
	deliverySystem[x][y].context = (char*)malloc(sizeof(char)*(strlen(msg)+1));	//allocate the memory size that you input the context length and +1 to record NULL 
	strcpy(deliverySystem[x][y].context,msg);	//get the information msg array has. 
	
	for(i=0;i<PASSWD_LEN+1;i++)			//the password is array, so get the informaiton one by one
	{
		deliverySystem[x][y].passwd[i] = passwd[i];
	}
	
	if(strlen(deliverySystem[x][y].context)>0)		//if there is the package in that cell, change the cnt 0 to 1 to mean that there is the package in that cell
		deliverySystem[x][y].cnt=1;
	
	if(deliverySystem[x][y].cnt==1)	//this is mean well push the package.
		return 0;
	
	else		//it means the package is not well recored.
		return -1; 
}



//extract the package context with password checking
//after password checking, then put the msg string on the screen and re-initialize the storage
//int x, int y : coordinate of the cell to extract
//return : 0 - successfully extracted, -1 = failed to extract
int str_extractStorage(int x, int y) {
	int passwd;		//to check the password is right or worng.
	passwd = inputPasswd(x, y); //get return 0 or -1
	if(passwd==0)		//right password
	{
		printf("------>context is : %s",deliverySystem[x][y].context);		//print the context that cell has
		free(deliverySystem[x][y].context);		//free the memory that cell's context has. becase you get the package in that cell has
		initStorage(x,y);		//initialize that cell. becase you get the package in that cell has.
		return 0;				//if well extracted
	}
	
	else		//it means wrong password you input
	{
		printf("----------------wrong password!--------------------\n");		//it need be noticed why you can't get your package
		return -1;	//to say that you fail to extrat
	}
}

//find my package from the storage
//print all the cells (x,y) which has my package
//int nBuilding, int nRoom : my building/room numbers
//return : number of packages that the storage system has
int str_findStorage(int nBuilding, int nRoom) {
	int i,j;		//integer to use 'for'
	int cnt=0;		//first the cnt is 0; 
	for(i=0;i<systemSize[0];i++)
	{
		for(j=0;j<systemSize[1];j++)
		{
			if(deliverySystem[i][j].building==nBuilding&&deliverySystem[i][j].room==nRoom)	//check the the cell contain the number you input
			{
				printf("{row : %d cell : %d}\n", i,j);		//print the cell
				cnt++;			//if the program find the cell that contain you input the number plus one by one. 
			}
			
		}
	}
	
	return cnt;
}
