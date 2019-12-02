#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"

#define MAX_ROW				4
#define MAX_COLUMN			6

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
static int systemSize[2] = {0, 0};  		//row/column of the delivery system
static int masterPassword[4] = {0, 5, 0, 7};				//master password
//마스터 패스워드는 그냥 제 생일롤 해보았습니다. 
int passwd[PASSWD_LEN+1];

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
	
	deliverySystem[x][y].building= 0;
	deliverySystem[x][y].context= 0;
	deliverySystem[x][y].passwd= 0;
	deliverySystem[x][y].room= 0;
	deliverySystem[x][y].cnt = 0;
	
}

//get password input and check if it is correct for the cell (x,y)
//int x, int y : cell for password check
//return : 0 - password is matching, -1 - password is not matching
static int inputPasswd(int x, int y) {
	int i;
	char record_pass;
	int record; //리턴할 값을 임시로 저장하는 변수 
	printf("input the password : ");
	scanf("%4s",&passwd[PASSWD_LEN+1],);
	
	for(i=0;i<4;i++)
	{
		if(record_pass[i]==deliverySystem[x][y].passwd[i])
			record = 0;
		else if(record_pass[i]!= deliverySystem[x][y].passwd[i])
			record = -1;
			break;
		
	}
	return record;
}





// ------- API function for main.c file ---------------

//backup the delivery system context to the file system
//char* filepath : filepath and name to write
//return : 0 - backup was successfully done, -1 - failed to backup
int str_backupSystem(char* filepath) {
	FILE *fp;							//define file pointer to write 
	FILE *fp_ch;						//define file pointer to check that the system context is well recorded
	int i, j;							//define number used "for" 
	fp = fopen("filepath","w");			
	fprintf(fp,"%d",MAX_ROW);			//record row 
	fprintf(fp,"%d",MAX_COLUMN);		//record column
	
	for(i=0;i<MAX_ROW;i++)				//record structure related with delivery
	{
		for(j=0;j<MAX_COLUMN;j++)
		{
			fprintf(fp,"%d",deliverySystem[x][y].building);
			fprintf(fp,"%d",deliverySystem[x][y].cnt);
			fprintf(fp,"%d",deliverySystem[x][y].context);
			fprintf(fp,"%d",deliverySystem[x][y].passwd);
			fprintf(fp,"%d",deliverySystem[x][y].room);
		}
	}
	for(i=0;i<4;i++)			//record master password
	{
		fprintf(fp,"%d",masterPassword[i]);
	}
	fclose(fp);
	
	fp_ch = fopen("filepath","r");
	
	if(fp_ch==NULL)
		return -1;
	else
		return 0;
}



//create delivery system on the double pointer deliverySystem
//char* filepath : filepath and name to read config parameters (row, column, master password, past contexts of the delivery system
//return : 0 - successfully created, -1 - failed to create the system
int str_createSystem(char* filepath) {
	
	FILE *fp_read;
	int i,j;
	fp_read = fopen("filepath","rb");
	if(fp==NULL)
	{
		return -1;
	}
	for(i=0;i<MAX_LOW;i++)
	{
		for(j=0;j<MAX_COLUMN;j++)
		{
			fread(&deliverySystem[i][j],1,fp_read);
		}
	}
	
	fcloase(fp_read);
	
	
}

//free the memory of the deliverySystem 
void str_freeSystem(void) {
	int i,j;
	for(i=0;i<MAX_ROW;i++)
	{
		for(j=0;j<MAX_COLUMN;j++)
		{
			deliverySystem[i][j].building=NULL;
			deliverySystem[i][j].cnt=NULL;
			deliverySystem[i][j].context=NULL;
			deliverySystem[i][j].passwd=NULL;
			deliverySystem[i][j].room=NULL;
		}
	}
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
	
	int i;
	deliverySystem[x][y].building = nBuilding;
	deliverySystem[x][y].room = nRoom;
	 
	for (i=0;i<MAX_MAS_SIZE+1;i++)
	{
		if(msg[i]!='\0')
		{
			deliverySystem[x][y].context[i]=msg[i];
		}
		else
			break;
	}
	for(i=0;i<PASSWD_LEN+1;i++)
	{
		if(passwd[PASSWD_LEN+1]!='\0')
		{
			deliverySystem[x][y].passwd[i] = passwd[i];
		}
		else
			break;
	}
	
	if(deliverySystem[x][y].building==NULL||deliverySystem[x][y].room==NULL||deliverySystem[x][y].context==NULL||deliverySystem[x][y].passwd==NULL)
		return -1;
	
	else   
		return 0;   
}



//extract the package context with password checking
//after password checking, then put the msg string on the screen and re-initialize the storage
//int x, int y : coordinate of the cell to extract
//return : 0 - successfully extracted, -1 = failed to extract
int str_extractStorage(int x, int y) {
	int passwd;
	passwd = inputPasswd(x, y);
	if(passwd==0)
	{
		printf("%100s",deliverySystem[x][y].context);
		initStorage(x,y);
		return 0;
	}
	else
	{
		return -1;
	}
}

//find my package from the storage
//print all the cells (x,y) which has my package
//int nBuilding, int nRoom : my building/room numbers
//return : number of packages that the storage system has
int str_findStorage(int nBuilding, int nRoom) {
	int i,j;	
	for(i=0;i<MAX_ROW;i++)
	{
		for(j=0;j<MAX_COLUMN;j++)
		{
			if(deliverySystem[i][j].building==nBuilding&&deliverySystem[i][j].room==nRoom)
			{
				cnt=deliverySystem[i][j].cnt;
				break;
			}
			
		}
	}
	
	return cnt;
}
