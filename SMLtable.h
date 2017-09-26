/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SMLtable.h
 * Author: kyler
 *
 * Created on August 8, 2017, 2:28 PM
 */

#ifndef SMLTABLE_H
#define SMLTABLE_H

#include <iostream>

using std::cout;
using std::endl;

#define MAX_INSTRUCTIONS 1000

class SMLtable
{
public:
    SMLtable();
    ~SMLtable();
    
    // getters
    int getNextAvailableMemForInstruction() const;
    int getNextAvailableMemForVar() const;
    int getInstructionAt(int i) const;
    
    // mutators
    void incrementInstructionCounter();
    void decrementVariableCounter();
    
    // insert instruction
    void insertSMLinstruction(int instruction);
    
    // add operand to instruction
    void resolveReference(int index, int operand);
    
private:
    int * tableOfInstructions;
    int nextAvailableMemForInstruction;  // keeps track of index of next available memory slot for instruction (incremented from 0)
    int nextAvailableMemForVar;   // keeps track of index of next available memory slot for variable or constant (decremented from 999)
    
    bool thereIsEnoughMemory();
};


#endif /* SMLTABLE_H */

