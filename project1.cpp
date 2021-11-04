/*
CSCI 620
Project 1
Due 11/19/2021
*/

//In the textbook, this stuff is on pages like 200 range-ish

#include <iostream>
#include <stdio.h>
#include <stdbool.h>

using namespace std;

struct InstructionStat{
	bool Issue=0;
	bool Execute=0;
	bool WriteResult=0;
}

struct ReservationStation (
	int Op; //operation, using a numberical code?
	int Vj;
	
)

//Hmm. not really sure yet what to do here lol, probably not a struct but some array or something
struct RegisterStat(
	int RegisterStat[]
)



void IssueFP(int r, int rs, int Qi, int Qj, int Vj){
	if(RegisterStat[rs].Qi!=0){
		(RS[r].Qj)=RegisterStat[rs].Qi);
	}
	else {
		RS[r].Vj=Regs[rs];
		RS[r].Qj=0;
	}
	if(RegisterStat[rt].Qi!=0){
		RS[r].Qk=RegisterStat[rt].Qi;
	}
	else {
		RS[r].VK=RegisterStat[rt].Qi;
	}
	RS[r].Busy=1; //RS[r] is busy
	RegisterStat[rd]/Q=r;
}

void IssueLS(int r, int rs, int Qi, int Qj, int Vj){
	if(RegisterStat[rs].Qi!=0){
		RS[r].Qj=RegisterStat[rs].Qi;
	}
	else{
		RS[r].Vj=Regs[rs];
		RS[r].Qj=0;
	}
	RS[r].A=imm; //imm is the sign extended immediate field
	RS[r].Busy=1; //RS[r] is busy
}

void IssueL(int r, int rs, int Qi){
	RegisterStat[rt].Qi=r;
}

void IssueS(int r, int rs, int Qi, int Qk, int Vk){
	if(RegisterStat[rt].Qi!=0){
		RS[r].Qk=RegisterStat[rs].Qi;
	}
	else{
		RS[r].Vk=Regs[rt];
		RS[r].Qk=0;
	}
}

double ExecuteFP(double Vj, double Vk){
	double result;
	//compute results for all fp operations and wait times etc. Operands in Vj and Vk
	return result;
}

void ExecuteLS1(){
	RS[r].A=RS[r].Vj+RS[r].A;
	ExecuteLS2();
}

void ExecuteLS2(){
	//Read from Mem[RS[r].A]
}

void Write(int result, int r, int Qr, int Qj, int Vj, int Vk){
	for(int x=0; x<total; x++){ //total global maybe?
		if(RegisterStat[x].Qi=r){
			Regs[x]=result;
			RegisterStat[x].Qi=0;
		}
		if(RS[x].Qj=r){
			RS[x].Vj=result;
			RS[x].Qj=0;
		}
		if(RS[x].Qr=r){
			RS[x].Vk=result;
			RS[x].Qk=0;
		}
	} 
	RS[r].Busy=0;
}

void Store(){
	Mem[RS[r].A]=RS[r].Vk;
	RS[r].Busy=0;
}

int main(){
	
	int Op; //The operation to perform on source operands S1 and S2
	int Qj, Qk; //The reservation stations
	int Vj, Vk; //The value of the source operands. 
	int A; //infomraiton for memory address calculation, load or store
	bool Busy; //indicates reservation station and accompanying functional unit are occupied
	int Qi; //number of the reservatoin station containing operation whos resulte is stored in register
	
	/*
	if FP operation, wait until Station r empty, then IssueFP()
	*/	
	
	/*
	if Load or store, wait until Buffer r empty, then IssueLS()
	*/
	
	/*
	If load only IssueL()
	*/

	/*
	If Store only IssueS()
	*/
	
	/*
	If RS[r].Qj=0 & (RS[r].Qk=0), ExecuteFP()
	*/
	
	/*
	If RS[r].Qj=0 & r is head of load-store queue, ExecuteLS1()
	*/
	
	/*
	If execution complete at r and CDB avaialble, Write()
	*/
	
	/*
	execution complete at r and RS[r].Qk=0, Store()
	*/
	
	
	
	return 0;
}