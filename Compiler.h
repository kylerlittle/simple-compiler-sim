/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Compiler.h
 * Author: kyler
 *
 * Created on August 3, 2017, 3:59 PM
 */

#ifndef COMPILER_H
#define COMPILER_H

#include <iostream>
#include <fstream>
#include <cstdlib>              // atoi()
#include <string>
#include <cstring>
#include <vector>
#include <stack>
#include "symbolTable.h"
#include "SMLtable.h"
#include "flagTable.h"

using std::fstream;
using std::string;
using std::stoi;
using std::stack;
using std::vector;
using std::islower;
using std::isdigit;

// Macros
#define MAX_WORDS 1000
#define MAX_COMMAND_SIZE 6
#define MAX_INPUT_SIZE 50
#define START_OF_LHS 0
#define START_OF_RHS 2
#define BASE_TEN 10

// Commands & Their Unique Identifiers
#define REM 324
#define INPUT 560
#define LET 325
#define PRINT 557
#define GOTO 441
#define IF 207
#define END 311

enum class InputOutputOps { READ = 10, WRITE };
enum class LoadStoreOps { LOAD = 20, STORE };
enum class ArithmeticOps { ADD = 30, SUBTRACT, DIVIDE, MULTIPLY, MOD, EXPONENTIATE };
enum class ControlTransfer { BRANCH = 40, BRANCH_NEG, BRANCH_ZERO, HALT };

class Compiler
{
public:
    // Encapsulates the entire compilation process.
    // Requires an input file in the high-level Simple language. Outputs SML
    // code to the output file.
    void compile(fstream &input, fstream &output);
private:
    /* PRIVATE ATTRIBUTES */
    symbolTable symboltable;
    flagTable flags;
    SMLtable instructionTable;
    
    /* PRIVATE METHODS */
    // This is the first pass of the compiler.
    // Assumes fstream has been opened for std::ios::in mode. However, function
    // will error check for whether or not file is open.
    // This function will compile the instructions in the high-level Simple
    // language into SML code. All instructions, except for any unresolved
    // references from a 'goto' or 'if...goto' command, are compiled in this pass.
    // Instructions in Simple are received from the simpleFile.
    void firstPass(fstream & simpleFile);
    
    // This is the second pass of the compiler.
    // This function resolves any unresolved references so that the operand
    // of the instruction is no longer 000. This is done by simply adding the
    // location attribute of the referenced object. 
    void secondPass();
    
    // This function outputs the entire contents of the instructionTable to
    // outputFile. It will check if outputFile is open before proceeding.
    void outputSMLcode(fstream & outputFile);
    
    // This function returns an integral value which is the sum of characters
    // in the 'command' string. This is done so that a switch statement
    // may be used based on the command.
    int uniqueIdentifier(string &command);
    
    // Each line is guaranteed to begin with a line number, followed by a space
    // and a one word command. There are likely statements (and a space) after
    // this, but it is not guaranteed. Thus, it is checked for. If there are 
    // statements afterwards, then they are stored in 'theRest.'
    void tokenizeLine(fstream &input, int &lineNumber, string &command, vector<string> &theRest);
    
    // This function sets lineNumber to 0 and command[0] to the null char; it also
    // clears 'theRest' vector.
    void clearTokens(int &lineNumber, string &command, vector<string> &theRest);
    
    // Returns the first element in the vector as an integer. Clears vector
    // afterwards.
    int grabSingleSymbol(vector<string> &theRest);
    
    // Checks to see if symbol is in the table. If it is not, then this function
    // produces SML instructions to get the user's input. Otherwise, nothing is done.
    void inputSymbol(int symbol);
    
    // Finds the symbol in the table. If it exists, then an SML instruction
    // will be added to the intructionTable to print the symbol. Otherwise,
    // an error message is printed to the console.
    void printSymbol(int symbol);
    
    // Searches table for symbol. If it doesn't exist, then it is added to the table.
    // Otherwise, if it is not found, then nothing is done.
    void loadSingleSymbol(int symbol);
    
    // If symbol is a number or a letter, then we search the symboltable for it.
    // If it is found, an SML instruction is produced. Otherwise, nothing is done.
    // Continues until all unloaded symbols are loaded.
    void loadMultipleSymbols(vector<string> &theRest);
    
    // Encapsulates the 'let' command.
    // Writes the SML instructions to evaluate whatever arbitrary expression is to
    // the right of the assignment operator.
    // Writes SML instruction to store result in variable to left of assignment operator.
    void let(vector<string> &expression);
    
    // This function converts the infix expression (in tokens from the vector) to
    // a vector containing the postfix expression.
    // NOTE: starting point for conversion of infix is index 2. This is because
    // it is the first token on the right hand side of the assignment operator.
    void infixToPostfix(vector <string> &infix, vector <string> &postfix);
    
    // This function "evaluates" the postfix expression. It doesn't actually
    // perform any calculations. Rather, it writes the SML instructions to 
    // evaluate the expression. This function encapsulates the general
    // algorithm for postfix evaluation.
    void evaluatePostfix(vector <string> &postfix, int locationOfLHS);
    
    // Returns true if the character is an operator. False otherwise.
    bool isOperator(char op);
    
    // Returns true if the string is a number. False otherwise.
    bool isNumber(string &numString);
    
    // Returns true if operator #1 is greater than or equal to precedence of 
    // operator #2. False otherwise.
    bool precedence(char op1, char op2);
    
    // Returns the proper operation code (in form of a scoped enum) for
    // the given operator.
    ArithmeticOps returnOperationCode(char op);
    
    // The last element in the vector is converted to an integer value. If
    // that line number is found in the symbol table, then the SML instruction
    // for branching are produced.
    // NOTE: in this function, vector is NOT cleared.
    void branchTo(vector<string> &theRest, ControlTransfer type);
    
    // This function encapsulates all aspects of the if...goto command. It first
    // tokenizes theRest of the statement, grabbing the two symbols/numbers
    // being compared and the comparison operator. It then checks if the two 
    // symbols being compared are in the symbol table. If not, it inserts them. 
    // Then, SML instructions are produced to perform the comparison &
    // branchTo produces the necessary SML instruction.
    void ifGoTo(vector <string> &theRest);
    
    // From theRest vector, this function simply picks off the first, second, and third elements
    // (such as in an expression like 'x > y') and stores the results into the
    // respective parameters.
    void tokenizeIfStatement(vector <string> &theRest, int &leftSymbol, string &comparisonOperator, int &rightSymbol);
    
    // This function produces SML instructions to compare the left and right symbols
    // Acceptable operators include <, >, <=, >=, ==, !=
    // Function automatically adds instructions to instructionTable
    void produceSMLforComparison(vector <string> &theRest, int leftSymbol, string &comparisonOperator, int rightSymbol);
    
    // Produces SML instructions for the following:
    // Loads the left symbol into the accumulator.
    // Subtracts the right symbol symbol from this value
    // Answer is now in accumulator in the Simulator. Branch based on result.
    void loadLeftSubtractRight(int leftSymbol, int rightSymbol);
    
    // This function checks to see if the symbol string is a lower case letter;
    // if it is, then the ASCII version of the symbol is returned. Otherwise,
    // the symbol should be a number (in string format) and stoi will be used
    // to convert it. Exception handling will still be performed in case this
    // is not the case.
    int convertSymbolToInt(string &symbol);
    
    // Appends 'HALT' instruction in SML to instruction table.
    void end();
};

#endif /* COMPILER_H */

