// CSCI620.cpp : This file contains the 'main' function. Program execution begins and ends there.


#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <map> 
#include<iomanip>
using namespace std;

struct latencies { //declaring struct outside to use vectors - g++ compiler errors
	string producer;
	string consumer;
	string latency;     //storing it as a string for now, convert to int using stringstream
};

class Instruction
{
public:
//	string fullOp;
	int rd;
	int rs;
	int rt;
	int opcode;
	int issueClock;
	int executeClockBegin;
	int executeClockEnd;
	int writebackClock;

	Instruction()
	{
//		fullOp="";
		rd = 0;
		rs = 0;
		rt = 0;
		opcode = 0;
		issueClock = 0;
		executeClockBegin = 0;
		executeClockEnd = 0;
		writebackClock = 0;
	}

	Instruction(int RD, int RS, int RT, int OPCODE)
	{
		rd = RD;
		rs = RS;
		rt = RT;
		opcode = OPCODE;
		issueClock = 0;
		executeClockBegin = 0;
		executeClockEnd = 0;
		writebackClock = 0;

	}
};

class reservationStation
{
public:
	bool busy;
	int Qj;
	int Qk;
	int Vj;
	int Vk;
	int lat;
	int op;
	int a;
	int result;
	bool resultReady;
	int instNum;
	int ISSUE_Lat;
	int WRITEBACK_Lat;

	reservationStation() {
		busy = false;
		op = 0;
		lat = 0;
		result = 0;
		resultReady = false;
		Qj = 1;
		Qk = 1;
		Vj = 0;
		a = 999;
		Vk = 0;
		instNum = 100000;
		ISSUE_Lat = 0;
		WRITEBACK_Lat = 0;
	}
	reservationStation(int op1, int rsop, int ea) {
		busy = false;
		op = op1;
		lat = 0;
		result = 0;
		resultReady = false;
		Qj = rsop;
		Qk = rsop;
		a = ea;
		Vj = 0;
		Vk = 0;
		instNum = 100000;
		ISSUE_Lat = 0;
		WRITEBACK_Lat = 0;
	}

};
class registerStatus {
public:
	bool busy;
	int Qi;
	registerStatus()
	{
		busy = false;
		Qi = 0;
	}
	registerStatus(int regEmpty)
	{
		busy = false;
		Qi = regEmpty;
	}
};

const int Num_ADD_RS = 4;
const int Num_MULT_RS = 2;
const int Num_DIV_RS = 3;
// Datapath Latency
const int ISSUE_Lat = 1;
const int WRITEBACK_Lat = 1;
// Global Clock
int Clock = 0;
// used to check if INST == WRITEBACKS to end program
bool Done = true;
int Total_WRITEBACKS = 0;
// Counter for instructions; Used to end issue if value is equal to length of instructions
int currentInst_ISSUE = 0;
// Temporary fix for errors due to RS names being numbers
// -> errors with REG/RS/REGSTATUS == zero
const int ZERO_REG = 5000;
const int regEmpty = 1000;
const int OperandAvailable = 1001;
const int OperandInit = 1002;
bool nextinstructionafterbranch = false;
int ipos = 0; // used for adding next iteration instruction into instruction set. Incremented in branch condition of execute function.
bool loopclock = false;

int issue(vector<Instruction>& inst1, vector<reservationStation>& resstation1, vector<registerStatus>& regstatus1, vector<int>& reg1, map<string, int> OPERATION) {

	// Latency of 1 if issued
	//**** check if spot in given reservation station is available
	int r = 0;
	bool rstationFree = false;
	// r is the current instruction to be issued's operation
	// code(add,sub,mult,div)
	// If all instructions have been issued then return 0 and exit issue function
	if (currentInst_ISSUE >= inst1.size())
		return 0;
	r = inst1[currentInst_ISSUE].opcode;
	// determine if there is an open RS of r type. if yes
	// -> r = that open spot.
	// Boundry's of given RS
	int addresst = 4;
	int mulresst = 8;
	int divresst = 12;
	int ldsdresst = 16;
	int branchresst = 19;
	int rstno = 999;//reservation station number. Initializer to avoid random values.
	if (r == 0) // if op is FADD
	{
		for (int i = 0; i < addresst; i++) {
			if (!resstation1[i].busy) {
				rstno = i;
				resstation1[i].op = 0;
				rstationFree = true;
				currentInst_ISSUE++;
				break;
			}
		}
		if (!rstationFree)
			return 1;
	}
	if (r == 1) // if op is FSUB
	{
		for (int i = 0; i < addresst; i++) {
			if (!resstation1[i].busy) {
				rstno = i;
				resstation1[i].op = 1;
				rstationFree = true;
				currentInst_ISSUE++;
				break;
			}
		}
		if (!rstationFree)
			return 1;
	}
	if (r == 2)// if op is FMUL
	{
		for (int i = 4; i < mulresst; i++) {
			if (!resstation1[i].busy) {
				rstno = i;
				resstation1[i].op = 2;
				rstationFree = true;
				currentInst_ISSUE++;
				break;
			}
		}
		if (!rstationFree)
			return 1;

	}
	if (r == 3)// if op is FDIV
	{
		for (int i = 8; i < divresst; i++) {
			if (!resstation1[i].busy) {
				rstno = i;
				resstation1[i].op = 3;
				rstationFree = true;
				currentInst_ISSUE++;
				break;
			}
		}
		if (!rstationFree)
			return 1;
	}
	if (r == 4)//for load
	{
		cout<<"in issue load"<<endl;
		for (int i = 12; i < ldsdresst; i++) {
			if (!resstation1[i].busy) {
				rstno = i;
				resstation1[i].op = 4;
				rstationFree = true;
				currentInst_ISSUE++;
				resstation1[i].a = 0;
				break;
			}
		}
		if (!rstationFree)
			return 1;
	}
	if (r == 5)//for store
	{
		cout<<"in issue store"<<endl;
		for (int i = 12; i < ldsdresst; i++) {
			if (!resstation1[i].busy) {
				rstno = i;
				resstation1[i].op = 5;
				rstationFree = true;
				currentInst_ISSUE++;
				resstation1[i].a = 1;
				break;
			}
		}
		if (!rstationFree)
			return 1;
	}
	if (r == 6) // if op is ADD
	{
		for (int i = 0; i < addresst; i++) {
			if (!resstation1[i].busy) {
				rstno = i;
				resstation1[i].op = 6;
				rstationFree = true;
				currentInst_ISSUE++;
				break;
			}
		}
		if (!rstationFree)
			return 1;
	}

	if (r == 7) // if op is SUB
	{
		for (int i = 0; i < addresst; i++) {
			if (!resstation1[i].busy) {
				rstno = i;
				resstation1[i].op = 7;
				rstationFree = true;
				currentInst_ISSUE++;
				break;
			}
		}
		if (!rstationFree)
			return 1;
	}
	if (r == 8) // if op is ADDI
	{
		for (int i = 0; i < addresst; i++) {
			if (!resstation1[i].busy) {
				rstno = i;
				resstation1[i].op = 8;
				rstationFree = true;
				currentInst_ISSUE++;
				resstation1[i].a = 2;
				break;
			}
		}
		if (!rstationFree)
			return 1;
	}

	if (r == 9) // if op is SUBI
	{
		for (int i = 0; i < addresst; i++) {
			if (!resstation1[i].busy) {
				rstno = i;
				resstation1[i].op = 9;
				rstationFree = true;
				currentInst_ISSUE++;
				resstation1[i].a = 2;
				break;
			}
		}
		if (!rstationFree)
			return 1;
	}
	if (r == 10)//for loadint
	{
		for (int i = 12; i < ldsdresst; i++) {
			if (!resstation1[i].busy) {
				rstno = i;
				resstation1[i].op = 10;
				rstationFree = true;
				currentInst_ISSUE++;
				resstation1[i].a = 0;
				break;
			}
		}
		if (!rstationFree)
			return 1;
	}
	if (r == 11)//for storeint
	{
		for (int i = 12; i < ldsdresst; i++) {
			if (!resstation1[i].busy) {
				rstno = i;
				resstation1[i].op = 11;
				rstationFree = true;
				currentInst_ISSUE++;
				resstation1[i].a = 1;
				break;
			}
		}
		if (!rstationFree)
			return 1;
	}
	if (r == 50)// for BNEZ Operation
	{
		std::cout << "Inside bnez";
		for (int i = 16; i < branchresst; i++) {
			if (!resstation1[i].busy) {
				rstno = i;
				resstation1[i].op = 50;
				rstationFree = true;
				currentInst_ISSUE++;
				resstation1[i].a = 3; // value is 3 for branch instruction
				break;
			}
		}
		if (!rstationFree)
			return 1;
	}
	if (r == 51)// for BEQZ Operation
	{
		std::cout << "Inside beqz";
		for (int i = 16; i < branchresst; i++) {
			if (!resstation1[i].busy) {
				rstno = i;
				resstation1[i].op = 51;
				rstationFree = true;
				currentInst_ISSUE++;
				resstation1[i].a = 3; // value is 3 for branch instruction
				break;
			}
		}
		if (!rstationFree)
			return 1;
	}

	//regstatus1[inst1[currentInst_ISSUE - 1].rd].Qi = rstno;// initialize destination register status to reservation station
	//**** Initialize characteristics of issued instruction
	// if operand rs is available -> set value of operand
	// (Vj) to given register value
	// else point operand to the reservation station (Qj)
	// that will give the operand value
	// NOTE: since currentInst was in incremented we must
	// do currentINST_ISSUE-1


			//check if rs register value is default. If it is empty it means that the rs value is available
			// if operand rs is available -> set value of
			// operand (Vj) to given register value
			// else point operand to the reservation station
			// (Qj) that will give the operand value

	//for load logic - fix
	if (resstation1[rstno].a == 0)
	{
		// check if rs field offset is available. We are not calculating but just checking if the register is available. 
		if (regstatus1[inst1[currentInst_ISSUE - 1].rs].Qi == regEmpty) {
			resstation1[rstno].Vj = reg1[inst1[currentInst_ISSUE - 1].rs];
			resstation1[rstno].Qj = OperandAvailable;
		}
		else {
			resstation1[rstno].Qj = regstatus1[inst1[currentInst_ISSUE - 1].rs].Qi;
		}
	}
	// for ALU Logic like FADD.D,FSUB.D,...
	if (resstation1[rstno].a == 999)
	{
		if (regstatus1[inst1[currentInst_ISSUE - 1].rs].Qi == regEmpty) {
			resstation1[rstno].Vj = reg1[inst1[currentInst_ISSUE - 1].rs];
			resstation1[rstno].Qj = OperandAvailable;
		}
		else {
			resstation1[rstno].Qj = regstatus1[inst1[currentInst_ISSUE - 1].rs].Qi;
		}
		// if operand rt is available -> set value of
		// operand (Vk) to given register value
		// else point operand to the reservation station
		// (Qk) that will give the operand value
		if (regstatus1[inst1[currentInst_ISSUE - 1].rt].Qi == regEmpty) {
			resstation1[rstno].Vk = reg1[inst1[currentInst_ISSUE - 1].rt];
			resstation1[rstno].Qk = OperandAvailable;
		}
		else {
			resstation1[rstno].Qk = regstatus1[inst1[currentInst_ISSUE - 1].rt].Qi;
		}




	}

	//for store logic - fix
	if (resstation1[rstno].a == 1) 
	{
		// check if rd is empty, if it's empty then we know rd value is available else point to the reservation station which returns rd value
		if (regstatus1[inst1[currentInst_ISSUE - 1].rd].Qi == regEmpty) {
			resstation1[rstno].Vj = reg1[inst1[currentInst_ISSUE - 1].rd];
			resstation1[rstno].Qj = OperandAvailable;
		}
		else {
			resstation1[rstno].Qj = regstatus1[inst1[currentInst_ISSUE - 1].rd].Qi;
		}
	}

	//for ADDI,SUBI Logic
	if (resstation1[rstno].a == 2)
	{
		//check for availability of rs field. if its not available point to reservation station
		if (regstatus1[inst1[currentInst_ISSUE - 1].rs].Qi == regEmpty) {
			resstation1[rstno].Vj = reg1[inst1[currentInst_ISSUE - 1].rs];
			resstation1[rstno].Qj = OperandAvailable;
		}
		else {
			resstation1[rstno].Qj = regstatus1[inst1[currentInst_ISSUE - 1].rs].Qi;
		}
		resstation1[rstno].Vk = inst1[currentInst_ISSUE - 1].rt; // store the rt value in Vk 
	}

	//for BNEZ, BEZ logic
	if (resstation1[rstno].a == 3)
	{
		//set loop clock to avoid redundant cc
		loopclock = true;
		// check if rs field offset is available. We are not calculating but just checking if the register is available. 
		if (regstatus1[inst1[currentInst_ISSUE - 1].rd].Qi == regEmpty) {
			resstation1[rstno].Vj = reg1[inst1[currentInst_ISSUE - 1].rs];
			resstation1[rstno].Qj = OperandAvailable;
		}
		else {
			resstation1[rstno].Qj = regstatus1[inst1[currentInst_ISSUE - 1].rd].Qi;
		}
		auto it = OPERATION.find("loop");

		if (it == OPERATION.end()) {
			// not found  
			std::cout << "Element not found";
		}
		else {
			// found  
			resstation1[rstno].Vk = it->second;//store the loop begining line to vk.
			
		}
		cout<<"at end of that loop, and issue lat is: "<<resstation1[rstno].ISSUE_Lat<<endl;
	}
	



	// given reservation station is now busy
	// until write back stage is completed.
	resstation1[rstno].busy = true;
	resstation1[rstno].ISSUE_Lat = 0;
	// set clock cycle for issue time
	inst1[currentInst_ISSUE - 1].issueClock = Clock;
	// The register status Qi is set to the current
	// instructions reservation station location r
	regstatus1[inst1[currentInst_ISSUE - 1].rd].Qi = rstno;




	// For Load Instruction - Ignore this code snippet
	/*if (resstation1[rstno].a == 0)
	{
		//Handle ld instuction
		resstation1[rstno].Vj = 25;
		resstation1[rstno].Vk = 25;
		resstation1[rstno].Qj = OperandAvailable;
		resstation1[rstno].Qk = OperandAvailable;
		// given reservation station is now busy
		// until write back stage is completed.
		resstation1[rstno].busy = true;
		resstation1[rstno].ISSUE_Lat = 0;
		// set reservation station instuction
		// number == current instruction

		// set clock cycle for issue time
		inst1[currentInst_ISSUE - 1].issueClock = Clock;

	}*/
	//set instNum property of reservation station to current Instruction number. Used in execute and writback to set clock value for execute & writeback
	resstation1[rstno].instNum = currentInst_ISSUE - 1;

	return 2;
}

// execute without loop
/*void execute(vector<Instruction>& inst1, vector<reservationStation>& resstation1, vector<registerStatus>& regstatus1, vector<int>& reg1, int FPMUL, int FPDIV, int FPADD, int FPLD, int FPALU, int LDINT, int INT)
{

	for (int i = 0; i < resstation1.size(); i++)
	{
		if (resstation1[i].busy == true)//check if any reservation station is busy
		{
			if (resstation1[i].ISSUE_Lat >= ISSUE_Lat)//check if issue latency is equal to 1 else wait for 1 cc
			{

				if ((resstation1[i].Qj == OperandAvailable && resstation1[i].Qk == OperandAvailable) || (resstation1[i].Qj == OperandAvailable && resstation1[i].a == 0) || (resstation1[i].Qj == OperandAvailable && resstation1[i].a == 1) || (resstation1[i].Qj == OperandAvailable && resstation1[i].a == 2))// check for operandavailable flag in Qj & Qk and QJ and a value for load and stores
				{
					if (inst1[resstation1[i].instNum].executeClockBegin == 0)// check if the executeclockbegin is having default value. Use instNum variable find the instruction number
					{
						inst1[resstation1[i].instNum].executeClockBegin = Clock;// if it is issue the current clock value as execution start cycle
					}
					resstation1[i].lat++;// Increment the latency to match the latency of FMUL,FADD,FSUB etc. If latency matches means that we can perform the execution
					int temp_operation = resstation1[i].op; // store operation type
					if (temp_operation == 0)// means FADD Operation
					{
						if (resstation1[i].lat == FPADD)
						{
							//perform addition
							resstation1[i].result = resstation1[i].Vj + resstation1[i].Vk;
							//we can now complete the execution and update the resultReady flag
							resstation1[i].resultReady = true;
							//reset the latency property
							resstation1[i].lat = 0;
							resstation1[i].ISSUE_Lat = 0;
							//set the Executeclock end
							inst1[resstation1[i].instNum].executeClockEnd = Clock;
						}


					}
					if (temp_operation == 1)// means FSUB Operation
					{
						if (resstation1[i].lat == FPADD)
						{
							//perform subtraction
							resstation1[i].result = resstation1[i].Vj - resstation1[i].Vk;
							//we can now complete the execution and update the resultReady flag
							resstation1[i].resultReady = true;
							//reset the latency property
							resstation1[i].lat = 0;
							resstation1[i].ISSUE_Lat = 0;
							//set the Executeclock end
							inst1[resstation1[i].instNum].executeClockEnd = Clock;
						}


					}
					if (temp_operation == 2)// means FMUL Operation
					{
						if (resstation1[i].lat == FPMUL)
						{
							//perform multiplication
							resstation1[i].result = resstation1[i].Vj * resstation1[i].Vk;
							//we can now complete the execution and update the resultReady flag
							resstation1[i].resultReady = true;
							//reset the latency property
							resstation1[i].lat = 0;
							resstation1[i].ISSUE_Lat = 0;
							//set the Executeclock end
							inst1[resstation1[i].instNum].executeClockEnd = Clock;
						}


					}
					if (temp_operation == 3)// means FDIV Operation
					{
						if (resstation1[i].lat == FPDIV)
						{
							//perfrom division
							resstation1[i].result = resstation1[i].Vj / resstation1[i].Vk;
							//we can now complete the execution and update the resultReady flag
							resstation1[i].resultReady = true;
							//reset the latency property
							resstation1[i].lat = 0;
							resstation1[i].ISSUE_Lat = 0;
							//set the Executeclock end
							inst1[resstation1[i].instNum].executeClockEnd = Clock;
						}


					}

					if (temp_operation == 4)// means fld
					{
						if (resstation1[i].lat == FPLD)
						{

							resstation1[i].result = resstation1[i].Vj;
							//we can now complete the execution and update the resultReady flag
							resstation1[i].resultReady = true;
							//reset the latency property
							resstation1[i].lat = 0;
							resstation1[i].ISSUE_Lat = 0;
							//set the Executeclock end
							inst1[resstation1[i].instNum].executeClockEnd = Clock;

						}


					}

					if (temp_operation == 5)// means fsd
					{
						if (resstation1[i].lat == FPALU)
						{
							resstation1[i].result = resstation1[i].Vj;
							//we can now complete the execution and update the resultReady flag
							resstation1[i].resultReady = true;
							//reset the latency property
							resstation1[i].lat = 0;
							resstation1[i].ISSUE_Lat = 0;
							//set the Executeclock end
							inst1[resstation1[i].instNum].executeClockEnd = Clock;

						}


					}
					if (temp_operation == 6 || temp_operation == 8)// means ADD,ADDI Operation
					{
						if (resstation1[i].lat - 1 == INT) // by default lat will start in 1. Since value for Inetger operations is zero we do -1 to avoid infinite loop scenario.
						{
							//perform addition
							resstation1[i].result = resstation1[i].Vj + resstation1[i].Vk;
							//we can now complete the execution and update the resultReady flag
							resstation1[i].resultReady = true;
							//reset the latency property
							resstation1[i].lat = 0;
							resstation1[i].ISSUE_Lat = 0;
							//set the Executeclock end
							inst1[resstation1[i].instNum].executeClockEnd = Clock;
						}


					}
					if (temp_operation == 7 || temp_operation == 9)// means SUB,SUBI Operation
					{
						if (resstation1[i].lat - 1 == INT) // by default lat will start in 1. Since value for Inetger operations is zero we do -1 to avoid infinite loop scenario.
						{
							//perform addition
							resstation1[i].result = resstation1[i].Vj - resstation1[i].Vk;
							//we can now complete the execution and update the resultReady flag
							resstation1[i].resultReady = true;
							//reset the latency property
							resstation1[i].lat = 0;
							resstation1[i].ISSUE_Lat = 0;
							//set the Executeclock end
							inst1[resstation1[i].instNum].executeClockEnd = Clock;
						}


					}

					if (temp_operation == 10)// means LD
					{
						if (resstation1[i].lat == LDINT)
						{

							resstation1[i].result = resstation1[i].Vj;
							//we can now complete the execution and update the resultReady flag
							resstation1[i].resultReady = true;
							//reset the latency property
							resstation1[i].lat = 0;
							resstation1[i].ISSUE_Lat = 0;
							//set the Executeclock end
							inst1[resstation1[i].instNum].executeClockEnd = Clock;

						}


					}
					if (temp_operation == 11)// means SD
					{
						if (resstation1[i].lat -1 == INT)
						{

							resstation1[i].result = resstation1[i].Vj;
							//we can now complete the execution and update the resultReady flag
							resstation1[i].resultReady = true;
							//reset the latency property
							resstation1[i].lat = 0;
							resstation1[i].ISSUE_Lat = 0;
							//set the Executeclock end
							inst1[resstation1[i].instNum].executeClockEnd = Clock;

						}


					}
				}
			}
			else
			{
				resstation1[i].ISSUE_Lat++;
				//we wait for 1 clock cycle after issuing instruction
			}
		}
	}

}*/

//execute with loop
void execute(vector<Instruction>& inst1, vector<reservationStation>& resstation1, vector<registerStatus>& regstatus1, vector<int>& reg1, int FPMUL, int FPDIV, int FPADD, int FPLD, int FPALU, int LDINT, int INT,int lst, int led, vector<string>& STRING_INST1, vector<Instruction> INST_AFTERLOOP)
{


	for (int i = 0; i < resstation1.size(); i++)
	{

		if (resstation1[i].busy == true)//check if any reservation station is busy
		{
			if (resstation1[i].ISSUE_Lat >= ISSUE_Lat)//check if issue latency is equal to 1 else wait for 1 cc
			{
				if (nextinstructionafterbranch)
				{
					nextinstructionafterbranch = false;
					break;//used for stalling instruction in the next iteration after the branch instruction
				}
				if ((resstation1[i].Qj == OperandAvailable && resstation1[i].Qk == OperandAvailable) || (resstation1[i].Qj == OperandAvailable && resstation1[i].a == 0) || (resstation1[i].Qj == OperandAvailable && resstation1[i].a == 1) || (resstation1[i].Qj == OperandAvailable && resstation1[i].a == 2) || (resstation1[i].Qj == OperandAvailable && resstation1[i].a == 3))// check for operandavailable flag in Qj & Qk and QJ and a value for load and stores
				{
					if (inst1[resstation1[i].instNum].executeClockBegin == 0)// check if the executeclockbegin is having default value. Use instNum variable find the instruction number
					{
						inst1[resstation1[i].instNum].executeClockBegin = Clock;// if it is issue the current clock value as execution start cycle
					}
					resstation1[i].lat++;// Increment the latency to match the latency of FMUL,FADD,FSUB etc. If latency matches means that we can perform the execution
					int temp_operation = resstation1[i].op; // store operation type
					if (temp_operation == 0)// means FADD Operation
					{
						if (resstation1[i].lat == FPADD)
						{
							//perform addition
							resstation1[i].result = resstation1[i].Vj + resstation1[i].Vk;
							//we can now complete the execution and update the resultReady flag
							resstation1[i].resultReady = true;
							//reset the latency property
							resstation1[i].lat = 0;
							resstation1[i].ISSUE_Lat = 0;
							//set the Executeclock end
							inst1[resstation1[i].instNum].executeClockEnd = Clock;
						}


					}
					if (temp_operation == 1)// means FSUB Operation
					{
						if (resstation1[i].lat == FPADD)
						{
							//perform subtraction
							resstation1[i].result = resstation1[i].Vj - resstation1[i].Vk;
							//we can now complete the execution and update the resultReady flag
							resstation1[i].resultReady = true;
							//reset the latency property
							resstation1[i].lat = 0;
							resstation1[i].ISSUE_Lat = 0;
							//set the Executeclock end
							inst1[resstation1[i].instNum].executeClockEnd = Clock;
						}


					}
					if (temp_operation == 2)// means FMUL Operation
					{
						if (resstation1[i].lat == FPMUL)
						{
							//perform multiplication
							resstation1[i].result = resstation1[i].Vj * resstation1[i].Vk;
							//we can now complete the execution and update the resultReady flag
							resstation1[i].resultReady = true;
							//reset the latency property
							resstation1[i].lat = 0;
							resstation1[i].ISSUE_Lat = 0;
							//set the Executeclock end
							inst1[resstation1[i].instNum].executeClockEnd = Clock;
						}


					}
					if (temp_operation == 3)// means FDIV Operation
					{
						if (resstation1[i].lat == FPDIV)
						{
							//perfrom division
							resstation1[i].result = resstation1[i].Vj / resstation1[i].Vk;
							//we can now complete the execution and update the resultReady flag
							resstation1[i].resultReady = true;
							//reset the latency property
							resstation1[i].lat = 0;
							resstation1[i].ISSUE_Lat = 0;
							//set the Executeclock end
							inst1[resstation1[i].instNum].executeClockEnd = Clock;
						}


					}

					if (temp_operation == 4)// means fld
					{
						if (resstation1[i].lat == FPALU)
						{

							resstation1[i].result = resstation1[i].Vj;
							//we can now complete the execution and update the resultReady flag
							resstation1[i].resultReady = true;
							//reset the latency property
							resstation1[i].lat = 0;
							resstation1[i].ISSUE_Lat = 0;
							//set the Executeclock end
							inst1[resstation1[i].instNum].executeClockEnd = Clock;

						}


					}

					if (temp_operation == 5)// means fsd
					{
						if (resstation1[i].lat == FPALU)
						{
							resstation1[i].result = resstation1[i].Vj;
							//we can now complete the execution and update the resultReady flag
							resstation1[i].resultReady = true;
							//reset the latency property
							resstation1[i].lat = 0;
							resstation1[i].ISSUE_Lat = 0;
							//set the Executeclock end
							inst1[resstation1[i].instNum].executeClockEnd = Clock;

						}


					}
					if (temp_operation == 6 || temp_operation == 8)// means ADD,ADDI Operation
					{
						if (resstation1[i].lat - 1 == INT) // by default lat will start in 1. Since value for Inetger operations is zero we do -1 to avoid infinite loop scenario.
						{
							//perform addition
							if (temp_operation == 6)
							{
								resstation1[i].result = resstation1[i].Vj + resstation1[i].Vk;
							}
							//perform addition increment
							if (temp_operation == 8)
							{
								resstation1[i].Vj = resstation1[i].Vj + resstation1[i].Vk;
								resstation1[i].result = resstation1[i].Vj;
							}

							//we can now complete the execution and update the resultReady flag
							resstation1[i].resultReady = true;
							//reset the latency property
							resstation1[i].lat = 0;
							resstation1[i].ISSUE_Lat = 0;
							//set the Executeclock end
							inst1[resstation1[i].instNum].executeClockEnd = Clock;
						}


					}
					if (temp_operation == 7 || temp_operation == 9)// means SUB,SUBI Operation
					{
						if (resstation1[i].lat - 1 == INT) // by default lat will start in 1. Since value for Inetger operations is zero we do -1 to avoid infinite loop scenario.
						{
							//perform subtraction
							if (temp_operation == 7)
							{
								resstation1[i].result = resstation1[i].Vj - resstation1[i].Vk;
							}
							//perform decrement
							if (temp_operation == 9)
							{
								resstation1[i].Vj = resstation1[i].Vj - resstation1[i].Vk;
								resstation1[i].result = resstation1[i].Vj;
							}
							std::cout<<"Calculation for subtraction "<< resstation1[i].Vj << " - " <<resstation1[i].Vk <<"= " <<resstation1[i].result;
							//we can now complete the execution and update the resultReady flag
							resstation1[i].resultReady = true;
							//reset the latency property
							resstation1[i].lat = 0;
							resstation1[i].ISSUE_Lat = 0;
							//set the Executeclock end
							inst1[resstation1[i].instNum].executeClockEnd = Clock;
						}


					}

					if (temp_operation == 10)// means LD
					{
						if (resstation1[i].lat == LDINT)
						{

							resstation1[i].result = resstation1[i].Vj;
							//we can now complete the execution and update the resultReady flag
							resstation1[i].resultReady = true;
							//reset the latency property
							resstation1[i].lat = 0;
							resstation1[i].ISSUE_Lat = 0;
							//set the Executeclock end
							inst1[resstation1[i].instNum].executeClockEnd = Clock;

						}


					}
					if (temp_operation == 11)// means SD
					{
						if (resstation1[i].lat - 1 == INT)
						{

							resstation1[i].result = resstation1[i].Vj;
							//we can now complete the execution and update the resultReady flag
							resstation1[i].resultReady = true;
							//reset the latency property
							resstation1[i].lat = 0;
							resstation1[i].ISSUE_Lat = 0;
							//set the Executeclock end
							inst1[resstation1[i].instNum].executeClockEnd = Clock;

						}


					}

					if (temp_operation == 50)// means BNEZ
					{
						std::cout<<"issuing a BNEZ"<<endl;
						if (resstation1[i].lat - 1 == INT)
						{
							
							//add next iteration instructions to instruction set using loopstartline and loopendline
							std::cout << "reservationstation. vj & vk bnez =====" << resstation1[i].Vj<<"  "<< resstation1[i].Vk<<endl;
							resstation1[i].result = resstation1[i].Vj;
							std::cout << "lst == " << lst << "led == " << led<<endl;
							if (resstation1[i].Vj != 0)
							{
								for (int k = lst; k <= led; k++)
								{

									Instruction I(inst1[k].rd, inst1[k].rs, inst1[k].rt, inst1[k].opcode);//(rd,rs,rt,operation)
									std::cout << "instruction" << STRING_INST1[k];
									//std::cout << "inst1.begin()" << inst1.begin();
									std::cout << "error before";
									inst1.insert(inst1.begin()+led+1+ipos,I);
									std::cout << "error after";
									STRING_INST1.insert(STRING_INST1.begin() +led+1+ipos,STRING_INST1[k]);
									ipos++;//incremented to add loop instruction in the proper order
								}

							}
							if (resstation1[i].Vj == 0)//during final iteration add the instructions outside loop body.
							{
								//add instructions after loop
								for (int m = 0; m < INST_AFTERLOOP.size(); m++)
								{
									inst1.push_back(INST_AFTERLOOP[m]);
								}
							}

							// set nextinstuctionafter branch to true. used to stall the next instruction of branch till branch instruction is executed.
							nextinstructionafterbranch = true;
							//we can now complete the execution and update the resultReady flag
							resstation1[i].resultReady = true;
							//reset the latency property
							resstation1[i].lat = 0;
							resstation1[i].ISSUE_Lat = 0;
							//set the Executeclock end
							inst1[resstation1[i].instNum].executeClockEnd = Clock;
							std::cout << "Current Instruction" << currentInst_ISSUE;
							Clock = Clock - 2;//temporary fix for issue iteration of branch instruction clock cycle
							loopclock = false; // set the loop clock to display the correct clock again
							

						}


					}
					
					if (temp_operation == 51)// means BEZ
					{
						std::cout<<"issuing a BEZ"<<endl;
						if (resstation1[i].lat - 1 == INT)
						{
							
							//add next iteration instructions to instruction set using loopstartline and loopendline
							std::cout << "reservationstation. vj & vk bez =====" << resstation1[i].Vj<<"  "<< resstation1[i].Vk<<endl;
							resstation1[i].result = resstation1[i].Vj;
							std::cout << "lst == " << lst << "led == " << led<<endl;
							if (resstation1[i].Vj == 0) //****************THIS LINE IS THE CHANGE FROM BNEZ
							{
								for (int k = lst; k <= led; k++)
								{

									Instruction I(inst1[k].rd, inst1[k].rs, inst1[k].rt, inst1[k].opcode);//(rd,rs,rt,operation)
									std::cout << "instruction" << STRING_INST1[k];
									//std::cout << "inst1.begin()" << inst1.begin();
									std::cout << "error before";
									inst1.insert(inst1.begin()+led+1+ipos,I);
									std::cout << "error after";
									STRING_INST1.insert(STRING_INST1.begin() +led+1+ipos,STRING_INST1[k]);
									ipos++;//incremented to add loop instruction in the proper order
								}

							}
							if (resstation1[i].Vj != 0)//during final iteration add the instructions outside loop body. ******THIS IS THE LINE CHNAGE FROM BNEZ
							{
								//add instructions after loop
								for (int m = 0; m < INST_AFTERLOOP.size(); m++)
								{
									inst1.push_back(INST_AFTERLOOP[m]);
								}
							}

							// set nextinstuctionafter branch to true. used to stall the next instruction of branch till branch instruction is executed.
							nextinstructionafterbranch = true;
							//we can now complete the execution and update the resultReady flag
							resstation1[i].resultReady = true;
							//reset the latency property
							resstation1[i].lat = 0;
							resstation1[i].ISSUE_Lat = 0;
							//set the Executeclock end
							inst1[resstation1[i].instNum].executeClockEnd = Clock;
							std::cout << "Current Instruction" << currentInst_ISSUE;
							Clock = Clock - 2;//temporary fix for issue iteration of branch instruction clock cycle
							loopclock = false; // set the loop clock to display the correct clock again
							

						}

					}
				}
			}
			else
			{
				resstation1[i].ISSUE_Lat++;
				//we wait for 1 clock cycle after issuing instruction
			}
		}
	}

}


void writeback(vector<Instruction>& inst1, vector<reservationStation>& resstation1, vector<registerStatus>& regstatus1, vector<int>& reg1)
{
	for (int i = 0; i < resstation1.size(); i++)//loop through reservation stations
	{
		if (resstation1[i].resultReady) // check if result ready flag is true. If it is true that means execution cycle is completed
		{
			if (resstation1[i].WRITEBACK_Lat == WRITEBACK_Lat)//check if writeback latency is equal to 1 else wait for 1 cc
			{
				if (inst1[resstation1[i].instNum].writebackClock == 0)// check if writeback clock is in default value.
				{
					inst1[resstation1[i].instNum].writebackClock = Clock;// set the writeback clock value to the current clock
					if (resstation1[i].a == 3)
					{
						inst1[resstation1[i].instNum].writebackClock = 999;//temporary fix for branch instruction clock cycle.
					}

				}

				// Check if any registers (via the registerStatus) are waiting for current i result
				for (int k = 0; k < reg1.size(); k++) {
					// if RegisterStatus points to the givenreservation station r set that register[x] equal to executed result
					if (regstatus1[k].Qi == i) {
						// Write back to Registers
						reg1[k] = resstation1[i].result;
						regstatus1[k].Qi = regEmpty;
					}
				}
				// Now that the writeb back clock value is set we have to check for reservation stations waiting for the result of the register from the current reservation station.

				for (int j = 0; j < resstation1.size(); j++) //loop through reservation station and check if values Qj and Qk points to numbers of reservation station i.e(1,2,3,etc)
				{
					if (resstation1[j].Qj == i)//if val of Qj == current reservation station number
					{
						resstation1[j].Vj = resstation1[i].result;
						resstation1[j].Qj = OperandAvailable;
					}
					if (resstation1[j].Qk == i)//if val of Qk == current reservation station number
					{
						resstation1[j].Vk = resstation1[i].result;
						resstation1[j].Qk = OperandAvailable;
					}
				}



				//reset the values of reservation station to make it available for upcoming instructions
				resstation1[i].busy = false;
				resstation1[i].resultReady = false;
				resstation1[i].result = 0;
				resstation1[i].Vj = 0;
				resstation1[i].Vk = 0;
				resstation1[i].Qj = OperandInit;
				resstation1[i].Qk = OperandInit;
				resstation1[i].a = 999;
				resstation1[i].WRITEBACK_Lat = 0;
				resstation1[i].ISSUE_Lat = 0;

				//After the writeback Increment the Writeback counter
				Total_WRITEBACKS++;



			}
			else
			{
				resstation1[i].WRITEBACK_Lat++;
				//we wait for 1 clock cycle after executing instruction for writeback
			}
		}
	}

}


void printclockcycletable(vector<Instruction> INST, vector<string> STRING_INST) {
	char separator = ' ';
	const int width = 20;
	char lineSeperator = '-';
	const int lineWidth = 35;

	// Define column labels
	std::cout << left << setw(width) << setfill(separator) << "Instruction";
	std::cout << left << setw(width) << setfill(separator) << "Issue";
	std::cout << left << setw(width) << setfill(separator) << "Execute";
	std::cout << left << setw(width) << setfill(separator) << "WB";
	std::cout << left << setw(width) << setfill(separator) << "SystemClock" << endl;
	std::cout << right << setw(width * 5) << setfill(separator) << Clock;
	std::cout << endl;
	std::cout << left << setw(lineWidth) << setfill(lineSeperator);
	std::cout << endl;
	// Define Row Labels and values
	for (int i = 0; i < INST.size(); i++) {

    std::cout << left << setw(width) << setfill(separator) << STRING_INST[i];
		std::cout << left << setw(width) << setfill(separator) << INST[i].issueClock;
		std::cout << INST[i].executeClockBegin << "-";
		std::cout << left << setw(width) << setfill(separator) << INST[i].executeClockEnd;
		std::cout << left << setw(width) << setfill(separator) << INST[i].writebackClock;
		std::cout << endl;

	}

}


void printRegisters(vector<int> RegistersVector) {
	std::cout << "Register Content:" << endl;
	for (int i = 0; i < RegistersVector.size(); i++)
		std::cout << RegistersVector[i] << ' ';
	std::cout << endl;
}


int main()
{
	int i, j;


	cout << "File path for Latencies file: ";
	//string projectLatencies = "Latencies.txt"; //shortcut for not entering input manually
	string projectLatencies;
	cin >> projectLatencies;
	cout << "File path is" << projectLatencies << endl;

	vector<latencies> l; //vector named l for latencies struct
	vector<Instruction> inst;//Vector for Instruction class
	vector<string> string_inst;

	ifstream latFile(projectLatencies.c_str());

	i = 0;
	while (!latFile.eof()) {
		l.push_back(latencies()); //pushing a latencies struct object into the vector every time
		latFile >> l[i].producer >> l[i].consumer >> l[i].latency; //then adding the values from the latencies file into that particular struct object
		i++;
	}
	latFile.close();

	//store latencies into individual variables

	int FPMUL, FPDIV, FPADD, FPLD, FPALU, LDINT, INT;

	for (int i = 0; i < l.size(); i++) {
		stringstream ss;
		if (l[i].producer == "FPMUL") {
			ss << l[i].latency;
			ss >> FPMUL;
		}
		else if (l[i].producer == "FPDIV") {
			ss << l[i].latency;
			ss >> FPDIV;
		}
		else if (l[i].producer == "FPADD") {
			ss << l[i].latency;
			ss >> FPADD;
		}
		else if (l[i].producer == "FPLD") {
			ss << l[i].latency;
			ss >> FPLD;
		}
		else if (l[i].producer == "FPALU") {
			ss << l[i].latency;
			ss >> FPALU;
		}
		else if (l[i].producer == "LDINT") {
			ss << l[i].latency;
			ss >> LDINT;
		}
		else if (l[i].producer == "INT") {
			ss << l[i].latency;
			ss >> INT;
		}
	}

	//display all to verify correctness

	std::cout << FPMUL << endl;
	std::cout << FPDIV << endl;
	std::cout << FPADD << endl;
	std::cout << FPLD << endl;
	std::cout << FPALU << endl;
	std::cout << LDINT << endl;
	std::cout << INT << endl;
	ifstream isFile("instruction.txt");


	map<string, int> operation = {
		 {"FADD.D", 0},
		 {"FSUB.D", 1},
		 {"FMUL.D", 2},
		 {"FDIV.D", 3},
		 {"FLD", 4},
		 {"FSD", 5},
		 {"LD", 10},
		 {"SD",11},
		 {"F0", 0},
		 {"F1", 1},
		 {"F2", 2},
		 {"F3", 3},
		 {"F4", 4},
		 {"F5", 5},
		 {"F6", 6},
		 {"F7", 7},
		 {"F7", 7},
		 {"F8", 8},
		 {"F9", 9},
		 {"F10", 10},
		 {"F11", 11},
		 {"F12", 12},
		 {"X0", 13},
		 {"X1", 14},
		 {"X2", 15},
		 {"X3", 16},
		 {"X4", 17},
		 {"X5", 18},
		 {"X6", 19},
		 {"X7", 20},
		 {"ADD", 6},
		 {"SUB", 7},
		 {"ADDI", 8},
		 {"SUBI", 9},
		 {"1",1}, // rt encoding for ADDI,SUBI
		 {"2",2}, // rt encoding for ADDI,SUBI
		 {"BNEZ",50},
	     {"BEZ",51}

	};

	//Initialize reservation station Class

	reservationStation
		ADD1(0, OperandInit, 999),
		ADD2(0, OperandInit, 999),
		ADD3(0, OperandInit, 999),
		ADD4(0, OperandInit, 999);
	reservationStation
		MULT1(2, OperandInit, 999),
		MULT2(2, OperandInit, 999),
		MULT3(2, OperandInit, 999),
		MULT4(2, OperandInit, 999);
	reservationStation
		DIV1(3, OperandInit, 999),
		DIV2(3, OperandInit, 999),
		DIV3(3, OperandInit, 999),
		DIV4(3, OperandInit, 999);
	reservationStation
		LDSD1(4, OperandInit, 999),
		LDSD2(4, OperandInit, 999),
		LDSD3(4, OperandInit, 999),
		LDSD4(4, OperandInit, 999);
	reservationStation 
		BRANCH1(51, OperandInit, 999),
		BRANCH2(51, OperandInit, 999);

	vector<reservationStation> resStation = { ADD1,ADD2,ADD3,ADD4,MULT1,MULT2,MULT3,MULT4,DIV1,DIV2,DIV3,DIV4,LDSD1,LDSD2,LDSD3,LDSD4,BRANCH1,BRANCH2};

	// Initialize Register Status Class
	registerStatus
		F0(regEmpty), F1(regEmpty), F2(regEmpty), F3(regEmpty), F4(regEmpty), F5(regEmpty),
		F6(regEmpty), F7(regEmpty), F8(regEmpty), F9(regEmpty), F10(regEmpty), F11(regEmpty), F12(regEmpty), X0(regEmpty), X1(regEmpty), X2(regEmpty), X3(regEmpty), X4(regEmpty), X5(regEmpty), X6(regEmpty), X7(regEmpty);
	vector<registerStatus> registerStatus = { F0, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,X0, X1, X2, X3, X4, X5, X6, X7 };

	// Initialize register file vector
	vector<int> registers = { ZERO_REG,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,3,17,18,19,20 };
	int looplinestart = 999;
	int looplinened = 999;
	// Read Instruction File line by line, Seperate line to words and Initialize Instruction Object of Class Instruction
	if (isFile.is_open())
	{
		string line = "";
		vector<int> temp_i;
		int cnt = -1;
		while (getline(isFile, line))
		{
			std::cout << line << endl;

			cnt++;
			string word;
			string_inst.push_back(line);
			string str1 = "loop:";
			size_t found = line.find(str1);
			if (found != string::npos)
			{
				looplinestart = cnt;
				std::cout << "First occurrence is " << found << endl;
				//clip the loop part from the string
				string tline = line.substr(found+6);
				std::cout << "tline===" << tline<<endl;
				line = "";
				line = tline;
				std::cout << "line===" << line<<endl;
				std::cout << "looplinestart===" << looplinestart << endl;
				operation.insert({ "loop",looplinestart }); // loop line number is added. The value is used to start the next iteration from this line.
			}

			stringstream sl(line);

			//find loop end
			string str2 = "loop";
			found = line.find(str2);
			if (found != string::npos)
			{
				looplinened = cnt;
				std::cout << "looplineend===" << looplinened << endl;
				operation.insert({ "loopend",looplinened});// loop's last instruction number.
			}
			while (sl >> word)
			{
				stringstream sw(word);

				int i = -1;
				while (sw.good())
				{
					string sub;
					getline(sw, sub, ',');
					i++;
					auto it = operation.find(sub);
					if (it == operation.end())
					{
						// not found  
						std::cout << "Element not foundr" << endl;

						// Handle Effective address computation Case like ld/sd
						int pos1 = sub.find("(");
						int pos2 = sub.find(")");

						string reg_x = sub.substr(pos1 + 1, pos2 - pos1 - 1);
						auto itx = operation.find(reg_x);
						//std::cout << " Postions of ()" << endl;
						//std::cout << pos1 << "  " << pos2 - 1 << "  " << reg_x;
						//std::cout << "itx->second" << "  " << itx->second;
						temp_i.push_back(itx->second);

					}

					else
					{
						temp_i.push_back(it->second);
					}

				}


			}



			//std::cout << "Temp_i size" << temp_i.size()<< endl;

			if (temp_i.size() == 4)
			{
				auto rd1 = temp_i.at(1);
				auto rs1 = temp_i.at(2);
				auto rt1 = temp_i.at(3);
				auto o1 = temp_i.at(0);
				Instruction I(rd1, rs1, rt1, o1);//(rd,rs,rt,operation)
				inst.push_back(I);
			}
			if (temp_i.size() == 3)
			{
				auto rd1 = temp_i.at(1);
				auto rs1 = temp_i.at(2);
				auto o1 = temp_i.at(0);
				Instruction I(rd1, rs1,999, o1);//(rd,rs,rt,operation)
				inst.push_back(I);
			}


			//std::cout << temp_i.at(1) << temp_i.at(2) << temp_i.at(3) << temp_i.at(0);

			//std::cout << "rd1" << I.rd;

			while (!temp_i.empty())
			{
				temp_i.pop_back();
			}

		}
		std::cout << inst.size() << "   ";
		// Print to check Inst vector is properly initialized

/*for (auto it1 : inst)
		{
			std::cout << it1.opcode << " ";
		}*/

	}

	//unroll loop to avoid complexity
	vector<Instruction> inst_afterloop;
	for (int k = 0; k < inst.size(); k++)
	{
		//BNEZ version
		if (inst[k].opcode == 50)
		{
			std::cout << "Inside bnez"<<k<<endl;
			//add instructions after loop
			for (int m = k+1; m < inst.size(); m++)
			{
				inst_afterloop.push_back(inst[m]);
			}
			//delete the instructions after loop
			for (int m = k + 1; m < inst.size(); m++)
			{
				inst.erase(inst.begin()+m);
			}
			break;//break to avoid redundant iteration
		}
		//BEZ version
		if (inst[k].opcode == 51)
		{
			std::cout << "Inside bez"<<k<<endl;
			//add instructions after loop
			for (int m = k+1; m < inst.size(); m++)
			{
				inst_afterloop.push_back(inst[m]);
			}
			//delete the instructions after loop
			for (int m = k + 1; m < inst.size(); m++)
			{
				inst.erase(inst.begin()+m);
			}
			break;//break to avoid redundant iteration
		}
	}
	std::cout << "Instruction Size"<<inst.size()<<endl;
	std::cout << "Inst_afterloop size" << inst_afterloop.size()<<endl;
	// Main Loop 
	do {
		// Datapath
		Clock++; // system clock

		issue(inst, resStation, registerStatus, registers,operation);
		execute(inst, resStation, registerStatus, registers, FPMUL, FPDIV, FPADD, FPLD, FPALU, LDINT, INT,looplinestart,looplinened,string_inst,inst_afterloop);
		writeback(inst, resStation, registerStatus, registers);

		//print cc table
		//printRegisters(registers);
		if(!loopclock)
			printclockcycletable(inst,string_inst);
		//std::cout << "inst" << inst.size();
		std::cout << "Total Writebacks" << Total_WRITEBACKS;
		//std::cout << "Total Writebacks" << Total_WRITEBACKS;

		// Check if all reservation stations are empty -> program done
	
		Done = false;
		if (Total_WRITEBACKS == inst.size())
			Done = true;

std::cout << endl;
	} while (!Done);
	

	return 0;
}


//TODO:-
//Branching code is implemented but need to pipeline the execute and writeback stage properly.
// check execute and writeback clock to support pipelining.ADD BEZ Instruction logic
