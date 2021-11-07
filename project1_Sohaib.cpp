#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

//Get input into struct - look into ifstream
//Create vector of struct for latencies and input so it can dynamically allocate size 
//Make the program take Unix path for inputs -- getline as cin
//Run and output to confirm

struct latencies  //declaring struct outside to use vectors - g++ compiler errors
    {
        string producer;
        string consumer;
        string latency;     //storing it as a string for now, convert to int using stringstream
    };

int main(){

    vector<latencies> l; //vector named l for latencies struct

    ifstream latFile("620_proj_latencies");

    int i = 0;

    while(!latFile.eof()) {

        l.push_back(latencies()); //pushing a latencies struct object into the vector every time
        latFile >> l[i].producer >> l[i].consumer >> l[i].latency; //then adding the values from the latencies file into that particular struct object
        i++;
        
    }
    latFile.close();

    for(int i = 0; i < l.size(); i++) {
        cout << left << setw(15) << l[i].producer;
        cout << setw(15) << l[i].consumer;
        cout << setw(15) << l[i].latency << endl;
    }

    cout << l[2].latency << endl; // example on how to access an element from the struct vector
                                  // here accessing the 3rd latency element 

    cout << endl;
    cout << endl;


    fstream myFile; //generic fstream file to read in inputs

    myFile.open("620_proj_isyntax", ios:: in); //read input file
    if(myFile.is_open()) {
        string line;
        while(getline(myFile, line)) {
            cout << line << endl;
        }

        myFile.close();
    }
    
    cout << endl;

    return 0;
}


