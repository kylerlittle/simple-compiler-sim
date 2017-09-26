/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "SMLtable.h"

SMLtable::SMLtable() 
{
    tableOfInstructions = new int[MAX_INSTRUCTIONS];
    nextAvailableMemForInstruction = 0;          // first available memory slot
    nextAvailableMemForVar = MAX_INSTRUCTIONS - 1;     // variables are started from 999 and decrement from there
}

SMLtable::~SMLtable()
{
    delete[] tableOfInstructions;
}

int SMLtable::getNextAvailableMemForInstruction() const
{
    return this->nextAvailableMemForInstruction;
}

int SMLtable::getNextAvailableMemForVar() const
{
    return this->nextAvailableMemForVar;
}

int SMLtable::getInstructionAt(int i) const
{
    return this->tableOfInstructions[i];
}
   
void SMLtable::incrementInstructionCounter()
{
    this->nextAvailableMemForInstruction += 1;
}

void SMLtable::decrementVariableCounter()
{
    this->nextAvailableMemForVar -= 1;
}

void SMLtable::insertSMLinstruction(int instruction)
{
    if (this->thereIsEnoughMemory()) {
        tableOfInstructions[nextAvailableMemForInstruction] = instruction;
        this->incrementInstructionCounter();
    } else {
        cout << "Not enough Simpletron memory for all instructions." << endl;
    }
}

void SMLtable::resolveReference(int index, int operand) 
{
    tableOfInstructions[index] += operand;
}

bool SMLtable::thereIsEnoughMemory()
{
    return (nextAvailableMemForInstruction < nextAvailableMemForVar);
}