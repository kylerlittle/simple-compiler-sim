/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   symbolTable.h
 * Author: kyler
 *
 * Created on August 11, 2017, 7:29 AM
 */

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <locale>               // islower()
#include <iostream>

#define NOT_FOUND -1
#define MAX_SYMBOLS 1000

class tableEntry
{
public:
    tableEntry() {
        symbol = 0;
        type = '\0';
        location = 0;
    }
    
    // Getters
    int getSymbol() const {
        return this->symbol;
    }
    char getType() const {
        return this->type;
    }
    int getLocation() const {
        return this->location;
    }
    
    // Setters
    void setSymbol(const int newSymbol) {
        this->symbol = newSymbol;
    }
    void setType(const char newType) {
        this->type = newType;
    }
    void setLocation(const int newLocation) {
        this->location = newLocation;
    }
    void setEntry(const int newSymbol, const char newType, const int newLocation)
    {
        this->symbol = newSymbol;
        this->type = newType;
        this->location = newLocation;
    }
    
private:
    int symbol;     // ASCII representation of a variable, line number, or constant
    char type;      // One of three letters: 'C' constant, 'L' line number, or 'V' variable
    int location;   // location in Simpletron's memory to which the symbol refers
                            // For line number, refers to element index in Simpletron memory where instructions begin
                            // For variable/constant, refers to element index in Simpletron memory where variable/constant is stored
};

class symbolTable
{
public:
    symbolTable() {
        symboltable = new tableEntry[MAX_SYMBOLS];
        nextAvailableSymbol = 0;   // first available index
    }
    ~symbolTable() {
        delete[] symboltable;
    }
    
    // Returns the location attribute of the tableEntry located at index.
    int getLocationOf(int index) {
        return symboltable[index].getLocation();
    }
    
    // Returns the location attribute of the tableEntry that was last inserted.
    int getLocationOfLatestItem() {
        return symboltable[nextAvailableSymbol - 1].getLocation();
    }
    
    // If symbol is found in symbolTable, true is returned. Else, false is returned.
    bool symbolWasFound(int symbol) {
        for (int i = 0; i < this->nextAvailableSymbol; ++i) {
            if (symboltable[i].getSymbol() == symbol)
                return true;
        }
        return false;
    }
    
    // If symbol is found in symbolTable, then its index is returned. If
    // it is not found, NOT_FOUND, an invalid macro, is returned.
    int locateSymbol(int symbol) {
        for (int i = 0; i < this->nextAvailableSymbol; ++i) {
            if (symboltable[i].getSymbol() == symbol)
                return i;
        }
        return NOT_FOUND;
    }
    
    // If the line number (i.e. the symbol) is found in the symbolTable, then true is
    // returned. Otherwise, false is returned, and the lineNumber is inserted into
    // the symbol table with symbol: 'lineNumber', type: 'L', and location:
    // 'next available memory slot in the SML table.'
    // nextAvailableSymbol is post-incremented.
    bool checkForLineAndInsert(int lineNumber, int location) {
        if (this->symbolWasFound(lineNumber) == false)      // line number hasn't been inserted yet
        {
            symboltable[nextAvailableSymbol++].setEntry(lineNumber, 'L', location);
            return false;
        }
        return true;
    }
    
    // If the symbol (can be either variable or constant) is found in the symbolTable,
    // then true is returned. Otherwise, false is returned, and the symbol is inserted
    // into the symboltable.
    // nextAvailableSymbol is post-incremented.
    bool checkForVarAndInsert(int var, int location) {
        if (this->symbolWasFound(var) == false) {
            if (islower((char)var))    // lower case letter, indicating it's a variable
                symboltable[nextAvailableSymbol++].setEntry(var, 'V', location);
            else                    // should be some random number
                symboltable[nextAvailableSymbol++].setEntry(var, 'C', location);
            return false;
        }
        return true;
    }
    
    // Prints the filled contents of symboltable. Only used for debugging
    // purposes. Provides no additional functionality to Compiler class.
    void printSymbolTable() {
        for (int i = 0; i < this->nextAvailableSymbol; ++i) {
            std::cout << "Symbol: " << symboltable[i].getSymbol() << '\t';
            std::cout << "Type: " << symboltable[i].getType() << "\t\t";
            std::cout << "Location: " << symboltable[i].getLocation() << std::endl;
        }
    }
    
private:
   tableEntry * symboltable;
   int nextAvailableSymbol;        // keeps track of next available slot to append to symbolTable
};

#endif /* SYMBOLTABLE_H */

