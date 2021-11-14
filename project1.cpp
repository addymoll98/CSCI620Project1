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


int issue(vector<Instruction>& inst1, vector<reservationStation>& resstation1, vector<registerStatus>& regstatus1, vector<int>& reg1) {
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


void execute(vector<Instruction>& inst1, vector<reservationStation>& resstation1, vector<registerStatus>& regstatus1, vector<int>& reg1, int FPMUL, int FPDIV, int FPADD, int FPLD, int FPALU, int LDINT, int INT)
{

	for (int i = 0; i < resstation1.size(); i++)
	{
		if (resstation1[i].busy == true)//check if any reservation station is busy
		{
			if (resstation1[i].ISSUE_Lat >= ISSUE_Lat)//check if issue latency is equal to 1 else wait for 1 cc
			{

				if ((resstation1[i].Qj == OperandAvailable && resstation1[i].Qk == OperandAvailable) || (resstation1[i].Qj == OperandAvailable && resstation1[i].a == 0) || (resstation1[i].Qj == OperandAvailable && resstation1[i].a == 1))// check for operandavailable flag in Qj & Qk and QJ and a value for load and stores
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

					if (temp_operation == 4)// means ld/fld
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

					if (temp_operation == 5)// means sd/fsd
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
				resstation1[i].resultReady = false;
				resstation1[i].result = 0;
				resstation1[i].Vj = 0;
				resstation1[i].Vk = 0;
				resstation1[i].Qj = OperandInit;
				resstation1[i].Qk = OperandInit;

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


void printclockcycletable(vector<Instruction> INST) {
	char separator = ' ';
	const int width = 13;
	char lineSeperator = '-';
	const int lineWidth = 35;

	// Define column labels
	cout << left << setw(width) << setfill(separator) << "Instruction";
	cout << left << setw(width) << setfill(separator) << "Issue";
	cout << left << setw(width) << setfill(separator) << "Execute";
	cout << left << setw(width) << setfill(separator) << "WB";
	cout << left << setw(width) << setfill(separator) << "SystemClock" << endl;
	cout << right << setw(width * 5) << setfill(separator) << Clock;
	cout << endl;
	cout << left << setw(lineWidth) << setfill(lineSeperator);
	cout << endl;
	// Define Row Labels and values
	for (int i = 0; i < INST.size(); i++) {
		cout << left << setw(width) << setfill(separator) << i;
		cout << left << setw(width) << setfill(separator) << INST[i].issueClock;
		cout << INST[i].executeClockBegin << "-";
		cout << left << setw(width) << setfill(separator) << INST[i].executeClockEnd;
		cout << left << setw(width) << setfill(separator) << INST[i].writebackClock;
		cout << endl;
	}

}



int main()
{
	int i, j;
	cout << "File path for Latencies file: ";
	string projectLatencies = "Latencies.txt";
	cout << "File path is" << projectLatencies << endl;

	vector<latencies> l; //vector named l for latencies struct
	vector<Instruction> inst;//Vector for Instruction class

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

	cout << FPMUL << endl;
	cout << FPDIV << endl;
	cout << FPADD << endl;
	cout << FPLD << endl;
	cout << FPALU << endl;
	cout << LDINT << endl;
	cout << INT << endl;
	ifstream isFile("instruction.txt");

	map<string, int> operation = {
		 {"FADD.D", 0},
		 {"FSUB.D", 1},
		 {"FMUL.D", 2},
		 {"FDIV.D", 3},
		 {"FLD", 4},
		 {"FSD", 5},
		 {"F0", 0},
		 {"F1", 1},
		 {"F2", 2},
		 {"F3", 3},
		 {"F4", 4},
		 {"F5", 5},
		 {"F6", 6},
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
		 {"X5", 18}

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

	vector<reservationStation> resStation = { ADD1,ADD2,ADD3,ADD4,MULT1,MULT2,MULT3,MULT4,DIV1,DIV2,DIV3,DIV4,LDSD1,LDSD2,LDSD3,LDSD4 };

	// Initialize Register Status Class
	registerStatus
		F0(regEmpty), F1(regEmpty), F2(regEmpty), F3(regEmpty), F4(regEmpty), F5(regEmpty),
		F6(regEmpty), F7(regEmpty), F8(regEmpty), F9(regEmpty), F10(regEmpty), F11(regEmpty), F12(regEmpty), X0(regEmpty), X1(regEmpty), X2(regEmpty), X3(regEmpty), X4(regEmpty), X5(regEmpty);
	vector<registerStatus> registerStatus = { F0, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,X0, X1, X2, X3, X4, X5 };

	// Initialize register file vector
	vector<int> registers = { ZERO_REG,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18 };

	// Read Instruction File line by line, Seperate line to words and Initialize Instruction Object of Class Instruction
	if (isFile.is_open())
	{
		string line = "";
		vector<int> temp_i;
		while (getline(isFile, line))
		{
			std::cout << line << endl;

			stringstream sl(line);
			string word;

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
						cout << "Element not foundr" << endl;

						// Handle Effective address computation Case like ld/sd
						int pos1 = sub.find("(");
						int pos2 = sub.find(")");

						string reg_x = sub.substr(pos1 + 1, pos2 - pos1 - 1);
						auto itx = operation.find(reg_x);
						//cout << " Postions of ()" << endl;
						//cout << pos1 << "  " << pos2 - 1 << "  " << reg_x;
						//cout << "itx->second" << "  " << itx->second;
						temp_i.push_back(itx->second);

					}

					else
					{
						temp_i.push_back(it->second);
					}

				}


			}



			//cout << "Temp_i size" << temp_i.size()<< endl;

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
				Instruction I(rd1, rs1, 17, o1);//(rd,rs,rt,operation)
				inst.push_back(I);
			}

			//cout << temp_i.at(1) << temp_i.at(2) << temp_i.at(3) << temp_i.at(0);

			//cout << "rd1" << I.rd;

			while (!temp_i.empty())
			{
				temp_i.pop_back();
			}

		}
		cout << inst.size() << "   ";
		// Print to check Inst vector is properly initialized
		/*for (auto it1 : inst)
		{
			cout << it1.opcode << " ";
		}*/
	}

	//cout << "Instruction Size"<<inst.size();
	// Main Loop 
	do {
		// Datapath
		Clock++; // system clock

		issue(inst, resStation, registerStatus, registers);
		execute(inst, resStation, registerStatus, registers, FPMUL, FPDIV, FPADD, FPLD, FPALU, LDINT, INT);
		writeback(inst, resStation, registerStatus, registers);

		//print cc table
		printclockcycletable(inst);
		//cout << "inst" << inst.size();
		//cout << "Total Writebacks" << Total_WRITEBACKS;
		//cout << "Total Writebacks" << Total_WRITEBACKS;

		// Check if all reservation stations are empty -> program done
	
		Done = false;
		if (Total_WRITEBACKS == inst.size())
			Done = true;
		cout << endl;
	} while (!Done);

	return 0;
}


//TODO:-
// Clock cycles for load and store Instruction are working properly. Need to add support for ADDI,SUBI,ADD,SUB. Also should add code to handle branching instructions
