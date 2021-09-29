#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>                            //header

typedef struct _node {                           //node struct for history
    struct _node* next;
    char data[80];
}Node;
typedef struct _hnode {                          //hnode struct for hash table
    struct _hnode* next;
	int key;
    int opnum;
    char opArr[20];
    char dnum[20];
}HNode;
typedef struct _asmnode{
	struct _asmnode* next;
	int line;
	int loc;
	char label[30];
	char opstr[30];
	char tastr[30];
	char laststr[30];
    char subroutine[100];
	int opjcode;
	int format;
}asmNode;
typedef struct _linkedlist {                     //Linkedlist for history
    Node* head;
    Node* now;
}List;
typedef struct _asmlist{
	asmNode* asmhead;
	asmNode* asmnow;
}asmList;
void Initasm(asmList* asmlist);
int makelist(asmList* asmlist, char*);
int printType(char*);
void symbol(asmList*);


void s_help(void);              // show all possible instructions
void s_dir();                   // show all present directory
void s_history(List* list);     // show all history

Node* makeNode(char* str);                    //make new node for history node
void Init(List* list, char* str);             //initalize LinkedList( make dummy node==head)
void InsertNode(List* list, char* str);       //insert node to linked list

void Dump(int*);                      //print 10 line. remember last address
void DumpStart(int, int*);            //print start(10 line). remember last address
void DumpStartEnd(int, int, int*);    //print start->end, remember last address
int EditAddress(int, int);           //edit address value
int Fillvalue(int, int, int);        //fill value
void reset(void);                     //reset all mArr to 00
void printArr(int, int);              //print mArr

void printError(void);                  //when invaild input occurs, print Error message
unsigned char mArr[65536][16];          //memory arr
HNode* hArry[20];                       //pointer Array for Hash Table
HNode* hpoint;                          //pointer for Hash Table

HNode* makenewnode();                   //make new node for hash table(for haining)
int getKey(char*);                      //get keynum from opcode str
void opcodelist();                      //show all hash table
void InitHash();                        //Initialize Hash Table
void SearchCode(char*);                 //Find opcode str and print it

/*****************************shell_function*****************************/
void s_help(void) {                         // show all possible instructions
    printf("h[elp]\n");
    printf("d[ir]\n");
    printf("q[uit]\n");
    printf("hi[story]\n");
    printf("du[mp] [start, end]\n");
    printf("e[dit] adress, value\n");
    printf("f[ill] start, end, value\n");
    printf("reset\n");
    printf("opcode mnemonic\n");
    printf("opcodelist\n");
	printf("assemble filename\n");
	printf("type filename\n");
	printf("symbol\n");
    return;
}
void s_dir() {                             // show all present directory
    DIR* pDir = NULL;
    struct dirent* pFile = NULL;
    struct stat buf;
    char str[10];
    int i;
    pDir = opendir(".");
    if (pDir != NULL) {
        while ((pFile = readdir(pDir)) != NULL) {
            memset(&buf, 0, sizeof(struct stat));
            lstat(pFile->d_name, &buf);
            if (pFile->d_name[0] != '.') {		//do not print hidden file & directory
                for (i = 0; i < 4; i++)
                    str[i] = pFile->d_name[strlen(pFile->d_name) - (4 - i)];		//find exe file which last our str is .out
                str[i] = '\0';
                if (S_ISDIR(buf.st_mode))			//directory find MACRO 
                    printf("%s/   ", pFile->d_name);
                else if (S_ISREG(buf.st_mode)) {		//file find MACRO
                    if (strcmp(str, ".out") == 0)		//exe file
                        printf("%s*   ", pFile->d_name);
                    else
                        printf("%s   ", pFile->d_name);
                }
            }
        }
        printf("\n");
    }
    closedir(pDir);
    return;
}
void s_history(List* list) {               // show all history
    int i;
    Node* point = list->head->next;
    for (i = 1; point != NULL; i++) {
        printf("%d %s\n", i, point->data);
        point = point->next;
    }
}
/*****************************linkedlist_function*****************************/

Node* makeNode(char* str) {                 //make new node and return it
    Node* newnode = (Node*)malloc(sizeof(Node));
    strcpy(newnode->data, str);
    newnode->next = NULL;
    return newnode;
}
void Init(List* list, char* str) {          //initalize LinkedList( make dummy node==head)
    list->head = makeNode(str);
    list->now = list->head;
    list->head->next = NULL;
}
void InsertNode(List* list, char* str) {    // insert node to linked list
    Node* newnode = makeNode(str);
    list->now->next = newnode;
    list->now = newnode;
}
/*****************************memory_function*****************************/

void Dump(int* point) {                        //print 10 line (160 nums)
    if (*point >= 65536 * 16)                   //check point range
        *point = 0;
    printArr(*point, *point + 159);
    *point = *point + 159;
    if (*point >= 16 * 65536 - 1)            //if new point address>=end address, go to first address
        *point = 0;
    else
        *point = *point + 1;                   //next address is present value+1
}
void DumpStart(int start, int* point) {                     //print address from start(10 line)
    if (start > 16 * 65536 - 1 || start < 0)
        printf("ERROR: start value is out of range\n");      //value check
    else {
        printArr(start, start + 159);
        *point = start + 159 + 1;
    }
}
void DumpStartEnd(int start, int end, int* point) {    //print address from start to end
    if (start > end)
        printf("ERROR: start value is larger than end value\n");
    else if (start > 16 * 65536 - 1 || start < 0 || end>16 * 65536 - 1 || end < 0)
        printf("ERROR: start value and end value are out of range\n");
    else {
        printArr(start, end);
        *point = end + 1;
    }
    //need error check
}
int EditAddress(int address, int value) {     //edit address's value
    int i, j;
    if (value < 0x00 || value>0xFF){
        printf("ERROR: value is out of range\n");
		return 0;
	}
    else if (address > 16 * 65536 - 1 || address < 0){
        printf("ERROR: address is out of range\n");
		return 0;
	}
    else {
        i = address / 16;
        j = address % 16;
        mArr[i][j] = value;
		return 1;
    }
}
int Fillvalue(int start, int end, int value) {         //fill value from start to end address
    int i, j;
    if (start > end){                                    //range check
        printf("ERROR: start value is larger than end value\n");
		return 0;
	}
    else if (start > 16 * 65536 - 1 || start < 0 || end>16 * 65536 - 1 || end < 0){
        printf("ERROR: start value and end value are out of size\n");
		return 0;
	}
    else if (value < 0x20 || value>0x7E){
        printf("ERROR: value is out of size\n");
		return 0;
	}
    else {
        for (i = start / 16; i <= end / 16; i++) {
            for (j = 0; j < 16; j++) {
                if (i == start / 16 && j < start % 16)
                    continue;
                else if (i == end / 16 && j > end % 16)
                    break;
                else{
                    mArr[i][j] = value;
					return 1;
				}
            }
        }
    }
	return 0;
}
void reset() {                           //reset all mArr to 00
    memset(mArr, 0, sizeof(mArr));
}
void printArr(int start, int end) {      //print mArr
    int i, j;
    if (end >= 16 * 65536)               //if end>sizeofarr than    set end=maxsize-1
        end = 16 * 65536 - 1;
    for (i = start / 16; i <= end / 16; i++) {
        printf("%05X  ", i * 16);                               //print left address

        for (j = 0; j < 16; j++) {
            if (i == start / 16 && j < start % 16) {            //first row. before start, print "     "
                printf("     ");
            }
            else if (i == end / 16 && j <= end % 16) {          //last row. vaild range, print value
                printf("%02X   ", mArr[i][j]);
            }
            else if (i == end / 16 && j > end % 16)             //after end address, print "     "
                printf("     ");
            else
                printf("%02X   ", mArr[i][j]);                  //print middle memory. vaild range

        }
        printf("; ");
        for (j = 0; j < 16; j++) {//print right ASCII code
		 	if(i==start/16 && j<start %16)
			 	printf("%c", '.');
			else if(i==end/16 && j>end%16)
			 	printf("%c",'.');
			else if (mArr[i][j] >= 0x20 && mArr[i][j] <= 0x7E)       //vaild range
                printf("%c", mArr[i][j]);
            else
                printf("%c", '.');                              //invaild range
        }
        printf("\n");
    }
}
void printError(void) {                                         //when Invaild cmd in.. print ERROR message
    printf("Invaild cmd\n");
}

/*****************************Hash_function*****************************/
HNode* makenewnode() {                                  //make new node & initialize
    HNode* newnode = (HNode*)malloc(sizeof(HNode));
    newnode->next = NULL;
    newnode->key = newnode->opnum = 0;
    strcpy(newnode->opArr, "\0");
    strcpy(newnode->dnum, "\0");
    return newnode;
}
int getKey(char* str) {                                 //get key
    int i, total = 0;
    for (i = 0; str[i] != '\0'; i++) {                  //add all str[i] values(char type). and do %20 operation
        total += str[i];
    }
    return total % 20;
}
void opcodelist() {                                      //print opcodelist
    int i;
    for (i = 0; i < 20; i++) {
        printf("%d : ", i);
        for (hpoint = hArry[i]; hpoint != NULL; hpoint = hpoint->next) {
            if (hpoint->next == NULL)                                 //last member. do not print  "->"
                printf("[%s,%X]", hpoint->opArr, hpoint->opnum);
            else
                printf("[%s,%X] -> ", hpoint->opArr, hpoint->opnum);  //general print format
        }
        printf("\n");
    }
}
void SearchCode(char* str) {                        //search opcode str and print it
    int key = 0;
    key = getKey(str);
    for (hpoint = hArry[key]; hpoint != NULL; hpoint = hpoint->next) {      //search hash table, using key & hpoint
        if (strcmp(hpoint->opArr, str) == 0) {
            printf("opcode is %X\n", hpoint->opnum);
            return;
        }
   }
    printf("Error: there is no %s opcode\n", str);     //can't find, print Error messasge
}
void InitHash() {                                      //reset all hasharry& hpoint to NULL
    int i;
    for (i = 0; i < 20; i++) {
        hArry[i] = NULL;
    }
    hpoint = NULL;
}
/*****************************ASM_function*****************************/





