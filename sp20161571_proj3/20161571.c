#include <stdio.h>
#include "20161571.h"
#include "20161571_1.h"
#include "20161571_2.h"

/*****************************main_function*****************************/
int main() {
    List list;
	asmList asmlist;
    Init(&list, "0");                   //initialize list
    Initasm(&asmlist); 
	HNode* newnode;                     
    char str[80], substr[80];           //str: arr using by save input data.   substr: arr using by divide str 
	int p = 0, start = 0, end = 0, address = 0, value = 0, key = 0, i, j,fun=0;
    int* point = &p;                    //pointer for save next address after function return. 
    InitHash();							//initialize hashtable
    FILE* fp = fopen("opcode.txt", "r");		//file open & error check
    if (fp == NULL) {
        printf("opcode.txt open Error\n");
        exit(-1);
    }
    while (!feof(fp)) {                 //Open file, get key values and save thier data to hash table
        newnode = makenewnode();
        fscanf(fp, "%X  %s %s\n", &newnode->opnum, newnode->opArr, newnode->dnum);
        key = getKey(newnode->opArr);
        if (hArry[key] == NULL)         //when empty, just save it
            hArry[key] = newnode;
        else {
            for (hpoint = hArry[key]; hpoint->next != NULL; hpoint = hpoint->next);         //not empty, find the last node and link newnode to it's next
            hpoint->next = newnode;
        }
    }

    fclose(fp);
    while (1) {                                                     //sicsim start
        printf("sicsim> ");
        fgets(str,80,stdin);
        str[strlen(str) - 1] = '\0';
        substr[0] = '\0';
        if (strcmp(str, "q") == 0 || strcmp(str, "quit") == 0) {    //quit
            InsertNode(&list, str);
            break;
        }
        else if (strcmp(str, "help") == 0 || strcmp(str,"h")==0) {                        //help
            InsertNode(&list, str);
            s_help();
        }
        else if (strcmp(str, "d") == 0 || strcmp(str, "dir") == 0) {      //dir
            InsertNode(&list, str);
            s_dir();
        }
        else if (strcmp(str, "hi") == 0 || strcmp(str, "history") == 0) {        //history
            InsertNode(&list, str);
            s_history(&list);
        }
        else if (strcmp(str, "reset") == 0) {          //reset
            InsertNode(&list, str);
            reset();
        }
        else if (strcmp(str, "opcodelist") == 0) {           //opcodelist
            InsertNode(&list, str);
            opcodelist();
        }
        else if (strcmp(str, "symbol") == 0) {              //print symbol
            InsertNode(&list, str);
            symbol(&asmlist);
        }
		else if(strcmp(str,"bp")==0){
			InsertNode(&list,str);
			printbp();			  
		}
		else if(strcmp(str,"progaddr")==0){
			printf("Invaild cmd %s\n",str);
		}
		else if(strcmp(str,"run")==0){
			InsertNode(&list,str);
			run();
		}
        else {
            if (strcmp(str, "dump") == 0 || strcmp(str, "du") == 0) {               //dump or du
                InsertNode(&list, str);
                Dump(point);
            }
            else {                                                     //if input str is no one word,
                for (i = 0; str[i] != ' ' && str[i] != '\0'; i++)      // str data to substr, before space  : find first str
                    substr[i] = str[i];
                substr[i] = '\0';
                if (strcmp(substr, "edit") == 0 || strcmp(substr, "e") == 0) {          //if first word is edit or e
                    j = 0;
                    for (i = i + 1; (str[i] != ',') && (str[i] != '\0') && (str[i] != ' '); i++){                          //get next word: address
						substr[j] = str[i];
						j++;
					}
                    if (str[i] == '\0' || str[i] == ' ') {                //Error check.
                        printError();
                    }
                    else {
                        substr[j] = '\0';
                        address = strtol(substr, NULL, 16);             //translate str to hexa num
                        j = 0;
                        for (i = i + 2; str[i] != '\0'; i++){     //get last word: value
						 	substr[j] = str[i];
							j++;
						}
                        substr[j] = '\0';
                        value = strtol(substr, NULL, 16);               //translate to str to hexa num
                        if(EditAddress(address, value)==1)
							InsertNode(&list,str);
                    }
                }
				else if(strcmp(substr,"bp")==0){
					j=0;
					for(i=i+1;str[i]!='\0';i++){
						substr[j]=str[i];
						j++;
					}
					substr[j]='\0';
					if(strcmp(substr,"clear")==0){
						InsertNode(&list,str);
						clearbp();
					}
					else{
					 	InsertNode(&list,str);
						createbp(strtol(substr,NULL,16));
					}
				
				}
				else if(strcmp(substr,"progaddr")==0){			//progaddr address
					j=0;
					for(i=i+1;str[i]!='\0';i++){
						substr[j]=str[i];
						j++;
					}
					substr[j]='\0';
					if(progaddr(strtol(substr,NULL,16))==1)
						InsertNode(&list,str);
				}
				else if(strcmp(substr,"loader")==0){			//loader
					char obj1[30], obj2[30],obj3[30];
					strcpy(obj1," ");
					strcpy(obj2," ");
					strcpy(obj3," ");
				 	j=0;
					for(i=i+1;str[i]!=' ' && str[i]!='\0';i++){
						substr[j]=str[i];
						j++;
					}
					substr[j]='\0';
					strcpy(obj1,substr);
					if(str[i]=='\0')
						linking_loader(obj1,obj2,obj3);
					else if(str[i]==' '){
						j=0;
						for(i=i+1;str[i]!=' ' && str[i]!='\0';i++){
							substr[j]=str[i];
							j++;
						}
						substr[j]='\0';
						strcpy(obj2,substr);
						if(str[i]=='\0')
							linking_loader(obj1,obj2,obj3);
						else if(str[i]==' '){
							j=0;
							for(i=i+1;str[i]!='\0';i++){
								substr[j]=str[i];
								j++;
							}
							substr[j]='\0';
							strcpy(obj3,substr);
							linking_loader(obj1,obj2,obj3);
						}
					}
					InsertNode(&list,str);
				}
                else if (strcmp(substr,"assemble")==0) {        //assemble file
                    j = 0;
                    for (i = i + 1; str[i] != '\0';i++){
					 	substr[j] = str[i];
						j++;
					}
                    substr[j] = '\0';
                    fun = makelist(&asmlist, substr);
                    if (fun!=-1) {
                        InsertNode(&list, str);
                        if(fun==1)
                            printf("Successfully %s\n", str);
                    }
                }
                else if (strcmp(substr, "type") == 0) {         //print type str
                    j = 0;
                    for (i = i + 1; str[i] != '\0'; i++){
					 	substr[j] = str[i];
						j++;
					}
                    substr[j] = '\0';
                    
                    if(printType(substr)!=-1)
                        InsertNode(&list, str);
                }
                else if (strcmp(substr, "fill") == 0 || strcmp(substr, "f") == 0) {     //fill or f
                    j = 0;
                    for (i = i + 1; (str[i] != ',') && (str[i] != '\0') && (str[i] != ' '); i++){      //get start
						 substr[j] = str[i];
				 		 j++;
					}
                    if (str[i] == '\0' || str[i] == ' ') {                //Error check. invaild format
                        printError();
                    }
                    else {
                        substr[j] = '\0';
                        start = strtol(substr, NULL, 16);
                        j = 0;
                        for (i = i + 2; (str[i] != ',') && (str[i] != '\0') && (str[i] != ' '); i++){       //get end
						 	substr[j] = str[i];
							j++;
						}
                        if (str[i] == '\0' || str[i] == ' ') {                //Error check. invaild format
                            printError();
                        }
                        else {
                            substr[j] = '\0';
                            end = strtol(substr, NULL, 16);
                            j = 0;
                            for (i = i + 2; (str[i] != ',') && (str[i] != '\0') && (str[i] != ' '); i++){       //get value
							 	substr[j] = str[i];
								j++;
							}
                            if (str[i] == ',' || str[i] == ' ') {                       //Error check. Too many input
                                printError();
                            }
                            else {
                                substr[j] = '\0';
                                value = strtol(substr, NULL, 16); 
                                if(Fillvalue(start, end, value)==1)
								 	InsertNode(&list,str);
                            }
                        }
                    }
                }
                else if (strcmp(substr, "opcode") == 0) {                  //opcode
                    j = 0;
                    for (i = i + 1; str[i] != '\0'; i++){
					 	substr[j] = str[i];
						j++;
					}
                    substr[j] = '\0';
                    if (strcmp(substr, "\0") == 0)
                        printf("Invaild cmd\n");
                    else{
                        InsertNode(&list,str);
						SearchCode(substr);
					}
                }
                else if (strcmp(substr, "dump") == 0 || strcmp(substr, "du") == 0) {         //dump ... or du ...
                    j = 0;
                    for (i = i + 1; str[i] != ',' && str[i] != '\0' && str[i]!=' '; i++){
					 	substr[j] = str[i];
						j++;
					}
                    if (str[i] == ' ')
                        printError();
                    else {
                        substr[j] = '\0';
                        start = strtol(substr, NULL, 16);           //get start value
                            if (str[i] == '\0') {
                                InsertNode(&list, str);
                                DumpStart(start, point);
                            }
                            else if (str[i] == ',') {
                                j = 0;
                                if (str[i + 1] == ' ') {
                                    for (i = i + 2; (str[i] != '\0') && (str[i] != ' ') && (str[i] != ','); i++){
									 	substr[j] = str[i];
										j++;
									}
                                    if (str[i] == '\0') {
                                        substr[j] = '\0';
                                        end = strtol(substr, NULL, 16);
                                            InsertNode(&list, str);
                                            DumpStartEnd(start, end, point);
                                    }
                                    else
                                        printError();
                                }
                                else
                                    printError();      // no space
                            }
                        else
                            printError();      //there are no case: invaild cmd
                    }
                }
                else
                    printError();
            }
        }
        for (i = 0; i < 30; i++)            //initialize
            str[i] = substr[i] = '\0';
    }
    return 0;
}

