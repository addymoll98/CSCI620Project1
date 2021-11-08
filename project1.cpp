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
//	fstream latencies;
	string instructionInput;
	fstream instuctionInput;
	
	//instructionStatus[i][0]==issue cc, [i][1]==execute cycle, [i][2]== write cycle
	int instructionStatus[20][3] ;//assumes for now no more than 20 instructions
	for(int i=0; i<20; i++){
		for (int j=0; j<3; j++){
			instructionStatus[i][j]=0;
		}
	}

	//initialize reservation stations, start with 2 load, 3 add, 2 mult. Are these specified somewhere??
	// ReservationStation RS[7];
	// RS[0].Name="Load1"; RS[1].Name="Load2"; 
	// RS[2].Name="Add1"; RS[3].Name="Add2"; RS[4].Name="Add3";
	// RS[5].Name="Mult1"; RS[6].Name="Mult2";
	// for(int i=0; i<7; i++){
		// RS[i].Busy=0;
		// RS[i].Op="";
		// RS[i].Vj=-1;
		// RS[i].Vk=-1;
		// RS[i].Qj="";
		// RS[i].Qk="";
		// RS[i].A=-1;
	// }
	
	ReservationStation Load1;
	ReservationStation Load2;
	ReservationStation Add1;
	ReservationStation Add2;
	ReservationStation Add3;
	ReservationStation Mult1;
	ReservationStation Mult2;
	
	// struct ReservationStation RS[]={ 
		// {Name="Load1", Busy=0, Op="", Vj=-1, Vk=-1, Qj="", Qk="", A=-1},
		// {Name="Load2", Busy=0, Op="", Vj=-1, Vk=-1, Qj="", Qk="", A=-1},
		// {Name="Add1", Busy=0, Op="", Vj=-1, Vk=-1, Qj="", Qk="", A=-1},
		// {Name="Add2", Busy=0, Op="", Vj=-1, Vk=-1, Qj="", Qk="", A=-1},
		// {Name="Add3", Busy=0, Op="", Vj=-1, Vk=-1, Qj="", Qk="", A=-1},
		// {Name="Mult1", Busy=0, Op="", Vj=-1, Vk=-1, Qj="", Qk="", A=-1},
		// {Name="Mult2", Busy=0, Op="", Vj=-1, Vk=-1, Qj="", Qk="", A=-1},
	// };
	
	//initialize reservation stations
	
	
	//Open project files
	
	cout << "File path for Latencies file: ";
	cin >> projectLatencies; 
	cout << "File path is" << projectLatencies << endl;
	
	//Sohaib's version:
	
	vector<latencies> l; //vector named l for latencies struct

    ifstream latFile(projectLatencies.c_str());

    int i = 0;

    while(!latFile.eof()) {
        l.push_back(latencies()); //pushing a latencies struct object into the vector every time
        latFile >> l[i].producer >> l[i].consumer >> l[i].latency; //then adding the values from the latencies file into that particular struct object
        i++;
    }
    latFile.close();
	
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
	
	cout<< FPMUL << endl;
	cout<< FPDIV << endl; 
	cout<< FPADD << endl; 
	cout<< FPLD << endl; 
	cout<< FPALU << endl; 
	cout<< LDINT << endl; 
	cout<< INT << endl;

	
	
	
	cout << "File path for Instruction inpu file: ";
	cin >> instructionInput;
	cout << "File path is" << instructionInput <<endl;
	
	instuctionInput.open(instructionInput.c_str());
	
	while (!instuctionInput){
		cout << "File could not be read. Reenter file path for instuctionInput file: ";
		cin >> instructionInput; 
		cout << "File path is" << instructionInput;
		instuctionInput.open(instructionInput.c_str());
	}
	
	
	//MAIN PART OF PROGRAM
	
	//READ AN INPUT LINE
	//ISSUE IT DURING THAT CLOCK CYCLE
	
	
	
	bool done=0;
	int cc=0;
	int instructionNumber=0;
	int loop;
	while(!done){
		cc++; //increment the clock cycle
		string operation, operands;
		
		//read an input line *****IGNORING LOOPS FOR NOW, COME BACK AND FIX THAT!!*********
		instructionInput >> operation >> operands;
		if (operation=="loop:"){
			loop=instructionNumber;
			operation=operands;
			instructionInput >> operands;
			//plus, whatever else needs to be done on a loop?
		}
		
		if (operation=="FLD" | operation== "FSD"){
			//readFLS(instuctionInput);
		}
		else if (operation=="FADD.D" | operation=="FSUB.D" | operation=="FMUL.D" | operation=="FDIV.D"){
				//readFPop(instructionInput);
		}
		else if (operation== "ADD" | operation== "SUB"){
				//readADD(instructionInput);
		}
		else if (operation=="LD" | operation=="SD"){
				//readLS(instructionInput);
		}
		else if (operation=="ADDI" | operation=="SUBI"){
				//readADDI(instructionInput);
		}
		else if (operation=="BNEZ"){
			//readBNEZ(instructionInput);
		}

		
		
		//If reservation station avaiable , **FOR NOW, ASSUME THEY ARE, HOW MANY ARE THERE??**
			
			//issue
			
			//write the clock cycle in the table
			
			//Go back and check on working instructions
				
				//if done w/ execute
					//write to cdb
					//write clock cycle in the table
					
				//if in issue
					//check if instructions are available and move to execute
					//write the clock cycle in the table
		
		//while reservation station not abailable, 
			
			//Go back and check on working instructions
				
				//if done w/ execute
					//write to cdb
					//write clock cycle in the table
					
				//if in issue
					//check if instructions are available and move to execute
					//write the clock cycle in the table
			//increment the clock cycle
			
		instructionNumber++;
	}
	

	return 0;
}