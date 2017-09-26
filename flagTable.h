/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   flagTable.h
 * Author: kyler
 *
 * Created on August 13, 2017, 1:38 PM
 */

#ifndef FLAGTABLE_H
#define FLAGTABLE_H

#define NOT_A_FLAG -1

class flagTable 
{
public:
    // Constructor
    flagTable() {
        // Allocate heap memory
        flags = new int[MAX_SYMBOLS];

        // Without any passes, every entry in 'flags' should initially be -1, an invalid operand
        for (int i = 0; i < MAX_SYMBOLS; ++i) {
            flags[i] = NOT_A_FLAG;
        }
    }
    
    // Destructor
    ~flagTable() {
        delete[] flags;
    }
    
    // Getter
    int getValue(int index) const {
        return this->flags[index];
    }
    
    // Goes to the index (line number in the SML instructions) that will eventually
    // contain the correct address corresponding to unresolvedSimpleLine & sets it
    // to such.
    void markAsUnresolved(int unresolvedSimpleLine, int unresolvedSMLline) {
        flags[unresolvedSMLline] = unresolvedSimpleLine;
    }
private:
    int * flags;                    // keeps track of unresolved references
};

#endif /* FLAGTABLE_H */

