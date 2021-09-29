#include <stdio.h>
#include "20161571.h"

/*****************************main_function*****************************/
int main() {
    List list;
    Init(&list, "0");                   //initialize list
    HNode* newnode;                     
    char str[80], substr[80];           //str: arr using by save input data.   substr: arr using by divide str 
    int p = 0, start = 0, end = 0, address = 0, value = 0, key = 0, i, j;
    int* point = &p;                    //pointer for save next address after function return. 
    InitHash();							//initialize hashtable
    FILE* fp = fopen("opcode.txt", "r");		//file open & error check
    if (fp == NULL) {
        printf("File open Error\n");
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
        else if (strcmp(str, "help") == 0) {                        //help
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
                    for (i = i + 1; (str[i] != ',') && (str[i] != '\0') && (str[i] != ' '); i++ && j++)                          //get next word: address
                        substr[j] = str[i];
                    if (str[i] == '\0' || str[i] == ' ') {                //Error check.
                        printError();
                    }
                    else {
                        substr[j] = '\0';
                        address = strtol(substr, NULL, 16);             //translate str to hexa num
                        j = 0;
                        for (i = i + 2; str[i] != '\0'; i++ && j++)     //get last word: value
                            substr[j] = str[i];
                        substr[j] = '\0';
                        value = strtol(substr, NULL, 16);               //translate to str to hexa num
                            InsertNode(&list, str);
                            EditAddress(address, value);
                    }
                }
                else if (strcmp(substr, "fill") == 0 || strcmp(substr, "f") == 0) {     //fill or f
                    j = 0;
                    for (i = i + 1; (str[i] != ',') && (str[i] != '\0') && (str[i] != ' '); i++ && j++)      //get start
                        substr[j] = str[i];
                    if (str[i] == '\0' || str[i] == ' ') {                //Error check. invaild format
                        printError();
                    }
                    else {
                        substr[j] = '\0';
                        start = strtol(substr, NULL, 16);
                        j = 0;
                        for (i = i + 2; (str[i] != ',') && (str[i] != '\0') && (str[i] != ' '); i++ && j++)       //get end
                            substr[j] = str[i];
                        if (str[i] == '\0' || str[i] == ' ') {                //Error check. invaild format
                            printError();
                        }
                        else {
                            substr[j] = '\0';
                            end = strtol(substr, NULL, 16);
                            j = 0;
                            for (i = i + 2; (str[i] != ',') && (str[i] != '\0') && (str[i] != ' '); i++ && j++)       //get value
                                substr[j] = str[i];
                            if (str[i] == ',' || str[i] == ' ') {                       //Error check. Too many input
                                printError();
                            }
                            else {
                                substr[j] = '\0';
                                value = strtol(substr, NULL, 16);
                                    InsertNode(&list, str);
                                    Fillvalue(start, end, value);
                            }
                        }
                    }
                }
                else if (strcmp(substr, "opcode") == 0) {                  //opcode
                    j = 0;
                    for (i = i + 1; str[i] != '\0'; i++ && j++)
                        substr[j] = str[i];
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
                    for (i = i + 1; str[i] != ',' && str[i] != '\0' && str[i]!=' '; i++ && j++)
                        substr[j] = str[i];
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
                                    for (i = i + 2; (str[i] != '\0') && (str[i] != ' ') && (str[i] != ','); i++ && j++)
                                        substr[j] = str[i];
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

