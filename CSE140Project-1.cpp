#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h> 
#include <bitset>
using namespace std;

int PC = 0;
int nextPC = 4;
string filename;
string instruction;
string nextinstr;
streamoff offset;
int flag = 0;
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
string iOff;
int alu_zero;
string branchtarget;
int d_mem[32];
int total_clock_cycles;
int imm;
string memaddress;
char IT;
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



void writeback(string address, int value){
    total_clock_cycles++;
    cout << "total_clock_cycles " << total_clock_cycles << " :" << endl;
    if(op == "sw"){    
        d_mem[stoi(address, nullptr, 2)] = value;
        cout << "memory " << hex << "0x" << stoi(address, nullptr, 2) << " is modified to 0x"<< hex << value << endl;
    }
    if(op == "lw"){
        RF[RtReg] = value;
        cout << Rt << " is modified to 0x" << hex << value << endl;
    }

    if(op == "add" || op == "sub"){
        RF[RdReg] = value;
        cout << Rd << " is modified to 0x" << hex << value << endl;
    }

    if(op == "and" || op == "or" || op == "slt" || op == "nor"){
        RF[RdReg] = RdValue;
        cout << Rd << " is modified to 0x" << hex << RdValue << endl;
    }
    if(op == "beq"){
        
    }
    cout << "pc is modified to 0x" << hex << PC << endl;
    cout << endl;

}


void mem(string address, int value){ 
    
    if(memread == 1){
        value = d_mem[stoi(address, nullptr, 2)/4];
    }
    writeback(address, value);
}


void execute(string alu_op){
    // cout << alu_op << endl;
    alu_zero = 0;
    branchtarget = "";
    if(alu_op == "0000"){ //AND
        RdValue = 0;
        if(RsValue&&RtValue)
            RdValue = 1;
    }
    if(alu_op == "0001"){ //OR
        RdValue = 0;
        if(RsValue||RtValue)
            RdValue = 1;
    }
    if(alu_op == "0010"){ //add
        if(op == "sw" || op == "lw"){
        //cout << "memread: " << memread << endl;
        //cout << "RtValue: " << RF[RtReg] << endl;      
        unsigned long long z = bitset<32>(iOff).to_ullong() + bitset<32>(RsValue).to_ullong();
        memaddress = bitset<32>(z).to_string();
        mem(memaddress, RtValue);
    }
    else
        RdValue = RsValue + RtValue;
    }
    if(alu_op == "0110"){ //subtract
        RdValue = RsValue - RtValue;
        if(RdValue == 0)
            alu_zero = 1;
        if(branch == 1){
            int temp = stoi(iOff, nullptr, 2);
            temp =  temp << 2;
            //cout << temp << endl;
            iOff = bitset<32>(temp).to_string();
    

            //  cout << "1: " << iOff << endl;
            //  cout << "2: " << nextinstr  << endl;

            unsigned long long x = bitset<32>(iOff).to_ullong() + bitset<32>(PC).to_ullong();
            branchtarget = bitset<32>(x).to_string();
            if(alu_zero == 1){
            PC = stoi(branchtarget, nullptr, 2);
    }
    }
        writeback(memaddress, RdValue);
    }
    if(alu_op == "0111"){ //slt
        RdValue = 0;
        if(RsValue < RtValue)
            RdValue = 1;
        writeback(memaddress, RdValue);
    }
    if(alu_op == "1100"){ //NOR
        RdValue = 0;
        if(!RsValue||!RtValue)
            RdValue = 1;
        writeback(memaddress, RdValue);
    }
    

    if(jump == 1){
        if(alu_zero == 1)
            PC = stoi(jumptarget, nullptr, 2);
    }

    //  cout << "target: " << memaddress << endl;

    
}


void controlunit(){
    if(IT == 'R'){
        jump = 0;
        regdst = 1;
        alusrc = 0;
        memtoreg = 0;
        regwrite = 1;
        memread = 0;
        memwrite = 0;
        branch = 0;
        aluop1 = 1;
        aluop2 = 0;
    }
    if(op == "lw"){
        jump = 0;
        regdst = 0;
        alusrc = 1;
        memtoreg = 1;
        regwrite = 1;
        memread = 1;
        memwrite = 0;
        branch = 0;
        aluop1 = 1;
        aluop2 = 0;
    }
    if(op == "sw"){
        jump = 0;
        regdst = 0;
        alusrc = 1;
        memtoreg = 0;
        regwrite = 0;
        memread = 0;
        memwrite = 1;
        branch = 0;
        aluop1 = 0;
        aluop2 = 0;
    }
    if(op == "beq"){
        jump = 0;
        regdst = 0;
        alusrc = 0;
        memtoreg = 0;
        regwrite = 1;
        memread = 0;
        memwrite = 0;
        branch = 1;
        aluop1 = 0;
        aluop2 = 1;
    }
    if(op == "j"){
        jump = 1;
        regdst = 0;
        alusrc = 0;
        memtoreg = 0;
        regwrite = 0;
        memread = 0;
        memwrite = 0;
        branch = 0;
        aluop1 = 0;
        aluop2 = 0;
    }
    //ALU Control Unit
    aluControlOut = "1100"; // nor
    if(op == "add" || op == "lw" || op == "sw" || op == "j")
        aluControlOut = "0010"; //add
    if(op == "beq" || op == "sub")
        aluControlOut = "0110"; //sub
    if(op == "or")
        aluControlOut = "0001"; //or
    if(op == "slt")
        aluControlOut = "0111"; //slt
    if(op == "and")
        aluControlOut = "0000"; //and


   
    // if(aluop1 == 0 && aluop2 == 0)
    //     aluControlOut = "0010"; //add
    // if(aluop2 == 1)
    //     aluControlOut = "0110"; //sub
    
    // if(aluop1 == 1){
    //     if(instruction[28] == 0 && instruction[29] == 0 && instruction[30] == 0 && instruction[31] == 0){
    //         aluControlOut = "0010"; //add
    //     }
    // }
    // if(aluop1 == 1){
    //     if(instruction[28] == 0 && instruction[29] == 0 && instruction[30] == 1 && instruction[31] == 0){
    //         aluControlOut = "0110"; //sub
    //     }
    // }
    // if(aluop1 == 1){
    //     if(instruction[28] == 0 && instruction[29] == 1 && instruction[30] == 0 && instruction[31] == 0){
    //         aluControlOut = "0000"; //and
    //     }
    // }
    // if(aluop1 == 1){
    //     if(instruction[28] == 0 && instruction[29] == 1 && instruction[30] == 0 && instruction[31] == 1){
    //         aluControlOut = "0001"; //or
    //     }
    // }
    // if(aluop1 == 1){
    //     if(instruction[28] == 1 && instruction[29] == 0 && instruction[30] == 1 && instruction[31] == 0){
    //         aluControlOut = "0111"; //slt
    //     }
    // }

    execute(aluControlOut);

}


void decode(){
    imm = 0;
    IT = 'x';
    int Shamt;
    int Funct;
    string code;
    int count = 0;
    int temp = 0;
    int temp2 = 0;
    int temp3 = 0;
    int temp4 = 0;
    int temp5 = 0;
    jumptarget = "";
    code = instruction;

    for(int i = 0; i < 6; i++){
        if(code[i] == '1')
            count++;
    }
    if(count == 0){
        IT = 'R';
    }
    else
        if((count < 3)&& code[4] == '1'){
            IT = 'J';
        }
    else
        IT = 'I';

    if(IT == 'R'){
        for(int i = 0; i < 5; i++){
            if(code[6+i] == '1'){
                temp += pow(2, 4-i);
            }
            if(code[11+i] == '1'){
                temp2 += pow(2, 4-i);
            }
            if(code[16+i] == '1'){
                temp3 += pow(2, 4-i);
            }
            if(code[21+i] == '1'){
                temp4 += pow(2, 4-i);
            }
        }
        for(int i = 0; i < 6; i ++){
            if(code[26+i] == '1'){
                temp5 += pow(2, 5-i);
            }
        }

    switch (temp) {
    case 0:
        Rs = "$zero";
        break;
    case 1:
        Rs = "$at";
        RsReg = 1;
        break;
    case 2:
        Rs = "$v0";
        break;
    case 3:
        Rs = "$v1";
        break;
    case 4:
        Rs = "$a0";
        break;
    case 5:
        Rs = "$a1";
        break;
    case 6:
        Rs = "$a2";
        break;
    case 7:
        Rs = "$a3";
        break;
    case 8:
        Rs = "$t0";
        break;
    case 9:
        Rs = "$t1";
        break;
    case 10:
        Rs = "$t2";
        break;
    case 11:
        Rs = "$t3";
        break;
    case 12:
        Rs = "$t4";
        break;
    case 13:
        Rs = "$t5";
        break;
    case 14:
        Rs = "$t6";
        break;
    case 15:
        Rs = "$t7";
        break;
    case 16:
        Rs = "$s0";
        break;
    case 17:
        Rs = "$s1";
        break;
    case 18:
        Rs = "$s2";
        break;
    case 19:
        Rs = "$s3";
        break;
    case 20:
        Rs = "$s4";
        break;
    case 21:
        Rs = "$s5";
        break;
    case 22:
        Rs = "$s6";
        break;
    case 23:
        Rs = "$s7";
        break;
    case 24:
        Rs = "$t8";
        break;
    case 25:
        Rs = "$t9";
        break;
    case 26:
        Rs = "$k0";
        break;
    case 27:
        Rs = "$k1";
        break;
    case 28:
        Rs = "$gp";
        break;
    case 29:
        Rs = "$sp";
        break;
    case 30:
        Rs = "$fp";
        break;
    case 31:
        Rs = "$ra";
        break;
    }

    switch (temp2) {
    case 0:
        Rt = "$zero";
        break;
    case 1:
        Rt = "$at";
        break;
    case 2:
        Rt = "$v0";
        break;
    case 3:
        Rt = "$v1";
        break;
    case 4:
        Rt = "$a0";
        break;
    case 5:
        Rt = "$a1";
        break;
    case 6:
        Rt = "$a2";
        break;
    case 7:
        Rt = "$a3";
        break;
    case 8:
        Rt = "$t0";
        break;
    case 9:
        Rt = "$t1";
        break;
    case 10:
        Rt = "$t2";
        break;
    case 11:
        Rt = "$t3";
        break;
    case 12:
        Rt = "$t4";
        break;
    case 13:
        Rt = "$t5";
        break;
    case 14:
        Rt = "$t6";
        break;
    case 15:
        Rt = "$t7";
        break;
    case 16:
        Rt = "$s0";
        break;
    case 17:
        Rt = "$s1";
        break;
    case 18:
        Rt = "$s2";
        break;
    case 19:
        Rt = "$s3";
        break;
    case 20:
        Rt = "$s4";
        break;
    case 21:
        Rt = "$s5";
        break;
    case 22:
        Rt = "$s6";
        break;
    case 23:
        Rt = "$s7";
        break;
    case 24:
        Rt = "$t8";
        break;
    case 25:
        Rt = "$t9";
        break;
    case 26:
        Rt = "$k0";
        break;
    case 27:
        Rt = "$k1";
        break;
    case 28:
        Rt = "$gp";
        break;
    case 29:
        Rt = "$sp";
        break;
    case 30:
        Rt = "$fp";
        break;
    case 31:
        Rt = "$ra";
        break;
    }

    switch (temp3) {
    case 0:
        Rd = "$zero";
        break;
    case 1:
        Rd = "$at";
        break;
    case 2:
        Rd = "$v0";
        break;
    case 3:
        Rd = "$v1";
        break;
    case 4:
        Rd = "$a0";
        break;
    case 5:
        Rd = "$a1";
        break;
    case 6:
        Rd = "$a2";
        break;
    case 7:
        Rd = "$a3";
        break;
    case 8:
        Rd = "$t0";
        break;
    case 9:
        Rd = "$t1";
        break;
    case 10:
        Rd = "$t2";
        break;
    case 11:
        Rd = "$t3";
        break;
    case 12:
        Rd = "$t4";
        break;
    case 13:
        Rd = "$t5";
        break;
    case 14:
        Rd = "$t6";
        break;
    case 15:
        Rd = "$t7";
        break;
    case 16:
        Rd = "$s0";
        break;
    case 17:
        Rd = "$s1";
        break;
    case 18:
        Rd = "$s2";
        break;
    case 19:
        Rd = "$s3";
        break;
    case 20:
        Rd = "$s4";
        break;
    case 21:
        Rd = "$s5";
        break;
    case 22:
        Rd = "$s6";
        break;
    case 23:
        Rd = "$s7";
        break;
    case 24:
        Rd = "$t8";
        break;
    case 25:
        Rd = "$t9";
        break;
    case 26:
        Rd = "$k0";
        break;
    case 27:
        Rd = "$k1";
        break;
    case 28:
        Rd = "$gp";
        break;
    case 29:
        Rd = "$sp";
        break;
    case 30:
        Rd = "$fp";
        break;
    case 31:
        Rd = "$ra";
        break;
    }

    switch (temp5) {
    case 32:
        op = "add";
        break;
    case 33:
        op = "addu";
        break;
    case 36:
        op = "and";
        break;
    case 8:
        op = "jr";
        break;
    case 39:
        op = "nor";
        break;
    case 37:
        op = "or";
        break;
    case 42:
        op = "slt";
        break;
    case 43:
        op = "sltu";
        break;
    case 0:
        op = "sll";
        break;
    case 2:
        op = "srl";
        break;
    case 34:
        op = "sub";
        break;
    case 35:
        op = "subu";
        break;
    }

    RsReg = temp;
    RtReg = temp2;
    RdReg = temp3;
    // cout << "Instruction Type: " << IT << endl;
    // cout << "Operation: " << op << endl;
    // cout << "Rs: " << Rs << " (R" << temp << ")" << endl;
    RsValue = RF[temp];
    // cout << "Rt: " << Rt << " (R" << temp2 << ")" << endl;
    RtValue = RF[temp2];
    // cout << "Rd: " << Rd << " (R" << temp3 << ")" <<  endl;
    RdValue = RF[temp3];
    // cout << "Shamt: " << temp4 << endl;
    // cout << "Funct: " << temp5 << endl;
    //cout << IT << " " << op << " " << Rs << " " << Rt << " " << Rd << " " << temp4 << " " << temp5 << endl;

    }

    if(IT == 'I'){
        for(int i = 0; i < 5; i++){
            if(code[6+i] == '1'){
                temp += pow(2, 4-i);
            }
            if(code[11+i] == '1'){
                temp2 += pow(2, 4-i);
            }
        }
        for(int i = 0; i < 6; i ++){
            if(code[0+i] == '1'){
                temp5 += pow(2, 5-i);
            }
        }
        
        for(int i = 0; i < 16; i ++){
            if(code[16+i] == '1'){
                temp3 += pow(2, 15-i);
            }
        }


    switch (temp) {
    case 0:
        Rs = "$zero";
        break;
    case 1:
        Rs = "$at";
        break;
    case 2:
        Rs = "$v0";
        break;
    case 3:
        Rs = "$v1";
        break;
    case 4:
        Rs = "$a0";
        break;
    case 5:
        Rs = "$a1";
        break;
    case 6:
        Rs = "$a2";
        break;
    case 7:
        Rs = "$a3";
        break;
    case 8:
        Rs = "$t0";
        break;
    case 9:
        Rs = "$t1";
        break;
    case 10:
        Rs = "$t2";
        break;
    case 11:
        Rs = "$t3";
        break;
    case 12:
        Rs = "$t4";
        break;
    case 13:
        Rs = "$t5";
        break;
    case 14:
        Rs = "$t6";
        break;
    case 15:
        Rs = "$t7";
        break;
    case 16:
        Rs = "$s0";
        break;
    case 17:
        Rs = "$s1";
        break;
    case 18:
        Rs = "$s2";
        break;
    case 19:
        Rs = "$s3";
        break;
    case 20:
        Rs = "$s4";
        break;
    case 21:
        Rs = "$s5";
        break;
    case 22:
        Rs = "$s6";
        break;
    case 23:
        Rs = "$s7";
        break;
    case 24:
        Rs = "$t8";
        break;
    case 25:
        Rs = "$t9";
        break;
    case 26:
        Rs = "$k0";
        break;
    case 27:
        Rs = "$k1";
        break;
    case 28:
        Rs = "$gp";
        break;
    case 29:
        Rs = "$sp";
        break;
    case 30:
        Rs = "$fp";
        break;
    case 31:
        Rs = "$ra";
        break;
    }

    switch (temp2) {
    case 0:
        Rt = "$zero";
        break;
    case 1:
        Rt = "$at";
        break;
    case 2:
        Rt = "$v0";
        break;
    case 3:
        Rt = "$v1";
        break;
    case 4:
        Rt = "$a0";
        break;
    case 5:
        Rt = "$a1";
        break;
    case 6:
        Rt = "$a2";
        break;
    case 7:
        Rt = "$a3";
        break;
    case 8:
        Rt = "$t0";
        break;
    case 9:
        Rt = "$t1";
        break;
    case 10:
        Rt = "$t2";
        break;
    case 11:
        Rt = "$t3";
        break;
    case 12:
        Rt = "$t4";
        break;
    case 13:
        Rt = "$t5";
        break;
    case 14:
        Rt = "$t6";
        break;
    case 15:
        Rt = "$t7";
        break;
    case 16:
        Rt = "$s0";
        break;
    case 17:
        Rt = "$s1";
        break;
    case 18:
        Rt = "$s2";
        break;
    case 19:
        Rt = "$s3";
        break;
    case 20:
        Rt = "$s4";
        break;
    case 21:
        Rt = "$s5";
        break;
    case 22:
        Rt = "$s6";
        break;
    case 23:
        Rt = "$s7";
        break;
    case 24:
        Rt = "$t8";
        break;
    case 25:
        Rt = "$t9";
        break;
    case 26:
        Rt = "$k0";
        break;
    case 27:
        Rt = "$k1";
        break;
    case 28:
        Rt = "$gp";
        break;
    case 29:
        Rt = "$sp";
        break;
    case 30:
        Rt = "$fp";
        break;
    case 31:
        Rt = "$ra";
        break;
    }

    switch (temp5) {
        case 8:
            op = "addi";
            break;
        case 9:
            op = "addiu";
            break;
        case 12:
            op = "andi";
            break;
        case 4:
            op = "beq";
            break;
        case 5:
            op = "bne";
            break;
        case 36:
            op = "lbu";
            break;
        case 37:
            op = "lhu";
            break;
        case 48:
            op = "ll";
            break;
        case 15:
            op = "lui";
            break;
        case 35:
            op = "lw";
            break;
        case 13:
            op = "ori";
            break;
        case 10: 
            op = "slti";
            break;
        case 11: 
            op = "sltiu";
            break;
        case 40:
            op = "sb";
            break;
        case 56:
            op = "sc";
            break;
        case 41:
            op = "sh";
            break;
        case 43:
            op = "sw";
            break;
    }
    cout << dec;
   RsReg = temp;
   RtReg = temp2;
    // cout << "Instruction Type: " << IT << endl;
    // cout << "Operation: " << op << endl;
    // cout << "Rs: " << Rs << " (R" << temp << ")" << endl;
    RsValue = RF[temp];
    // cout << "Rt: " << Rt << " (R" << temp2 << ")" << endl;
    RtValue = RF[temp2];
    // cout << "Immediate: " << temp3 << endl;
    iOff = instruction;
    for(int i = 0; i < 16; i++){
            iOff[i] = instruction[16];
    }
    // cout << iOff << endl;
    //cout << IT << " " << op << " " << Rs << " " << Rt << " " << temp3 << endl;
    }

    if(IT == 'J'){
    for(int i = 0; i < 6; i ++){
        if(code[0+i] == '1'){
            temp += pow(2, 5-i);
        }
    }
    for(int i = 0; i < 26; i ++){
        if(code[6+i] == '1'){
            temp2 += pow(2, 25-i);
        }
    }
    if(temp == 2)
        op = "j";
    if(temp == 3)
        op = "jal";
    // cout << "Instruction Type: " << IT << endl;
    // cout << "Operation: " << op << endl;
    // cout << "Address: "; 
    // cout << hex << temp2 << endl;
    for(int i = 0; i < 4; i ++){
        jumptarget[i] = nextinstr[i];
    }
    for(int i = 4; i < 32; i++){
        jumptarget[i] = instruction[i];
    }

    // cout << jumptarget << endl;
    //cout << IT << " " << op << " " << hex << temp2 << endl;
}
controlunit();
}


void fetch(string filename){
    ifstream inFile;
    inFile.open(filename);
    
    if (!inFile) {
        cerr << "Unable to open file datafile.txt";
        exit(1);
    }
    while(getline(inFile, instruction)){
    inFile.seekg(offset*(PC/4), inFile.beg);
    getline(inFile, instruction);
    //if(getline(inFile, nextinstr))
    // cout << "instruction length: " << instruction.length() << endl;
    if(flag == 0){
    offset = inFile.tellg();
    flag = 1;
    }
    getline(inFile, nextinstr);
    inFile.seekg(offset*((PC+4)/4), inFile.beg);
    PC += 4;
    nextPC = PC + 4;

    decode();
    }
    inFile.close();
}



int main(){
    total_clock_cycles = 0; //initialization
    for(int i = 0; i < 32; i++){
        RF[i] = 0;
        d_mem[i] = 0;
    }
    jump = 0;
    regwrite = 0;
    regdst = 0;
    branch = 0;
    alusrc = 0;
    aluop1 = 0;
    aluop2 = 0;
    memwrite = 0;
    memtoreg = 0;
    memread = 0;
    RF[9] = 32;
    RF[10] = 5;
    RF[16] = 112;
    d_mem[28] = 5;
    d_mem[29] = 16;


    cout << "Enter the program file name to run: " << endl;
    cin >> filename;

    fetch(filename);

    cout << endl;
    cout << "program terminated: " << endl;
    cout << "total execution time is " << total_clock_cycles << " cycles" << endl;


}
    
    
