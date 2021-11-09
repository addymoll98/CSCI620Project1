/*
CSCI 620
Project 1
Due 11/19/2021
*/

//In the textbook, this stuff is on pages like 200 range-ish

#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <bits/stdc++.h>
#include <stdio.h>
#include <stdbool.h>

using namespace std;

struct ReservationStation { //-1 will indicate an empty field
//	string Name;
	bool Busy=0;
	string Op="";
	int Vj=-1;
	int Vk=-1;
	struct ReservationStation *Qj=NULL;
	struct ReservationStation *Qk=NULL;
	int A;	
};

struct latencies{ //declaring struct outside to use vectors - g++ compiler errors
    string producer;
    string consumer;
    string latency;     //storing it as a string for now, convert to int using stringstream
};

struct instructions{ //deal with reading that something is a loop..
	bool loop=0;
	string operation;
	string operands; //store all operands as one string for now, parse after reading operation
};

/*

struct InstructionStat{
	bool Issue=0;
	bool Execute=0;
	bool WriteResult=0;
}



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


*/

int main(){
	
	string projectLatencies;
	string projectInstructions;
	int i = 0;
	int j=0;
	
	
	
//Open latency file
	
	cout << "File path for Latencies file: ";
	cin >> projectLatencies; 
	cout << "File path is" << projectLatencies << endl;
	
	vector<latencies> l; //vector named l for latencies struct

    ifstream latFile(projectLatencies.c_str());
    
	i=0;
    while(!latFile.eof()) {
        l.push_back(latencies()); //pushing a latencies struct object into the vector every time
        latFile >> l[i].producer >> l[i].consumer >> l[i].latency; //then adding the values from the latencies file into that particular struct object
        i++;
    }
    latFile.close();
	
	//store latencies into individual variables
	
	int FPMUL, FPDIV, FPADD, FPLD, FPALU, LDINT, INT;

	for(int i=0; i<l.size(); i++){ 
		stringstream ss;
		if (l[i].producer=="FPMUL"){
			ss << l[i].latency;
			ss >> FPMUL;
		}
		else if (l[i].producer=="FPDIV"){
			ss <<l[i].latency;
			ss >> FPDIV;
		}
		else if (l[i].producer=="FPADD"){
			ss <<l[i].latency;
			ss >> FPADD;
		}
		else if (l[i].producer=="FPLD"){
			ss <<l[i].latency;
			ss >> FPLD;
		}
		else if (l[i].producer=="FPALU"){
			ss <<l[i].latency;
			ss >> FPALU;
		}
		else if (l[i].producer=="LDINT"){
			ss <<l[i].latency;
			ss >> LDINT;
		}
		else if (l[i].producer=="INT"){
			ss <<l[i].latency;
			ss >> INT;	
		}			
	}
	
	//display all to verify correctness
	
	cout<< FPMUL << endl;
	cout<< FPDIV << endl; 
	cout<< FPADD << endl; 
	cout<< FPLD << endl; 
	cout<< FPALU << endl; 
	cout<< LDINT << endl; 
	cout<< INT << endl;

	
//Open instruction file
	
	cout << "File path for Instruction input file: ";
	cin >> projectInstructions;
	cout << "File path is" << projectInstructions <<endl;
	
	vector<instructions> ins; //vector names ins for instructions struct
	
	ifstream insFile(projectInstructions.c_str());

	i=0; 
	
	while(!insFile.eof()) {
        ins.push_back(instructions()); //pushing a instructions struct object into the vector every time
		insFile>> ins[i].operation; //add the value from the instructions file into operation struct object
		if (ins[i].operation=="loop:"){ //check if the value added was "loop". If yes, then mark loop as true and add operation
			ins[i].loop=1;
			insFile >> ins[i].operation;
		}
        insFile >> ins[i].operands; //then adding the operands from the instructions file into operands object
        i++;
    }
    insFile.close();
	
	
	
	//prints out input for testing purposes
	for (int i=0; i<ins.size(); i++){
		if (ins[i].loop==1)
			cout<< "loop: ";
		cout << ins[i].operation << " " << ins[i].operands << endl;
	}

	
	
	
/*MAIN PART OF PROGRAM
	
//Initialize reservation stations
	
	ReservationStation Load1;
	ReservationStation Load2;
	ReservationStation Add1;
	ReservationStation Add2;
	ReservationStation Add3;
	ReservationStation Mult1;
	ReservationStation Mult2;

//Initialize instructionStatus array based on the number of instructions
//instructionStatus[i][0]==issue cc, [i][1]==execute cycle, [i][2]== write cycle
	int instructionStatus[ins.size()][3] ;//assumes for now no more than 20 instructions
	for(i=0; i<ins.size(); i++){
		for (j=0; j<3; j++){
			instructionStatus[i][j]=0;
		}
	}
	
	bool done=0;
	int cc=0;
	int instructionNumber=0;
	while(!done){
		cc++; //increment the clock cycle
		//read an input line from the ins vector

		if (ins[instructionNumber].operation=="FLD" | ins[instructionNumber].operation== "FSD"){
				//readFLS(instuctionInput);
		}
		else if (ins[instructionNumber].operation=="FADD.D" | ins[instructionNumber].operation=="FSUB.D" | ins[instructionNumber].operation=="FMUL.D" | ins[instructionNumber].operation=="FDIV.D"){
				//readFPop(instructionInput);
		}
		else if (ins[instructionNumber].operation== "ADD" | ins[instructionNumber].operation== "SUB"){
				//readADD(instructionInput);
		}
		else if (ins[instructionNumber].operation=="LD" | ins[instructionNumber].operation=="SD"){
				//readLS(instructionInput);
		}
		else if (ins[instructionNumber].operation=="ADDI" | ins[instructionNumber].operation=="SUBI"){
				//readADDI(instructionInput);
		}
		else if (ins[instructionNumber].operation=="BNEZ"){
			//readBNEZ(instructionInput);
		}


		//check if any of the instrucitons that have not begun execution yet now have only null pointers in Qj and Qk
		//(that is, they either just got issued last cc and don't need to wait on anything, or what they're waiting on wrote last cc)
			//if yes, change InstructionStat[i][2] to cc, they can begin execution.
					//change write InstructionStat[i][3]=cc+latency for this instr typede
					//except, while InstructionStat[all][3]== cc+latency for any instruction, then add 1 to that value
			
		if (instructionNumber<=ins.size()){
			//check if any reservation stations are available. If not, then we will read  instruciton next time.
				//if reservation station avaialble:
					//load instruciton itno reservation station
					instructionNumber++
		}	
		
		//Check if anything writes to the CDB this clock cyle, ie if instructionStatus[j][3]==cc for any instruction
				//ie, it's writing ot the CDB this CC
				//check if anything of the reservation stations point to its reservation station
					//if yes, clear null those pointers
					//set the reservation stations to available 
		
		cc++;
		
			
		//after doing all necissary updates for that clock cycle, check if any of the instructions are not complete
		//if everything is complete, then set done=1 to exit the loop
		for (i=0; i<ins.size(); i++){
			done=1; //set to done, and change to not done if any instruction is not complete
			if (instructionStatus[i][3]==0){
				done=0;
			}
		}
		
		
		done=1; //done to exit loop until I get to the done logic
	}
	

	return 0;
*/
}