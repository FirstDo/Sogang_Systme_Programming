#define TRUE	1
#define FALSE	0
#define UP		2
#define DOWN	-2
typedef struct _enode{		//enode structure for linked list.
	struct _enode* next;
 	char c_section[30];
	char symbol[30];
	int address;
	int length;
}ENode;

typedef struct _reg{		//struct for run function. They are register
	int A;
	int L;
	int B;
	int T;
	int X;
	int PC;
	int S;	
}Reg;
Reg reg;

typedef struct _flag{		//struct for run function. It will be used for store n,i,x,b,p,e bit
	int n;
	int i;
	int x;
	int b;
	int p;
	int e;
}Flag;
Flag f;

int target_address=0x00;	//T.A

typedef struct _mem{		//It will be used for store memory data
	int mem_total;
	int mem1;
	int mem2;
	int mem3;
	int mem4;
}buffer_mem;
buffer_mem m;

typedef struct _bp{			//BNode for break point
	struct _bp *next;
	int bp;
	int onoff;
}BNode;

BNode* H_bnode=NULL;
BNode* cur_bnode=NULL;

typedef struct _refer1{
	int s_addr;
	char s_name[30];
}Refer1;


typedef struct _refer2{
	int s_addr;
	char s_name[30];
}Refer2;

typedef struct _refer3{
	int s_addr;
	char s_name[30];
}Refer3;

Refer1 r1[10];
Refer1 r2[10];
Refer3 r3[10];

int R_address=0x00;
int Address=0x00;		//Address(default is 0x00)
int temp_Address=0x00;	//variable for save Address
ENode *H_enode;			//ENode head pointer
ENode *cur_enode;
void InitEnode(void){			//init ENode(make dummy node and point it)
	ENode* newnode=(ENode*)malloc(sizeof(ENode));
	newnode->next=NULL;
	H_enode=newnode;
	cur_enode=newnode;
}
ENode* InsertEnode(void){		//make newnode,initialize and insert it to linkedlist  
	ENode* newnode=(ENode*)malloc(sizeof(ENode));
	newnode->next=NULL;
	strcpy(newnode->c_section," ");
	strcpy(newnode->symbol," ");
	newnode->address=-1;
	newnode->length=0;
	cur_enode->next=newnode;
	cur_enode=newnode;
	return newnode;
}
void InitBnode(void){
	BNode* newnode=(BNode*)malloc(sizeof(BNode));
	newnode->next=NULL;
	H_bnode=newnode;
	cur_bnode=newnode;
}
void InsertBnode(int bp){
	BNode* newnode=(BNode*)malloc(sizeof(BNode));
	newnode->next=NULL;
	newnode->bp=bp;
	newnode->onoff = 1;
	cur_bnode->next=newnode;
	cur_bnode=newnode;
}
void createbp(int bp){	//create all bp
 	if(H_bnode==NULL && cur_bnode==NULL)	//make dummy node and set H_bnode, cur_bnode
		InitBnode();
	InsertBnode(bp);
	printf("		[ok] create breakpoint %x\n",bp);
}
void printbp(void){		//print all bp
	BNode* blist;
	printf("		breakpoint\n");
	printf("		----------\n");
	if(H_bnode==NULL && cur_bnode==NULL)
		return;
	for(blist=H_bnode->next;blist!=NULL;blist=blist->next)
		printf("		%X\n",blist->bp);
}
void clearbp(void){		//clear all bp
 	if(H_bnode==NULL && cur_bnode==NULL){
		printf("		[ok] clear all breakpoints\n");
		return;
	}
 	BNode *blist;
	BNode *next_blist;
	blist=H_bnode->next;
	next_blist=blist->next;
	while(next_blist!=NULL){
	 	free(blist);
		blist=next_blist;
		next_blist=next_blist->next;
	}
	H_bnode=NULL;
	cur_bnode=NULL;
	free(blist);
	printf("		[ok] clear all breakpoints\n");
}
int CharToDecimal(char ch) {		//convert char to decimal
	if (ch >= '0' && ch <= '9')
		return ch - 48;
	else if (ch >= 'A' && ch <= 'F')
		return ch - 55;
	else
	 	return FALSE;
}
int getAddrValue(int address){	//function for Modification record
	int i,j;
	if(address >16*65536-1 || address<0){
		printf("ERROR: address is out of range\n");
		return FALSE;
	}
	else{
		i=address/16;
		j=address%16;
		return mArr[i][j];
	}
}
int progaddr(int address){			//progaddr function
	if(address>=0 && address<=0xFFFFF){
 		Address=address;
		temp_Address=Address;
		return TRUE;
	}
	else
	 	return FALSE;
}
void linking_loader(char* obj1, char* obj2, char*obj3){	//Linking loader function
//****************************************PASS 1_START******************************************************// 	
 	char type_check;
	char p_name[100],r_temp[100];
	int num=0,temp=0,temp_sub=0,i=0,index=0;
	int temp_length=0;
	InitEnode();		//init Enode
	reset();			//clear memory

	for (i = 0; i < 10; i++) {			//init r1,r2,r3
		r1[i].s_addr = -1;
		r2[i].s_addr = -1;
		r3[i].s_addr = -1;

		strcpy(r1[i].s_name, " ");
		strcpy(r2[i].s_name, " ");
		strcpy(r3[i].s_name, " ");
	}

	FILE *fp=fopen(obj1,"rt");
	if(fp==NULL){
		printf("FILE OPEN ERROR. There is no %s file.\n",obj1);
		return ;
	}
	while(fscanf(fp,"%c",&type_check)!=EOF){			//read the first char of row
		if(type_check=='H'){						//when 'H'
			ENode* newnode=InsertEnode();
		    fscanf(fp,"%s",newnode->c_section);		//read file name and store it to ESTAB
		    fscanf(fp,"%x",&num);				//read start address+length
    		newnode->address=Address+num/100000;	//store (load)Address + program start address
			r1[1].s_addr=newnode->address;			//store progA start address to refer1 arr
			newnode->length=num%100000;			//store program length
			if(strcmp(obj1,"copy.obj")==0){		//initialize register for run copy.obj
				reg.A=0;
				reg.L=newnode->length;
				reg.B=0;
				reg.T=0;
				reg.X=0;
				reg.PC=0;
				reg.S=0;
			}
			temp_length=newnode->length;		//save program length to temp_length
			fgetc(fp);							//remove '\n'
		}
		else if(type_check=='D'){					//when 'D'
			while(1){
				num=0;						
				temp_sub=16*16*16*16*16;
				fscanf(fp,"%s",p_name);		//read symbol name and store it to ESTAB
				if(p_name[0]=='R'){			//when 'R' save the R line to r_str1 for reference
					index=(p_name[1]-48)*16;
					index+=p_name[2]-48;
					strcpy(r1[index].s_name,p_name+3);

					while(fscanf(fp,"%s",r_temp)){
						if(strcmp(r_temp,".")==0)
							break;
						index=(r_temp[0]-48)*16;
						index+=r_temp[1]-48;
						strcpy(r1[index].s_name,r_temp+2);
					}
					break;
				}
				ENode* newnode=InsertEnode();		//make new enode
				strcpy(newnode->symbol,p_name);		//store p_name(symbol name) to newnode->symbol
				for(i=0;i<6;i++){			//read number one by one, make address and store address
					temp = fgetc(fp);
					if (temp == ' ') {		//ignore ' '
						i--;
						continue;
					}
					temp = temp - 48;
					num+=temp*temp_sub;		//make number
					temp_sub/=16;			
			  	}
			  	newnode->address=num+Address;
			}
		}
		else
			fgets(p_name,sizeof(p_name),fp);	//just read one row(not need)
	}
	fclose(fp);
	Address+=temp_length;			//set next program's starting address

	if(strcmp(obj2," ")!=0){		//if obj2 exist, do same thing as obj1
		FILE *fp2=fopen(obj2,"rt");
		if(fp2==NULL){
			printf("FILE OPEN ERROR. There is no %s file.\n",obj2);
			return;
		}
		while(fscanf(fp2,"%c",&type_check)!=EOF){			
		  	if(type_check=='H'){
		   		ENode* newnode=InsertEnode();
		  		fscanf(fp2,"%s",newnode->c_section);		
				fscanf(fp2,"%x",&num);				
				newnode->address=Address+num/100000;
				r2[1].s_addr=newnode->address;
				newnode->length=num%100000;
				temp_length=newnode->length;
				fgetc(fp2);
		  	}
		  	else if(type_check=='D'){
				while(1){
			 		num=0;
			  		temp_sub=16*16*16*16*16;
			  		fscanf(fp2,"%s",p_name);		
					if (p_name[0] == 'R') {			
						index = (p_name[1] - 48) * 16;
						index += p_name[2] - 48;
						strcpy(r2[index].s_name, p_name + 3);
						while (fscanf(fp, "%s", r_temp)) {
							if (strcmp(r_temp, ".") == 0)
								break;
							index = (r_temp[0] - 48) * 16;
							index += r_temp[1] - 48;
							strcpy(r2[index].s_name, r_temp + 2);
						}
						break;
					}
					ENode* newnode=InsertEnode();
					strcpy(newnode->symbol,p_name);
					for (i = 0; i < 6; i++) {		
						temp = fgetc(fp2);
						if (temp == ' ') {		
							i--;
							continue;
						}
						temp = temp - 48;
						num += temp * temp_sub;		
						temp_sub /= 16;
					}
			  		newnode->address=num+Address;
			    }
			}
			else
				  fgets(p_name,sizeof(p_name),fp2);
		}
		fclose(fp2);
	}
	Address+=temp_length;

	if(strcmp(obj3," ")!=0){		//if obj3 exist, do same thing as obj1, obj2
		FILE *fp3=fopen(obj3,"rt");
		if(fp3==NULL){
			printf("FILE OPEN ERROR. There is no %s file.\n",obj3);
			return;
		}
        while(fscanf(fp3,"%c",&type_check)!=EOF){	
			if(type_check=='H'){
				  ENode* newnode=InsertEnode();
				  fscanf(fp3,"%s",newnode->c_section);		
				  fscanf(fp3,"%x",&num);				
				  newnode->address=Address+num/100000;
				  r3[1].s_addr=newnode->address;
				  newnode->length=num%100000;
				  temp_length=newnode->length;
				  fgetc(fp3);
			}
			else if(type_check=='D'){
				  while(1){
			 		num=0;
			  		temp_sub=16*16*16*16*16;
			  		fscanf(fp3,"%s",p_name);	
					if (p_name[0] == 'R') {			
						index = (p_name[1] - 48) * 16;
						index += p_name[2] - 48;
						strcpy(r3[index].s_name, p_name + 3);
						while (fscanf(fp, "%s", r_temp)) {
							if (strcmp(r_temp, ".") == 0)
								break;
							index = (r_temp[0] - 48) * 16;
							index += r_temp[1] - 48;
							strcpy(r3[index].s_name, r_temp + 2);
						}
						break;
					}
					ENode* newnode=InsertEnode();
					strcpy(newnode->symbol,p_name);
					for (i = 0; i < 6; i++) {
						temp = fgetc(fp3);
						if (temp == ' ') {
							i--;
							continue;
						}
						temp = temp - 48;
						num += temp * temp_sub;
						temp_sub /= 16;
					}
			  		newnode->address=num+Address;
			  	  }
			}
			else
				  fgets(p_name,sizeof(p_name),fp3);
		}
		fclose(fp3);
	}

	ENode* elist1;
	for (i = 1;i<10; i++) {					//referenceing
		if ((strcmp(r1[i].s_name," ")==0) && (strcmp(r2[i].s_name, " ") == 0) && (strcmp(r3[i].s_name, " ") == 0))
			continue;
		for (elist1 = H_enode->next; elist1 != NULL; elist1 = elist1->next) {
			if (strcmp(r1[i].s_name, elist1->symbol) == 0)
				r1[i].s_addr = elist1->address;
			if (strcmp(r2[i].s_name, elist1->symbol) == 0)
				r2[i].s_addr = elist1->address;
			if (strcmp(r3[i].s_name, elist1->symbol) == 0)
				r3[i].s_addr = elist1->address;
		}
	}
	printf("control symbol address length\n");
	printf("section name\n");
	printf("------------------------------------\n");

	int total_length=0;
	for(elist1=H_enode->next;elist1!=NULL;elist1=elist1->next){			//print all loadmap
	 	total_length+=elist1->length;
		if(elist1->length==0)
 			printf("%-7s %6s	%04X\n",elist1->c_section,elist1->symbol,elist1->address);
		else
	 		printf("%-7s %6s	%04X	%04X\n",elist1->c_section,elist1->symbol,elist1->address,elist1->length);
	}
	printf("-------------------------------------\n");
	printf("	   total length %04X\n",total_length);


//***************************************PASS 1_END***************************************************//
//***************************************PASS 2_START*************************************************//
	Address=temp_Address;
	int t_address, t_mult,t_length,t_mem,t_modi,total_addr;
	char tempstr[100],oper;
	FILE *p1=fopen(obj1,"rt");
	if(p1==NULL){
		printf("FILE OPEN ERROR. There is no %s \n",obj1);
		return ;
	}


	while((type_check=fgetc(p1))){		//read first char of row
		if(type_check=='T'){			//when 'T'
			t_mult=1048576;
			t_address=0;
			for(i=0;i<6;i++){			//get text recoder's start address
				t_address+=(CharToDecimal(fgetc(p1)))*t_mult;
				t_mult/=16;
			}
			t_address+=r1[1].s_addr;
			t_length=CharToDecimal(fgetc(p1))*16+ CharToDecimal(fgetc(p1));	//get text recoder's length

			for(i=0;i<t_length;i++){					//get 1byte and load it to memory
				t_mem= CharToDecimal(fgetc(p1)) *16+ CharToDecimal(fgetc(p1));
				EditAddress(t_address+i,t_mem);
			}
		}
		else if(type_check=='M'){		//when 'M'
			t_mult=1048576;
			t_address=0;
			for(i=0;i<6;i++){
				t_address+= CharToDecimal(fgetc(p1)) *t_mult;
				t_mult/=16;
			}
			t_address+=r1[1].s_addr;
			t_modi= CharToDecimal(fgetc(p1)) *16+ CharToDecimal(fgetc(p1));
			oper=fgetc(p1);
			index= CharToDecimal(fgetc(p1)) *16+ CharToDecimal(fgetc(p1));	//get reference index
			t_mult=65536;
			total_addr=0;
			for(i=0;i*2<t_modi;i++){				//get t_modi byte address form memory(total_addr)				
				total_addr+=getAddrValue(t_address+i)*t_mult;
				t_mult/=256;
			}
			if(oper=='+')
				total_addr+=r1[index].s_addr;
			else if (oper == '-') 
				total_addr -= r1[index].s_addr;
			else
			 	return ;
			if (total_addr < 0)
				total_addr += 0x10000000;
			total_addr %= 0x1000000;
			t_mult = 65536;
			for(i=0;i*2<t_modi;i++){
				EditAddress(t_address+i,total_addr/t_mult);
				total_addr %= t_mult;
				t_mult /= 256;
			}
		}
		else if(type_check=='E'){		//when 'E'
			t_mult=1048576;
			t_address=0;
			for(i=0;i<6;i++){		
				t_address+= CharToDecimal(fgetc(p1)) *t_mult;
				t_mult/=16;
			}
			Address=t_address;			//set program start Address
			break;
		}
		else if(type_check=='\n'){
			continue;	
		}
		else
			fgets(tempstr,sizeof(tempstr),p1);
	}
	fclose(p1);
	if (strcmp(obj2, " ") != 0) {		//if obj2 exist, do same as obj1
		FILE* p2 = fopen(obj2, "rt");
		if (p2 == NULL) {
			printf("FILE OPEN ERROR. There is no %s\n", obj2);
			return ;
		}
		while ((type_check = fgetc(p2))) {		
			if (type_check == 'T') {		//when 'T'
				t_mult = 1048576;
				t_address = 0;
				for (i = 0; i < 6; i++) {			
					t_address += CharToDecimal(fgetc(p2)) * t_mult;
					t_mult /= 16;
				}
				t_address += r2[1].s_addr;
				t_length = CharToDecimal(fgetc(p2)) * 16 + CharToDecimal(fgetc(p2));

				for (i = 0; i < t_length; i++) {					
					t_mem = CharToDecimal(fgetc(p2)) * 16 + CharToDecimal(fgetc(p2));
					EditAddress(t_address + i, t_mem);
				}
			}
			else if (type_check == 'M') {		//when 'M'
				t_mult = 1048576;
				t_address = 0;
				for (i = 0; i < 6; i++) {
					t_address += CharToDecimal(fgetc(p2)) * t_mult;
					t_mult /= 16;
				}
				t_address += r2[1].s_addr;
				t_modi = CharToDecimal(fgetc(p2)) * 16 + CharToDecimal(fgetc(p2));
				oper = fgetc(p2);
				index = CharToDecimal(fgetc(p2)) * 16 + CharToDecimal(fgetc(p2));
				t_mult = 65536;
				total_addr = 0;
				for (i = 0; i * 2 < t_modi; i++) {							
					total_addr += getAddrValue(t_address + i) * t_mult;
					t_mult /= 256;
				}
				if (oper == '+')
					total_addr += r2[index].s_addr;
				else if (oper == '-')
					total_addr -= r2[index].s_addr;
				else
					return ;
				if (total_addr < 0)
					total_addr += 0x10000000;
				total_addr %= 0x1000000;
				t_mult = 65536;
				for (i = 0; i * 2 < t_modi; i++) {
					EditAddress(t_address + i, total_addr / t_mult);
					total_addr %= t_mult;
					t_mult /= 256;
				}
			}
			else if (type_check == 'E') {		//when 'E'
				t_mult = 1048576;
				t_address = 0;
				for (i = 0; i < 6; i++) {
					t_address += CharToDecimal(fgetc(p2)) * t_mult;
					t_mult /= 16;
				}
				break;
			}
			else if (type_check == '\n') {
				continue;
			}
			else
				fgets(tempstr, sizeof(tempstr), p2);
		}
		fclose(p2);
	}


	if (strcmp(obj3, " ") != 0) {		//if obj3 exist, do same as obj1,obj2
		FILE* p3 = fopen(obj3, "rt");
		if (p3 == NULL) {
			printf("FILE OPEN ERROR. There is no %s\n", obj2);
			return ;
		}
		while ((type_check = fgetc(p3))) {
			if (type_check == 'T') {		//when 'T'
				t_mult = 1048576;
				t_address = 0;
				for (i = 0; i < 6; i++) {			
					t_address += CharToDecimal(fgetc(p3)) * t_mult;
					t_mult /= 16;
				}
				t_address += r3[1].s_addr;
				t_length = CharToDecimal(fgetc(p3)) * 16 + CharToDecimal(fgetc(p3));

				for (i = 0; i < t_length; i++) {					
					t_mem = CharToDecimal(fgetc(p3)) * 16 + CharToDecimal(fgetc(p3));
					EditAddress(t_address + i, t_mem);
				}
			}
			else if (type_check == 'M') {		//when 'M'
				t_mult = 1048576;
				t_address = 0;
				for (i = 0; i < 6; i++) {
					t_address += CharToDecimal(fgetc(p3)) * t_mult;
					t_mult /= 16;
				}
				t_address += r3[1].s_addr;
				t_modi = CharToDecimal(fgetc(p3)) * 16 + CharToDecimal(fgetc(p3));
				oper = fgetc(p3);
				index = CharToDecimal(fgetc(p3)) * 10 + CharToDecimal(fgetc(p3));
				t_mult = 65536;
				total_addr = 0;
				for (i = 0; i * 2 < t_modi; i++) {						
					total_addr += getAddrValue(t_address + i) * t_mult;
					t_mult /= 256;
				}
				if (oper == '+')
					total_addr += r3[index].s_addr;
				else if (oper == '-')
					total_addr -= r3[index].s_addr;
				else
					return ;
				if (total_addr < 0)
					total_addr += 0x10000000;
				total_addr %= 0x1000000;
				t_mult = 65536;
				for (i = 0; i * 2 < t_modi; i++) {
					EditAddress(t_address + i, total_addr / t_mult);
					total_addr %= t_mult;
					t_mult /= 256;
				}
			}
			else if (type_check == 'E') {		//when 'E'
				t_mult = 1048576;
				t_address = 0;
				for (i = 0; i < 6; i++) {
					t_address += CharToDecimal(fgetc(p3)) * t_mult;
					t_mult /= 16;
				}
				break;
			}
			else if (type_check == '\n') {
				continue;
			}
			else
				fgets(tempstr, sizeof(tempstr), p3);
		}
		fclose(p3);
	}
	
//***************************************PASS 2_END***************************************************//
}
void get_flag(void){	//get n i x b p e
	f.n = m.mem1 % 4 / 2;
	f.i=m.mem1%2;
	f.x=m.mem2/16/8;
	f.b=m.mem2/16/4%2;
	f.p=m.mem2/16%4/2;
	f.e=m.mem2/16%2;
}
void get_pc_mem(void){		//decide format 3 or 4 and get address
 	if(f.e==1){		//case: format4
		m.mem_total=m.mem2%16*16*16*16+m.mem3*16*16+m.mem4;	//get address
		reg.PC+=4;		//set cur pc(format 4)	
	}
	else{
		m.mem_total=m.mem2%16*16*16+m.mem3;		//get disp
		if(m.mem2%16>=0x08)			//if mem2 is -,
			m.mem_total+=0xF000;
		reg.PC+=3;		//set cur pc(format 3)
	}
}
void addressing_type(void){		//get target address
	if (f.e == 1)
		target_address = m.mem_total;
	if (f.n == 0 && f.i == 1)
		target_address = m.mem_total;
	if(f.p==1)
		target_address=reg.PC+m.mem_total;	//case: pc relative
	if(f.b==1)
	 	target_address=reg.B+m.mem_total;		//case: base relative
	if(f.e==0)				//if overflow occur, make a correction.(remove high number of digits)
		target_address%=0x10000;
}
void init_fm(void){		//init n i x b p e and mem(for store memory)
	f.n=0;
	f.i=0;
	f.x=0;
	f.b=0;
	f.p=0;
	f.e=0;

	m.mem_total=0;
	m.mem1=0;
	m.mem2=0;
	m.mem3=0;
	m.mem4=0;

	target_address=0x00;
}
void clear_reg(void){
	int r=m.mem2/16;
	if(r==0)
		reg.A=0;
	else if(r==1)
	 	reg.X=0;
	else if(r==2)
	 	reg.L=0;
	else if(r==8)
		reg.PC=0;
	else if(r==3)
		reg.B=0;
	else if(r==4)
		reg.S=0;
	else if(r==5)
		reg.S=0;
	else
	 	printf("Invaild REGISTER\n");

}
void print_reg(void){
	printf("A : %06X   X : %06X\n",reg.A,reg.X);
	printf("L : %06X  PC : %06X\n",reg.L, reg.PC);
	printf("B : %06X   S : %06X\n",reg.B,reg.S);
	printf("T : %06X\n",reg.T);
}
//***************************************RUN START****************************************************//
int CC=0,exit_flag=0;

void run(void){
	int opcode,findbp=0;
	BNode* bpoint;
	while(1){
		if (reg.PC >= 0x1077) {
			if (H_bnode != NULL) {
				for (bpoint = H_bnode->next; bpoint != NULL; bpoint = bpoint->next)		//set breakpoint =1
					bpoint->onoff = 1;
			}
			print_reg();
			printf("\t\tEnd Program\n");
			reg.A = 0;
			reg.X = 0;
			reg.L = 0x1077;
			reg.PC = 0;
			reg.B = 0;
			reg.S = 0;
			reg.T = 0;
			break;
		}
		if (H_bnode != NULL) {
			for (bpoint = H_bnode->next; bpoint != NULL; bpoint = bpoint->next) {
				if (reg.PC == bpoint->bp) {		//if program meets break point,
					findbp = 1;
					break;
				}
			}
			if (findbp==1 &&bpoint!=NULL) {
				if (bpoint->onoff == 1) {
					bpoint->onoff = 0;
					findbp = 0;
					print_reg();
					printf("		Stop at checkpoint[%X]\n", bpoint->bp);
					break;
				}
			}
		}
	 	init_fm();			//initialize
		m.mem1=getAddrValue(R_address+reg.PC);		//get 1byte memory from 1MB memory
		m.mem2=getAddrValue(R_address+reg.PC+1);
		m.mem3=getAddrValue(R_address+reg.PC+2);
		m.mem4=getAddrValue(R_address+reg.PC+3);
		//if operation code exist..
		opcode=m.mem1-m.mem1%4;

		if(opcode==0x14){		//case 0x14: STL.
		 	get_flag();
			get_pc_mem();
			addressing_type();
			EditAddress(target_address,reg.L/0x10000);			//store L reg to target_mem
			EditAddress(target_address+1,(reg.L/0x100)%0x100);
			EditAddress(target_address+2,reg.L%0x100);
			
		}
		else if(opcode==0x68){	//case 0x68: LDB
			get_flag();
			get_pc_mem();
			addressing_type();
			if(f.n==0 && f.i==1)	//immediate addressing
				reg.B=target_address;		//store B<=target_address
		}
		else if(opcode==0x48){	//case 0x48: JSUB
			get_flag();
			get_pc_mem();
			addressing_type();
			reg.L=reg.PC;		//store return address to L 
			reg.PC=target_address;	//JUMP to target_address		
		}
		else if(opcode==0xB4){	//case 0xB4: CLEAR
			clear_reg();
			reg.PC+=2;
		}
		else if(opcode==0x74){	//case 0x74 :LDT
			get_flag();
			get_pc_mem();
			addressing_type();
			if(f.n==1 && f.i==1)	//simple addressing
				reg.T=getAddrValue(target_address)*0x10000+getAddrValue(target_address+1)*0x100+getAddrValue(target_address+2);
			else if(f.n==0 && f.i==1)	//immeditate addressing
				reg.T=target_address;		//store T<=target_address
		}
		else if(opcode==0xE0){	//case 0xE0 :TD
			reg.PC+=3;
			CC=-1;
		}
		else if(opcode==0x30){	//case 0x30: JEQ
			if (CC == 0) {			//if CC=0, JUMP to target_addres
				get_flag();
				get_pc_mem();
				addressing_type();
				reg.PC = target_address;
			}
			else
				reg.PC += 3;
		}
		else if(opcode==0xD8){	//case 0xD8: RD
			reg.PC+=3;
			CC=0;
		}
		else if(opcode==0xA0){	//case 0xA0: COMPR
			if(CC==0)
				reg.PC+=2;
		}
		else if(opcode==0x10){	//case 0x10: STX
			get_flag();
			get_pc_mem();
			addressing_type();
			EditAddress(target_address,reg.X);			//store x to length. length=0
		}
		else if(opcode==0x4C){	//case 0x4C: RSUB
			reg.PC=reg.L;
		}
		else if(opcode==0x00){	//case 0x00: LDA
			get_flag();
			get_pc_mem();
			addressing_type();
			if(f.n==1 && f.i==1)		//simple addressing
				reg.A=getAddrValue(target_address)*0x10000+getAddrValue(target_address+1)*0x100+getAddrValue(target_address+2);		//store target_address to A
			else if(f.n==0 && f.i==1)	//immediate addressing
				reg.A=target_address;
			else
				reg.A=target_address;
		}
		else if(opcode==0x28){	//case 0x28: COMP
			if (reg.A == 0) {
				CC = 0;
			}
			reg.PC += 3;
		}
		else if(opcode==0x0C){	//case 0x0C: STA
			get_flag();
			get_pc_mem();
			addressing_type();
			EditAddress(target_address,reg.A/0x10000);
			EditAddress(target_address+1,reg.A%0x10000/0x100);
			EditAddress(target_address+2,reg.A%0x100);
		}
		else if(opcode==0xDC){	//case 0xDc: WD
			reg.PC+=3;
		}
		else if(opcode==0x50){	//case 0x50: LDCH
			get_flag();
			get_pc_mem();
			addressing_type();
			if(getAddrValue(target_address+reg.X)==0x00)
				exit_flag=1;
			else
				reg.A=getAddrValue(target_address+reg.X);
		}
		else if(opcode==0xB8){	//case 0xB8: TIXR
		 	reg.X++;
			if(reg.X>=reg.T || exit_flag==1)
				CC=1;
			reg.PC+=2;
		}
		else if(opcode==0x38){	//case 0x38: JLT
			if(CC==1){	//end	
				reg.PC+=3;
			}
			else{
				get_flag();
				get_pc_mem();
				addressing_type();
				reg.PC=target_address;
			}
		}
		else if(opcode==0x3C){	//case 0x3C: J
			get_flag();
			get_pc_mem();
			addressing_type();
			if(f.n==1 && f.i==0){	//indirect addressing
				reg.PC=getAddrValue(target_address)*0x10000+getAddrValue(target_address+1)*0x100+getAddrValue(target_address+2);
			}
		}
	}

}
	
//***************************************RUN END******************************************************//




























