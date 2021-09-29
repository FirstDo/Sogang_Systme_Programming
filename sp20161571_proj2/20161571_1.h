void Initasm(asmList *asmlist){								//function: Init asmlist and make dummynode;
	asmlist->asmhead=(asmNode*)malloc(sizeof(asmNode));
	asmlist->asmhead->next=NULL;
	asmlist->asmnow=asmlist->asmhead;
}
int printType(char* filename) {							//function: Open file and print it line by line
	FILE* fp=fopen(filename,"r");
	if (fp == NULL) {
		printf("file open error. There is no %s\n", filename);
		return -1;
	}
	char s[200];
	while (1) {										//print line by line
		fgets(s, 200, fp);
		if (feof(fp))
			break;
		printf("%s", s);
	}
	return 1;
}
int makelist(asmList *asmlist, char* filename){			//function: Make .lst and .obj
	FILE* asmfp = fopen(filename, "r");	//open filename.asm(r) 2_5.lst(r)  2_5.obj(r)
	asmNode *p,*p2;					//pointer for asmNode p,p2
	if (asmfp == NULL) {
		printf("file open error. There is no %s\n",filename);
		return -1;
	}
	int count=0,loc=0x0,line=5,key=0,i,found=0,fin=0,base=0;
	char s1[30],s2[30],s3[30],temp[1000];					//s1,s2,s3 for read string
	/*****************************make line,loc,data**************************************/
	while(1){									//At first, make line,loc,data
		if (fin == 1)										//Termination condition
			break;
		temp[0] = s1[0] = s2[0] = s3[0] = '\0';				//init variable
		count = 0;
		fscanf(asmfp,"%s",s1);								//read first data
		asmNode* newnode = (asmNode*)malloc(sizeof(asmNode));//make newnode & Insert it to asmlist
		newnode->next = NULL;
		asmlist->asmnow->next = newnode;
		asmlist->asmnow = newnode;

		newnode->line = line;								//init asmdata. line,loc...opjcode,found
		strcpy(newnode->label, " ");
		strcpy(newnode->opstr, " ");
		strcpy(newnode->tastr, " ");
		strcpy(newnode->laststr, " ");
		newnode->opjcode = 0x0;
		found = 0;
		count++;
		if (s1[0] == '.') {									//if s1=. save data to node and continue
			fgets(temp, sizeof(temp), asmfp);				//data is . subroutine~~~	
			temp[strlen(temp) - 1] = '\0';
			strcpy(newnode->label, ".");
			strcpy(newnode->subroutine, temp);
			line += 5;
			continue;
		}
		if(fgetc(asmfp)!='\n'){								//Read second data
			fscanf(asmfp,"%s",s2);
			count++;
			if(fgetc(asmfp)!='\n'){							//Read third data
				fscanf(asmfp,"%s",s3);
				count++;
				if (fgetc(asmfp) != '\n') {					//ERROR: too many data(over 3data)
					printf("Error: line%d  too many data\n", line);
					return 0;
				}
			}
		}
		if (strcmp(s2, "START") == 0)						//Init loc = start loc 
			loc = strtol(s3,NULL,16);
		newnode->loc = loc;
		if(count==1){											//Case: only RSUB
		 	strcpy(newnode->opstr,s1);
			newnode->format=3;
			found = 1;
		}
		if (count == 2) {										//Case: no LABEL. only opstr and tastr
			if (s1[0] == '+') {									//If operation str is format4
				for (i = 0; s1[i + 1] != '\0'; i++)				//Remove '+' and make temp
					temp[i] = s1[i + 1];
				temp[i] = '\0';
				key = getKey(temp);								//Get key
			}
			else
				key = getKey(s1);								//Get key
			for (hpoint = hArry[key]; hpoint != NULL; hpoint = hpoint->next) {	//Search operation in hash table
				if (strcmp(hpoint->opArr, s1) == 0) {
					strcpy(newnode->opstr, s1);				//Search success ->store it to node
					if (strcmp(hpoint->dnum, "2") == 0)		//Case: format2
						newnode->format = 2;
					else if (strcmp(hpoint->dnum, "3/4") == 0)
						newnode->format = 3;				//Case: format3
					found = 1;
					break;
				}
				else if (strcmp(hpoint->opArr, temp) == 0) {//Search operation in hash table. format4
					strcpy(newnode->opstr, s1);				//Search success->store it to node
					newnode->format = 4;
					found = 1;
					break;
				}
			}

			if (strcmp(s1, "END") == 0 || strcmp(s1, "BASE") == 0) {	//Special case: END, BASE
				strcpy(newnode->opstr, s1);
				newnode->format = 0;
				found = 1;
			}
			strcpy(newnode->tastr, s2);						//Store s2 to asmnode->tastr
		}

		if(count==3){										//Case: 3data
		 	key=getKey(s1);									
			for(hpoint=hArry[key];hpoint!=NULL;hpoint=hpoint->next){
				if(strcmp(hpoint->opArr,s1)==0){			//Case: if s1=opstr
				 	found=1;
					if (strcmp(hpoint->dnum, "2") == 0)		//set format
						newnode->format = 2;
					else if (strcmp(hpoint->dnum, "3/4") == 0)		//set format
						newnode->format = 3;
					strcpy(newnode->opstr,s1);				//set s1,s2,s3
					strcpy(newnode->tastr,s2);
					strcpy(newnode->laststr,s3);
					break;
				}
			}
			if(found==0){
				strcpy(newnode->label,s1);					//Case: s1==label
				if(s2[0]=='+'){								//Format 4. remode '+'
					for(i=0;s2[i+1]!='\0';i++)
						temp[i]=s2[i+1];
					temp[i]='\0';
					key=getKey(temp);
				}
				else
					key=getKey(s2);
				for(hpoint=hArry[key];hpoint!=NULL;hpoint=hpoint->next){	//Search
					if(strcmp(hpoint->opArr,s2)==0){		//If s2=opstr
						found=1;
						if(strcmp(hpoint->dnum,"2")==0)
							newnode->format=2;
						else if(strcmp(hpoint->dnum,"3/4")==0)
								newnode->format = 3;
						strcpy(newnode->opstr,s2);			//Set s2,s3 and s1
						strcpy(newnode->tastr,s3);
						break;
					}
					else if (strcmp(hpoint->opArr,temp) == 0) {		//Case: format4
						found = 1;
						newnode->format = 4;
						strcpy(newnode->opstr, s2);			//Set s2,s3 and s1
						strcpy(newnode->tastr, s3);
						break;
					}
				}
				if(found==0){								//Special case!
					if(strcmp(s2,"BYTE")==0){				//Case: s2=BYTE
						found=1;
						strcpy(newnode->opstr,s2);
						strcpy(newnode->tastr,s3);
						if (s3[0] == 'C') {					//If BYTE char,
							int j=0;
							for (i = 0; s3[i] != '\''; i++);
							for (i = i + 1; s3[i] != '\''; i++)
								j++;
							newnode->format = j;
						}
						else if (s3[0] == 'X')				//If BYTE num
							newnode->format = 1;
					}
					if (strcmp(s2, "WORD") == 0) {				//Case: s2=WORD
						found = 1;
						strcpy(newnode->opstr, s2);
						strcpy(newnode->tastr, s3);
						if (s3[0] == 'C') {					//If WORD char,
							int j = 0;
							for (i = 0; s3[i] != '\''; i++);
							for (i = i + 1; s3[i] != '\''; i++)
								j++;
							newnode->format = j*3;
						}
						else if (s3[0] == 'X')				//If WORD num
							newnode->format = 3;
					}
					if(strcmp(s2,"RESB")==0){				//Case: s2=RESB
						found=1;
						strcpy(newnode->opstr,s2);
						strcpy(newnode->tastr,s3);
						newnode->format=atoi(s3);			//Change str to num and store it
					}
					if(strcmp(s2,"RESW")==0){				//Case: s2=RESW
						found=1;
						strcpy(newnode->opstr,s2);
						strcpy(newnode->tastr,s3);
						newnode->format=atoi(s3)*3;			//Change str to num and store it(x3)
					}
					if (strcmp(s2, "START") == 0) {			//Case: s2=START
						found = 1;
						strcpy(newnode->opstr, s2);
						strcpy(newnode->tastr, s3);
						newnode->format = 0;				//no format
					}
				}
				if (found == 0) {								//ERROR: There is no case. Invaild Input
					printf("Error: line %d \n Invaild Input %s", line,newnode->opstr);
					return 0;
				}
			}
		}
		line+=5;						//+ 5line
		loc+=newnode->format;			//Calculate next loc and store it
		if (strcmp(s2, "FIRST") == 0)	//End Termination. meet FIRST
			fin = 1;					//Set end flag
	}
/************************************make opcode*****************************************************/
	FILE* listfp = fopen("2_5.lst", "w");
	FILE* objfp = fopen("2_5.obj", "w");
	int nb, ib, xb, bb, pb, eb,format,opnum,opject_code,pc,reg1,reg2;	//Flag and temporary variable
	char str[10],subta[10];						//String to save data
	int endAddress=0;
	for (p = asmlist->asmhead->next; p!=NULL; p = p->next) {	//Until p->opstr==END
		if (strcmp(p->opstr, "END") != 0) {
			pc = p->next->loc;										//Pc counter
			endAddress = pc;									//save list opjcode for pass2
		}
		nb = 0; ib = 0; xb = 0; bb = 0; pb = 0; eb = 0; format = 0;
		opnum= 0;reg1=reg2=opject_code=format=0;		//Init flag
		subta[0] = '\0';
		strcpy(str, p->opstr);
		if (strcmp(p->opstr, "BASE") == 0) {				//Special case: BASE. get base's loc
			for (p2 = asmlist->asmhead->next; p2!=NULL; p2 = p2->next) {
				if (strcmp(p2->label, p->tastr) == 0) {
					base = p2->loc;
					break;
				}
			}
		}
		if (strcmp(p->label, ".")==0) {						//for . subroutine~
			fprintf(listfp,"%-7d %s %s\n", p->line, p->label, p->subroutine);
			continue;
		}
		//Special Case: START END BASE RESW RESB BYTE WORD
		if (strcmp(str, "START") == 0 || strcmp(str, "END") == 0 || strcmp(str, "BASE") == 0 || strcmp(str, "RESW") == 0 || strcmp(str, "RESB") == 0 || strcmp(str,"BYTE")==0 ||strcmp(str,"WORD")==0) {
			if (strcmp(p->opstr, "BASE") == 0 || strcmp(p->opstr, "END") == 0) // Case : BASE END
				fprintf(listfp,"%-5d %-10s %-10s %-10s %s\n", p->line, p->label, p->opstr, p->tastr, p->laststr);
			else if(strcmp(p->opstr,"START")==0 ||strcmp(p->opstr,"RESB")==0 || strcmp(p->opstr,"RESW")==0) //Case : START RESB RESW
				fprintf(listfp,"%-7d %04X     %-10s %-10s %-10s %s\n", p->line, p->loc, p->label, p->opstr, p->tastr, p->laststr);
			else if(strcmp(p->opstr,"BYTE")==0){
				if (p->tastr[0] == 'C') {			//Case: BYTE C
					strcpy(subta, p->tastr);
					for (i = 0;subta[i+2]!='\0';i++)
						subta[i] = subta[i + 2];
					subta[i] = '\0';
					int j = 1;
					for (i = strlen(subta) - 2; i >= 0; i--) {
						opject_code += subta[i] * j;
						j *= 16*16;
					}
					p->opjcode = opject_code;
					fprintf(listfp,"%-7d %04X     %-10s %-10s %-10s %s     %06X\n", p->line, p->loc, p->label, p->opstr, p->tastr, p->laststr, p->opjcode);
				}
				else if (p->tastr[0] == 'X') {		//Case: BYTE X
					strcpy(subta, p->tastr);
					for (i = 0;subta[i+2]!='\'';i++)
						subta[i] = subta[i + 2];
					subta[i] = '\0';
					opject_code = strtol(subta, NULL, 16);
					p->opjcode = opject_code;
					fprintf(listfp,"%-7d %04X     %-10s %-10s %-10s %s     %02X\n", p->line, p->loc, p->label, p->opstr, p->tastr, p->laststr, p->opjcode);
				}
			}
			else if (strcmp(p->opstr, "WORD") == 0) {
				if (p->tastr[0] == 'C') {			//Case: WORD C
					strcpy(subta, p->tastr);
					for (i = 0; subta[i + 2] != '\0'; i++)
						subta[i] = subta[i + 2];
					subta[i] = '\0';
					int j = 1;
					for (i = strlen(subta) - 2; i >= 0; i--) {
						opject_code += subta[i] * j;
						j *= 16 * 16;
					}
					p->opjcode = opject_code;
					fprintf(listfp, "%-7d %04X     %-10s %-10s %10s %s     %06X\n", p->line, p->loc, p->label, p->opstr, p->tastr, p->laststr, p->opjcode);
				}
				else if (p->tastr[0] == 'X') {		//Case: WORD X
					strcpy(subta, p->tastr);
					for (i = 0; subta[i + 2] != '\''; i++)
						subta[i] = subta[i + 2];
					subta[i] = '\0';
					opject_code = strtol(subta, NULL, 16);
					p->opjcode = opject_code;
					fprintf(listfp, "%-7d %04X     %-10s %-10s %10s %s     %02X\n", p->line, p->loc, p->label, p->opstr, p->tastr, p->laststr, p->opjcode);
				}
			}
			continue;
		}
		else {					//General case
			if (str[0] == '+') {									//Format4. remove '+'
				eb = 1;
				format = 4;
				for (i = 0; str[i] != '\0'; i++)
					str[i] = str[i + 1];
			}
			key = getKey(str);
			for (hpoint = hArry[key]; hpoint != NULL; hpoint = hpoint->next) {		//Search opnum in hash table
				if (strcmp(hpoint->opArr, str) == 0) {
					if (strcmp(hpoint->dnum, "2") == 0)				//Format 2
						format = 2;
					else if (strcmp(hpoint->dnum, "3/4") == 0) {	//Format 3,4
						if (format != 4)
							format = 3;
					}
					opnum = hpoint->opnum;							//Get opnum from node
					break;
				}
			}
			if (format == 2) {										//Case: format 2
				if (strcmp(p->laststr, " ") != 0) {		//If laststr exist,
					subta[0] = p->tastr[0];
					subta[1] = '\0';					//Remove ',' from tastr
					if (strcmp(p->laststr, "A") == 0)
						reg2 = 0;
					else if (strcmp(p->laststr, "X") == 0)
						reg2 = 1;
					else if (strcmp(p->laststr, "L") == 0)
						reg2 = 2;
					else if (strcmp(p->laststr, "PC") == 0)
						reg2 = 8;
					else if (strcmp(p->laststr, "SW") == 0)
						reg2 = 9;
					else if (strcmp(p->laststr, "B") == 0)
						reg2 = 3;
					else if (strcmp(p->laststr, "S") == 0)
						reg2 = 4;
					else if (strcmp(p->laststr, "T") == 0)
						reg2 = 5;
					else if (strcmp(p->laststr, "F") == 0)
						reg2 = 6;
					else {
						printf("Error: line %d The Invaild register input\n", p->line);
						fclose(listfp);
						fclose(objfp);
						remove("2_5.lst");
						remove("2_5.obj");
						return 0;
					}
				}
				if (strcmp(subta, "A") == 0 ||strcmp(p->tastr,"A")==0)
					reg1 = 0;
				else if (strcmp(subta, "X") == 0 || strcmp(p->tastr, "X") == 0)
					reg1 = 1;
				else if (strcmp(subta, "L") == 0 || strcmp(p->tastr, "L") == 0)
					reg1 = 2;
				else if (strcmp(subta, "PC") == 0 || strcmp(p->tastr, "PC") == 0)
					reg1 = 8;
				else if (strcmp(subta, "SW") == 0 || strcmp(p->tastr, "SW") == 0)
					reg1 = 9;
				else if (strcmp(subta, "B") == 0 || strcmp(p->tastr, "B") == 0)
					reg1 = 3;
				else if (strcmp(subta, "S") == 0 || strcmp(p->tastr, "S") == 0)
					reg1 = 4;
				else if (strcmp(subta, "T") == 0 || strcmp(p->tastr, "T") == 0)
					reg1 = 5;
				else if (strcmp(subta, "F") == 0 || strcmp(p->tastr, "F") == 0)
					reg1 = 6;
				else {
					printf("Error: line %d The Invaild register input\n", p->line);
					fclose(listfp);
					fclose(objfp);
					remove("2_5.lst");
					remove("2_5.obj");
					return 0;
				}
				opject_code = opnum * 16 * 16 + reg1 * 16 + reg2;		
				p->opjcode = opject_code;
				if (strcmp(p->laststr, " ") == 0)
					fprintf(listfp,"%-7d %04X     %-10s %-10s %-10s %s     %-8X\n", p->line, p->loc, p->label, p->opstr, p->tastr, p->laststr, p->opjcode);
				else
					fprintf(listfp,"%-7d %04X     %-10s %-10s %s %-10s    %-8X\n", p->line, p->loc, p->label, p->opstr, p->tastr, p->laststr, p->opjcode);
				continue;
			}
			if (p->tastr[0] == '#') {								//Immediate addressing
				nb = 0;												//Set n,i flag= 0, 1 
				ib = 1;
				for (i = 0; p->tastr[i] != '\0'; i++)				//Remove #. make subta
					subta[i] = p->tastr[i + 1];
			}
			else if (p->tastr[0] == '@') {							//Indirect addressing
				nb = 1;												//Set n,i flag= 1. 0
				ib = 0;
				for (i = 0; p->tastr[i] != '\0'; i++)				//Remove @. make subta
					subta[i] = p->tastr[i + 1];
			}
			else {													//Simple addressing
				nb = 1;												//Set n,i flga = 1, 1
				ib = 1;
			}
			if (p->tastr[strlen(p->tastr) - 1] == ',') {			//For Buffer, remove ,
				for (i = 0;i!=strlen(p->tastr)-1; i++)
					subta[i] = p->tastr[i];
				subta[i] = '\0';
			}
			if (strcmp(p->opstr, "RSUB") == 0) {					//Special case: RSUB
				opject_code = opnum * 16 * 16 * 16 * 16 + nb * 16 * 16 * 16 * 16 * 2 + ib * 16 * 16 * 16 * 16;
				p->opjcode = opject_code;
				fprintf(listfp,"%-7d %04X     %10s %-10s %10s %s     %-8X\n", p->line, p->loc, p->label, p->opstr, p->tastr, p->laststr, p->opjcode);
				continue;
			}
			for (p2 = asmlist->asmhead->next; p2 != NULL; p2 = p2->next) {		//Search label
				int pc_value;
				if (strcmp(p2->label, p->tastr) == 0) {		//Simple addressing
					pc_value = p2->loc - pc;
					if (format == 3) {									//Format 3
						if (strcmp(p->laststr, "X") == 0)
							xb = 1;
						if (-2048 <= pc_value && pc_value <= 2047) {	//Pc relative
							pb = 1;
							if (pc_value < 0)
								pc_value += 4096;
							//opnum + n(1) + i(1) + x(?) + b(0) + p(1) + e(0) + disp(12)
							opject_code = opnum * 16 * 16 * 16 * 16 + nb * 16 * 16 * 16 * 16 * 2 + ib * 16 * 16 * 16 * 16 + xb * 16 * 16 * 16 * 8 + pb * 16 * 16 * 16 * 2 + pc_value;
							p->opjcode = opject_code;
							break;
						}
						else {											//Base relative
							bb = 1;
							//opnum + n(1) + i(1) + x(?) + b(1) + p(0) + e(0) + disp(12)
							opject_code = opnum * 16 * 16 * 16 * 16 + nb * 16 * 16 * 16 * 16 * 2 + ib * 16 * 16 * 16 * 16 + xb * 16 * 16 * 16 * 8 + bb * 16 * 16 * 16 * 4 + p2->loc - base;
							p->opjcode = opject_code;
							break;
						}
					}
					else if (format == 4) {						//Format 4
						eb = 1;
						//opnum + n(1) + i(1) + x(?) + b(0) + p(0) + e(1) + address(20)
						opject_code = opnum * 16 * 16 * 16 * 16 * 16 * 16 + nb * 16 * 16 * 16 * 16 * 16 * 16 * 2 + ib * 16 * 16 * 16 * 16 * 16 * 16 + eb * 16 * 16 * 16 * 16 * 16 + p2->loc;
						p->opjcode = opject_code;
						break;
					}
				}
				else if (strcmp(p2->label, subta) == 0) {		//Other case.
					if (nb == 0 && ib == 1) {					//Case: immediate addressing
						pc_value = p2->loc - pc;				//Set pc_value
						if (format == 3) {						//Format 3
							if (strcmp(p2->laststr, "X") == 0)	
								xb = 1;
							if (-2048 <= pc_value && pc_value <= 2047) {// Pc relative
								if (pc_value < 0)
									pc_value += 4096;
								pb = 1;
								//opnum + n(0) + i(1) + x(?) + b(0) + p(1) + disp(12)
								opject_code = opnum * 16 * 16 * 16 * 16 + ib * 16 * 16 * 16 * 16 + xb * 16 * 16 * 16 * 8 + pb * 16 * 16 * 16 * 2 + pc_value;
								p->opjcode = opject_code;
								break;
							}
							else {										//Base relative
								//opnum + n(0) + i(1) + x + b(1) + p(0)  + disp(12)
								bb = 1;
								opject_code = opnum * 16 * 16 * 16 * 16 + ib * 16 * 16 * 16 * 16 + xb * 16 * 16 * 16 * 8 + bb * 16 * 16 * 16 * 4 + p2->loc - base;
								p->opjcode = opject_code;
								break;
							}
						}
						else if (format == 4) {					//Format 4
							eb = 1;
							if (strcmp(p->laststr, "X") == 0)
								xb = 1;
							if (-2048 <= pc_value && pc_value <= 2047) {// Pc relative
								if (pc_value < 0)
									pc_value += 4096;
								pb = 1;
								//opnum + n(0) + i(1) + x(?) + b(0) + p(1) + e(1) + address(20)
								opject_code = opnum * 16 * 16 * 16 * 16 * 16 * 16 + pc_value + ib * 16 * 16 * 16 * 16 * 16 * 16 + xb * 16 * 16 * 16 * 16 * 16 * 8 + pb * 16 * 16 * 16 * 16 * 16 * 2 + eb * 16 * 16 * 16 * 16 * 16;
								p->opjcode = opject_code;
								break;
							}
							else {										//Base relative
								//opnum + n(0) + i(1) + x(?) + b(1) + p(0) + e(1)+ address(20)
								bb = 1;
								opject_code = opnum * 16 * 16 * 16 * 16 * 16 * 16 + p2->loc - base + ib * 16 * 16 * 16 * 16 * 16 * 16 + xb * 16 * 16 * 16 * 16 * 16 * 8 + bb * 16 * 16 * 16 * 16 * 16 * 4 + eb * 16 * 16 * 16 * 16 * 16;
								p->opjcode = opject_code;
								break;
							}
						}
					}
					else if (nb == 1 && ib == 0) {						//Case: indirect addressing
						pc_value = p2->loc - pc;
						if (format == 3) {							//Format 3
							if (strcmp(p->laststr, "X") == 0)
								xb = 1;
							if (-2048 <= pc_value && pc_value <= 2047) {		//Pc relative
								pb = 1;
								if (pc_value < 0)
									pc_value += 4096;
								//opnum + n(1) + i(0) + x(?) + b(0) + p(1) + e(0) + disp(12)
								opject_code = opnum * 16 * 16 * 16 * 16 + nb * 16 * 16 * 16 * 16 * 2 + xb * 16 * 16 * 16 * 8 + pb * 16 * 16 * 16 * 2 + pc_value;
								p->opjcode = opject_code;
								break;
							}
							else {												//Base relative
								bb = 1;
								//opnum + n(1) + i(0) + x(?) + b(1) + p(0) + e(0) + disp(12)
								opject_code = opnum * 16 * 16 * 16 * 16 + nb * 16 * 16 * 16 * 16 * 2 + xb * 16 * 16 * 16 * 8 + bb * 16 * 16 * 16 * 4 + p2->loc - base;
								p->opjcode = opject_code;
								break;
							}
						}
						else if (format == 4) {								//Format 4
							eb = 1;
							//opnum + n(0) + i(1) + x(?) + b(0) + p(0) + e(1) + address(20)
							opject_code = opnum * 16 * 16 * 16 * 16 * 16 * 16 + nb * 16 * 16 * 16 * 16 * 16 * 16 * 2 + ib * 16 * 16 * 16 * 16 * 16 * 16 + eb * 16 * 16 * 16 * 16 * 16 + p2->loc;
							p->opjcode = opject_code;
							break;
						}
					}
					else if (nb == 1 && ib == 1) {					//Special Case: BUFFER
						pc_value = p2->loc - pc;
						if (format == 3) {							//Format 3
							if (strcmp(p->laststr, "X") == 0)		//Set x flag
								xb = 1;
							if (-2048 <= pc_value && pc_value <= 2047) {		//Pc relative
								pb = 1;
								if (pc_value < 0)
									pc_value += 4096;
								//opnum + n(1) + i(1) + x(?) + b(0) + p(1) + e(0) + disp(12)
								opject_code = opnum * 16 * 16 * 16 * 16 + nb * 16 * 16 * 16 * 16 * 2 + ib*16*16*16*16+xb * 16 * 16 * 16 * 8 + pb * 16 * 16 * 16 * 2 + pc_value;
								p->opjcode = opject_code;
								break;
							}
							else {												//Base relative
								bb = 1;
								//opnum + n(1) + i(1) + x(?) + b(1) + p(0) + e(0) + disp(12)
								opject_code = opnum * 16 * 16 * 16 * 16 + nb * 16 * 16 * 16 * 16 * 2 + ib*16*16*16*16+xb * 16 * 16 * 16 * 8 + bb * 16 * 16 * 16 * 4 + p2->loc - base;
								p->opjcode = opject_code;
								break;
							}
						}
						else if (format == 4) {					//Format 4
							eb = 1;
							if (strcmp(p->laststr, "X") == 0)		//Set x flag
								xb = 1;
							//opnum + n(1) + i(1) + x(?) + b(0) + p(0) + e(1) + address(20)
							opject_code = opnum * 16 * 16 * 16 * 16 * 16 * 16 + nb * 16 * 16 * 16 * 16 * 16 * 16 * 2 + ib * 16 * 16 * 16 * 16 * 16 * 16 + xb*16*16*16*16*16*8+eb * 16 * 16 * 16 * 16 * 16 + p2->loc;
							p->opjcode = opject_code;
							break;
						}
					}
				}
			}
			if (p->opjcode == 0) {							//Case: Real immediate addressing ex) #0 #4096
				if (0<=strtol(subta, NULL, 10) &&strtol(subta,NULL,10)<=4096) {		//if input is decimal
					if (strcmp(p->laststr, "X") == 0)
						xb = 1;
					if (format == 4) {	//Format 4
						eb = 1;
						//opnum + n(0) + i(1) + x(?) + b(0) + p(0) + e(1) + address(20)
						opject_code = opnum * 16 * 16 * 16 * 16 * 16 * 16 + ib * 16 * 16 * 16 * 16 * 16 * 16 + xb * 16 * 16 * 16 * 16 * 16 * 8 +eb * 16 * 16 * 16 * 16 * 16 + strtol(subta, NULL, 10);
					}
					else				//Format 3
						opject_code = opnum * 16 * 16 * 16 * 16 + ib * 16 * 16 * 16 * 16 +xb*16*16*16*8+ strtol(subta, NULL, 16);
					p->opjcode = opject_code;
					if(format==4)
						fprintf(listfp,"%-7d %04X     %-10s %-10s %-10s %-s     %08X\n", p->line, p->loc, p->label, p->opstr, p->tastr, p->laststr, p->opjcode);
					else {
						fprintf(listfp,"%-7d %04X     %-10s %-10s %-10s %-s     %06X\n", p->line, p->loc, p->label, p->opstr, p->tastr, p->laststr, p->opjcode);
					}
					continue;
				}
				else {		//ERROR: can't make opcode
					printf("ERROR: cannot make opcode line: %d\n", p->line);
					fclose(listfp);
					fclose(objfp);
					remove("2_5.lst");
					remove("2_5.obj");
					return -1;
				}
			}
		}
		if (strcmp(p->opstr, "BASE") == 0 || strcmp(p->opstr, "END") == 0) {
			fprintf(listfp,"%-7d %4s     %-10s %-10s %-10s %s\n", p->line, " ", p->label, p->opstr, p->tastr, p->laststr);
		}
		else {
			if(format==4)
				fprintf(listfp,"%-7d %04X     %-10s %-10s %-8s %-5s   %08X\n", p->line, p->loc, p->label, p->opstr, p->tastr, p->laststr, p->opjcode);
			else
				fprintf(listfp,"%-7d %04X     %-10s %-10s %-8s %-5s   %06X\n", p->line, p->loc, p->label, p->opstr, p->tastr, p->laststr, p->opjcode);
		}
	}
	fclose(asmfp);
	fclose(listfp);
/****************************************make obj file************************************************/
	int curlen=0,floc=0,wb=0;			
	char tmp[100], tmp2[100];
	tmp[0] =tmp2[0]= '\0';
	for (p = asmlist->asmhead->next; p != NULL; p = p->next) {
		if (strcmp(p->label, "COPY") == 0)						//Make first line
			fprintf(objfp,"H%s	%06X%06X", p->label, p->opjcode, endAddress);
		else if (p->opjcode!=0 || strcmp(p->opstr, "RESB") == 0 || strcmp(p->opstr, "RESW") == 0){
			if (strcmp(p->opstr, "RESB") == 0 || strcmp(p->opstr, "RESW") == 0)	//Set termination flag
				wb =1;
			else if (curlen>=29 || wb==1){
				fprintf(objfp,"\nT%06X%02X%s", floc,curlen,tmp);		//Print next line
				curlen =wb= 0;									//Init data
				tmp[0] = tmp2[0]='\0';
				
				floc = p->loc;
				if (strcmp(p->opstr, "BYTE") == 0) {
					if(p->tastr[0]=='C')
						sprintf(tmp2, "%06X", p->opjcode);
					else
						sprintf(tmp2, "%02X", p->opjcode);
				}
				else if (p->format == 2)
					sprintf(tmp2, "%04X", p->opjcode);
				else if(p->format==3 ||p->format==4)
					sprintf(tmp2, "%06X", p->opjcode);
				strcat(tmp, tmp2);
				curlen += p->format;
			}
			else {
				if (curlen == 0)
					floc = p->loc;
				if (strcmp(p->opstr, "BYTE") == 0) {
					if (p->tastr[0] == 'C')
						sprintf(tmp2, "%06X", p->opjcode);
					else
						sprintf(tmp2, "%02X", p->opjcode);
				}
				else if (p->format == 2)
					sprintf(tmp2, "%04X", p->opjcode);
				else if (p->format == 3 || p->format == 4)
					sprintf(tmp2, "%06X", p->opjcode);
				strcat(tmp, tmp2);
				curlen += p->format;
			}
		}
	}
	fprintf(objfp,"\nT%06X%02X%s\n",floc,curlen,tmp);
	for (p = asmlist->asmhead->next; p != NULL; p = p->next) {
		if (strcmp(p->label, "COPY") == 0)
			floc = p->loc;
		if (p->format == 4 && p->tastr[0] != '#')	//format 4 and simple address
			fprintf(objfp,"M%06X%02X\n", p->loc + 1, 5);
	}
	fprintf(objfp,"E%06X\n", floc);		//print end lined

	fclose(objfp);
	return 1;
}

void symbol(asmList* asmlist) {							//make symbol and print it
	asmNode* newnode = (asmNode*)malloc(sizeof(asmNode));
	asmNode *p,*now,*before,*hp;										//pointer for list
	strcpy(newnode->label, "A");
	newnode->next = NULL;
	hp = asmlist->asmhead->next;
	p = newnode;										//initialize

	for (; hp != NULL; hp = hp->next) {
		if (strcmp(hp->label, " ") != 0 && strcmp(hp->opstr,"START")!=0 && strcmp(hp->label,".")) {		//if hp->label exist,
			asmNode* newnode = (asmNode*)malloc(sizeof(asmNode));
			newnode->next = NULL;
			strcpy(newnode->label, hp->label);
			newnode->loc = hp->loc;								//save hp to newnode
			now = before = p;
			while (1) {											// Insert newnode to list
				if (strcmp(now->label, newnode->label) < 0) {
					if (now->next == NULL) {						// insert newnode if nextnode is NULL
						now->next = newnode;
						break;
					}
					else {
						before = now;
						now = now->next;
					}
				}
				else if (strcmp(now->label, newnode->label) > 0) {
					newnode->next = now;
					before->next = newnode;
					break;
				}
			}

		}
	}
	for (p = p->next; p != NULL; p = p->next)
		printf("	%-10s %04X\n", p->label, p->loc);
	return;
}








