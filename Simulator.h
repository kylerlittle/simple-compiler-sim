/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Simulator.h
 * Author: kyler
 *
 * Created on July 28, 2017, 1:20 PM
 */

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <cmath>
    
using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::setw;
using std::setfill;
using std::showpos;
using std::noshowpos;
using std::internal;
using std::fstream;
    
#define SENTINEL_VALUE -999999
#define MAX_INTEGER 99999
#define MAX_WORDS 1000
#define MAX_LINE_SIZE 7
#define MAX_OPERAND_WIDTH 3
#define MAX_WORD_WIDTH 5
#define NUM_OPERATIONS 14
#define HALT 43000

struct registerStruct
{
    int accumulator;            // info is put here before calculations are performed; results stored here as well
    int instructionCounter;     // represents index in 'memory' array of next instruction to be performed
    int instructionRegister;    // next instruction to be performed
    int operationCode;          // specifies operation to be performed [first 2 digits of instructionRegister]
    int operand;                // address of memory location containing the word to which the operation applies
};
    
class Simulator
{
public:
    Simulator();
    ~Simulator();
   
    // Only this function needs to be available publicly.
    void runApplication(fstream &input);
        
private:
    // Private Methods
    void printStartMenu();
    bool loadInstructionsFromFile(fstream &inputMachineCode);
    void loadInstructionsFromKeyboard();
    bool isValidInstruction(int word);
    bool haltInstructionIsPresent();
    void executeIntructions();
    bool fatalErrorOccurred();
    bool divisionByZero();
    bool arithmeticOverflowOccurred();
    bool isInvalidOperationCode();
    void performOperation();
    void readInstructionFromKeyboard();
    void writeValueToScreen();
    void registerAndMemoryDump();
        
    // Private Attributes
    int *memory;
    registerStruct Registers;   
    };

#endif /* SIMULATOR_H */

