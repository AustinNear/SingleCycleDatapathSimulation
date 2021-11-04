**CSE 140 Project Report**  

**Team ID : Austin Near  
Date: 5/5/2021  
Single-cycle MIPS CPU   
1.1	Overview**  
It has global variables and functions for each stage of a pipelined CPU datapath, but mine only has single cycled implementation. 
**1.2	Code Structure**  
// Explain the detailed code structure that shows the functions and variables with how you implemented and how those are interacting with the other functions or variables.
**1.2.1	Functions**  
**Fetch()**: Gets lines from the .txt file one at a time and puts them into the instruction variable. It updates the PC and PC+4 values and then calls the decode function. It uses the flag and offset variable to keep its place in the file.

**Decode()**: Goes through the instruction (machine code), it checks the instruction type first and then it will check the values of the sections of the instruction corresponding to each field of that type. It sets global variables that are ints and strings for the number of each register and the name of each register because that is useful in further stages. It also stores immediates and jump addresses for future use. It uses far too many switch statements to be neat but it ended up being a simple solution. It then calls the controlunit() function at the end.
 
**ControlUnit()**: It changes the values of the control signal variables depending on the instruction. It also has the ALU Control Unit inside it that sets a variable aluControlOut to the right 4 bit binary string that represents the ALU operation needed. Then it runs execute sending the aluControlOut value.


**Execute()**: Runs different operations based on the aluControlOut value, operations, and control signals. It does calculations between registers, offsets, and addresses. At the end of each operation it calls the writeback() function, mem() function, or finish there depending on the instruction and control signals.

**Mem()**: If it is called it checks the memread control signal and if it’s a 1 then it writes to the memory and then after it continues to the writeback function.

**Writeback()**: The final function called for all operations besides jump. It writes the values to the corresponding register indexes in the RF array. It also outputs what the changes are into the console and updates and prints the clock cycles.

**1.2.2	Variables**  
int PC = 0;
int nextPC = 4;
string filename;  
string instruction;  
string nextinstr;
streamoff offset; //offset to keep spot while reading .txt
int flag = 0; //flag for when the file has been read to help with PC+4
int RF[32];
string jumptarget;
string op;
string Rs; //name of the reg
string Rt;
string Rd;
int RsValue; //value in the reg
int RtValue;
int RdValue;
int RsReg; //number of the reg in list
int RtReg;
int RdReg;
string iOff; //instruction offset
int alu_zero;
string branchtarget;
int d_mem[32];
int total_clock_cycles;
int imm;
string memaddress;
char IT; //instruction tpye
//control signals
int jump;
int regwrite;
int regdst;
int branch;
int alusrc;
int aluop1;
int aluop2; 
int memwrite;
int memtoreg;
int memread;
string aluControlOut;

**1.3	Execution Results**  
![alt text](https://github.com/AustinNear/SingleCycleDatapathSimulation/blob/main/Picture1.png?raw=true)  
**1.4	Challenges and Limitations**  
I spent a long time trying to add the next instruction and the target address for branch instructions because the value was too big to be stored in an integer and I had to use unsigned long long but that was hard to work with. I eventually found the bitset library and was able to convert the binary string to ull, add them, and then convert back to a string in just two lines instead of whole helper functions like I was using before.

My program is only complete for single cycle and does not have pipelined implementation because I found it too difficult.

I also had an issue getting fetch to correctly get the current instruction and next one, but I managed to make it working using a flag and keeping track of where was being read in the file with a stream offset.

When I first completed the program it outputted the last cycle incorrectly and had an additional “c” being printed as its own line, and I couldn’t figure out why. After hours of trying I found that the “c” was from a debug cout statement that I accidentally left in and that the wrong information being displayed on the last cycle was just because of wrong outputs and the actual stored values were modified accordingly the whole time. I found this out by tracking the values all the way through the cycles.
