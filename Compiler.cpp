/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <istream>

#include "Compiler.h"

void Compiler::compile(fstream &input, fstream &output)
{
    this->firstPass(input);
    this->secondPass();
    this->outputSMLcode(output);
}

void Compiler::firstPass(fstream & simpleFile)
{
    int lineNum = 0, symbol = 0;
    string command = "";
    vector<string> restOfSimpleLine;
    
    if (simpleFile.is_open())
    {
        while (!simpleFile.eof() && command != "end")   // helps avoid problem of eof marker that doesn't function properly
        {
            this->tokenizeLine(simpleFile, lineNum, command, restOfSimpleLine);
            symboltable.checkForLineAndInsert(lineNum, instructionTable.getNextAvailableMemForInstruction()); // location is whatever SML instruction we're currently at
            switch (this->uniqueIdentifier(command))
            {
                case INPUT:
                    symbol = this->grabSingleSymbol(restOfSimpleLine);
                    this->inputSymbol(symbol);
                    break;
                case LET:
                    this->let(restOfSimpleLine);
                    break;
                case PRINT:
                    symbol = this->grabSingleSymbol(restOfSimpleLine);
                    this->printSymbol(symbol);
                    break;
                case GOTO:    // control transfer
                    this->branchTo(restOfSimpleLine, ControlTransfer::BRANCH);
                    break;
                case IF:
                    this->ifGoTo(restOfSimpleLine);
                    break;
                case END:
                    this->end();
                    break;
            }
            this->clearTokens(lineNum, command, restOfSimpleLine);
        }
    }
}

void Compiler::secondPass()
{
    symboltable.printSymbolTable();
    
    int indexOfBranch = 0, SMLline = 0;
    
    for (int i = 0; i < MAX_SYMBOLS; ++i) {
        if (flags.getValue(i) != NOT_A_FLAG) {      // i.e. is a flag
            // 1. Locate branchTo line in the symbol table.
            indexOfBranch = symboltable.locateSymbol(flags.getValue(i));
            
            // 2. Get its location attribute (i.e. the line in the SML instructions).
            SMLline = symboltable.getLocationOf(indexOfBranch);
            
            // 3. SMLline is the operand. Add this to where branch instruction is: i.
            instructionTable.resolveReference(i, SMLline);
        }
    }
}
    
void Compiler::outputSMLcode(fstream & outputFile)
{
    int instruction = 0, halt = static_cast<int>(ControlTransfer::HALT) * MAX_WORDS;
    
    if (outputFile.is_open()) {
        for (int i = 0; i < instructionTable.getNextAvailableMemForInstruction(); ++i) {
            instruction = instructionTable.getInstructionAt(i); 
            outputFile << instruction;
            if (instruction != halt)    // if not on the last line, include an endline character
                outputFile << std::endl;
        }
    }
}

int Compiler::uniqueIdentifier(string &command)
{
    int letterSum = 0;
    for (int i = 0; i < command.length(); ++i) {
        letterSum += static_cast<int>(command.at(i));       // sum up the ASCII values of each char
    }
    return letterSum;
}

void Compiler::tokenizeLine(fstream &input, int &lineNumber, string &command, vector<string> &theRest)
{
    char tempString[MAX_INPUT_SIZE] = "", *token = NULL;
    
    input.getline(tempString, MAX_INPUT_SIZE - 1, ' ');
    lineNumber = atoi(tempString);
    input.getline(tempString, MAX_INPUT_SIZE - 1, ' ');
    command.assign(tempString);
    if (command.at(0) != 'r')   // check to see if command is rem; only need to check 1st char to be a little more efficient
    {          // if command isn't rem, continue tokenizing
        input.getline(tempString, MAX_INPUT_SIZE - 1);      // Grab remainder of the line.
        token = strtok(tempString, " ");        // First call to strtok.
        
        while (token != NULL) {         // continue tokenizing until we reach the end of the line
            if (token[0] != '\n') {     // Ensure we're not tokenizing the new line.
                // 1. Process token.
                string tokenString(token);      // create std::string from char * using constructor
                if (tokenString.front() == '(') {     // then we need a separate token so it's not intermingled with the number
                    theRest.push_back("(");
                    theRest.push_back(tokenString.substr(1, MAX_INPUT_SIZE));    //
                } else if (tokenString.back() == ')') {      // same as first reasoning...
                    theRest.push_back(")");
                    tokenString.pop_back();
                    theRest.push_back(tokenString);
                } else {           // no problems, just append
                    theRest.push_back(tokenString);
                } 
                // 2. Grab next token.
                token = strtok(NULL, " ");
            }
        }
    } else {
        input.getline(tempString, MAX_INPUT_SIZE - 1);  // grab the rest of remark & discard
    }
}

void Compiler::clearTokens(int &lineNumber, string &command, vector<string> &theRest)
{
    lineNumber = 0;
    command.clear();
    theRest.clear();
}

int Compiler::grabSingleSymbol(vector<string> &theRest)
{
    return static_cast<int>(theRest.front().front());       // grab first char & convert to ASCII representation
}

void Compiler::inputSymbol(int symbol)
{
    bool symbolWasFound = symboltable.checkForVarAndInsert(symbol, instructionTable.getNextAvailableMemForVar());
    if (symbolWasFound == false) {
        int opCode = static_cast<int>(InputOutputOps::READ);  // cast scoped enum to integer
        instructionTable.insertSMLinstruction(opCode * MAX_WORDS + symboltable.getLocationOfLatestItem());
        instructionTable.decrementVariableCounter();     // thus, we won't be overriding anything accidentally later 
    }
}

void Compiler::printSymbol(int symbol)
{
    int indexOfSymbolInTable = symboltable.locateSymbol(symbol), operationCode = 0;
    if (indexOfSymbolInTable == NOT_FOUND) {
        cout << "Variable '" << (char)symbol << "' doesn't exist." << endl;
    } else {       // symbol was found and can be printed
        operationCode = static_cast<int>(InputOutputOps::WRITE);  // cast scoped enum to integer
        instructionTable.insertSMLinstruction(operationCode * MAX_WORDS + symboltable.getLocationOf(indexOfSymbolInTable));
    }
}

void Compiler::loadSingleSymbol(int symbol)
{
    bool symbolWasFound = symboltable.checkForVarAndInsert(symbol, instructionTable.getNextAvailableMemForVar());
    if (symbolWasFound == false)
        instructionTable.decrementVariableCounter();    // allocating the position for said variable
}

void Compiler::loadMultipleSymbols(vector<string> &theRest)
{
    for (int i = 0; i < theRest.size(); ++i) {
        if (this->isNumber(theRest.at(i)) || islower(theRest.at(i).front())) {      // if number or lower case letter
            int symbol = this->convertSymbolToInt(theRest.at(i));
            this->loadSingleSymbol(symbol);
        }
    }
}

void Compiler::let(vector<string> &expression)
{
    this->loadMultipleSymbols(expression);  // load all symbols in remainder of line if they haven't been loaded yet
    
    vector <string> postfix;        // vector to store postfix expression
    // get location of symbol to left of assignment operator (use as temporary storage for calculations)
    int symbol = this->convertSymbolToInt(expression.at(START_OF_LHS)), indexOfLHS = symboltable.locateSymbol(symbol),
            locationOfLHS = symboltable.getLocationOf(indexOfLHS);
    
    this->infixToPostfix(expression, postfix);      // convert expression to postfix (retain vector form)
    this->evaluatePostfix(postfix, locationOfLHS);
}
    
void Compiler::infixToPostfix(vector <string> &infix, vector <string> &postfix)
{
    stack<char> helper;
    string rightPar = ")", pushThisOperator = "";
    
    helper.push('(');
    infix.push_back(rightPar);
    for (int i = START_OF_RHS; helper.empty() == false; ++i)        // while stack not empty, read infix from left to right
    {
        if (this->isNumber(infix.at(i)) || islower(infix.at(i).front())) {    // character is a number or variable storing a number  
            postfix.push_back(infix.at(i));                         // push_back current string onto postfix
        } else if (infix.at(i) == "(") {                         // character is left parenthesis
            helper.push('(');       // push char onto stack
        } else if (this->isOperator(infix.at(i).front())) {              // character is an operator                    
            while (this->isOperator(helper.top()) && this->precedence(helper.top(), infix.at(i).front())) 
            {      // Peek char on top. If operator, continue loop while precedence is greater than or equal to current operator.
                pushThisOperator.assign(1, helper.top());   // assign value of string to the operator character
                postfix.push_back(pushThisOperator);
                helper.pop();       // then, pop the operator
            }
            helper.push(infix.at(i).front());
        } else if (infix.at(i) == ")") {                         // character is right parenthesis
            while (helper.top() != '(') {
                pushThisOperator.assign(1, helper.top());   // assign value of string to the operator character
                postfix.push_back(pushThisOperator);        // push the operator onto postfix
                helper.pop();
            }
            helper.pop();       // discard remaining left parenthesis
        }
    }
}

void Compiler::evaluatePostfix(vector <string> &postfix, int locationOfLHS)
{
    stack<int> digits;
    int rightOperand = 0, leftOperand = 0, loadOpCode = static_cast<int>(LoadStoreOps::LOAD),
            storeResultOp = static_cast<int>(LoadStoreOps::STORE), arithmeticOp = 0;
    
    for (int i = 0; i < postfix.size(); ++i) {
        if (this->isOperator(postfix.at(i).front())) {
            // 1. Determine the operation code of the arithmetic operator.
            arithmeticOp = static_cast<int>(this->returnOperationCode(postfix.at(i).front()));
            
            // 2. Pop top two items. Assumes valid postfix notation (i.e. no checking of empty stack).
            rightOperand = digits.top();
            digits.pop();
            leftOperand = digits.top();
            digits.pop();
            
            // 3. Load left operand into accumulator. Write SML to perform operation on accumulator
            // from memory location of right operand. Store temporary (or final) result into LHS of equation.
            instructionTable.insertSMLinstruction(loadOpCode * MAX_SYMBOLS + leftOperand);  // load left operand into accumulator
            instructionTable.insertSMLinstruction(arithmeticOp * MAX_SYMBOLS + rightOperand);
            instructionTable.insertSMLinstruction(storeResultOp * MAX_SYMBOLS + locationOfLHS);
            
            // 4. Push location of result onto the stack.
            digits.push(locationOfLHS);      
        } else {        // we're dealing with a variable or number, so push its location onto the stack
            int symbol = this->convertSymbolToInt(postfix.at(i)), indexOfSymbol = symboltable.locateSymbol(symbol);
            digits.push(symboltable.getLocationOf(indexOfSymbol));
        }
    }
}

bool Compiler::isOperator(char op)
{
    string operatorList = "+-*/^%";
    for (int i = 0; i < operatorList.length(); ++i) {
        if (operatorList.at(i) == op)
            return true;
    }
    return false;
}

bool Compiler::isNumber(string &numString)
{
    for (int i = 0; i < numString.length(); ++i) {
        if (!isdigit(numString.at(i)))        // character is not a digit
            return false;
    }
    return true;
}

bool Compiler::precedence(char op1, char op2)
{
    vector<string> operatorsWithPrecedence = { "^", "*/%", "+-" };
    int indexOfPrecedence1 = 0, indexOfPrecedence2 = 0;
    
    for (int i = 0; i < operatorsWithPrecedence.size(); ++i) {
        for (int j = 0; j < operatorsWithPrecedence.at(i).length(); ++j) {
            if (operatorsWithPrecedence.at(i).at(j) == op1)
                indexOfPrecedence1 = i;
            if (operatorsWithPrecedence.at(i).at(j) == op2)
                indexOfPrecedence2 = i;
        }
    }
    
    return (indexOfPrecedence1 >= indexOfPrecedence2);
}

ArithmeticOps Compiler::returnOperationCode(char op)
{
    switch (op) 
    {
        case '+':
            return ArithmeticOps::ADD;
        case '-':
            return ArithmeticOps::SUBTRACT;
        case '/':
            return ArithmeticOps::DIVIDE;
        case '*':
            return ArithmeticOps::MULTIPLY;
        case '%':
            return ArithmeticOps::MOD;
        case '^':
            return ArithmeticOps::EXPONENTIATE;
    }
}

void Compiler::branchTo(vector<string> &theRest, ControlTransfer type)
{
    int branchTo = this->convertSymbolToInt(theRest.back()), operationCode = static_cast<int>(type),
            locationOfOperand = 0, indexOfBranch = 0;

    // Check for branchTo symbol in symbolTable. If found, produce SML instruction.
    if (symboltable.symbolWasFound(branchTo)) { // operation code is contained in 'type' variable
        indexOfBranch = symboltable.locateSymbol(branchTo);
        locationOfOperand = symboltable.getLocationOf(indexOfBranch);        // update location!
    } else {    // If it is not found, mark as unresolved reference.
        flags.markAsUnresolved(branchTo, instructionTable.getNextAvailableMemForInstruction());
    }
    // Insert SML instruction regardless. We will update location of operand in the second pass of the compiler.
    instructionTable.insertSMLinstruction(operationCode * MAX_WORDS + locationOfOperand);
}

void Compiler::ifGoTo(vector <string> &theRest)
{
    int leftSymbol = 0, rightSymbol = 0;
    string comparisonOperator;
    
    this->tokenizeIfStatement(theRest, leftSymbol, comparisonOperator, rightSymbol);
    this->loadSingleSymbol(leftSymbol);    // Load the left & right symbols into symbolTable if not there already
    this->loadSingleSymbol(rightSymbol);
    this->produceSMLforComparison(theRest, leftSymbol, comparisonOperator, rightSymbol);
}

void Compiler::tokenizeIfStatement(vector <string> &theRest, int &leftSymbol, string &comparisonOperator, int &rightSymbol)
{
    int tokenCounter = 0;

    leftSymbol = this->convertSymbolToInt(theRest.at(tokenCounter++));
    comparisonOperator.assign(theRest.at(tokenCounter++));
    rightSymbol = this->convertSymbolToInt(theRest.at(tokenCounter++));
}

void Compiler::produceSMLforComparison(vector <string> &theRest, int leftSymbol, string &comparisonOperator, int rightSymbol)
{
    this->loadLeftSubtractRight(leftSymbol, rightSymbol);
    this->loadSingleSymbol(-1);       // load this automatically for comparisons
    int negOneLocation = symboltable.locateSymbol(-1), multCode = static_cast<int>(ArithmeticOps::MULTIPLY);
    if (comparisonOperator[0] == '>') {     // multiply result by -1, branchneg
        instructionTable.insertSMLinstruction(multCode * MAX_WORDS + symboltable.getLocationOf(negOneLocation));
        this->branchTo(theRest, ControlTransfer::BRANCH_NEG);
        if (comparisonOperator[1] == '=')   // also branchzero (simply append this)
            this->branchTo(theRest, ControlTransfer::BRANCH_ZERO);
    } else if (comparisonOperator[0] == '<') {      // branchneg
        this->branchTo(theRest, ControlTransfer::BRANCH_NEG);
        if (comparisonOperator[1] == '=')
            this->branchTo(theRest, ControlTransfer::BRANCH_ZERO);
    } else if (comparisonOperator[0] == '=') {
        this->branchTo(theRest, ControlTransfer::BRANCH_ZERO);
    } else {        // then the comparison operator must be != (or it could be a syntax error)
        this->branchTo(theRest, ControlTransfer::BRANCH_NEG);       // branchneg
        instructionTable.insertSMLinstruction(multCode * MAX_WORDS + symboltable.getLocationOf(negOneLocation));    // multiply by -1
        this->branchTo(theRest, ControlTransfer::BRANCH_NEG);       // branchneg once more
    }
}

void Compiler::loadLeftSubtractRight(int leftSymbol, int rightSymbol)
{
    int leftIndex = symboltable.locateSymbol(leftSymbol), rightIndex = symboltable.locateSymbol(rightSymbol),
            loadCode = static_cast<int>(LoadStoreOps::LOAD), subCode = static_cast<int>(ArithmeticOps::SUBTRACT);
    instructionTable.insertSMLinstruction(loadCode * MAX_WORDS + symboltable.getLocationOf(leftIndex));
    instructionTable.insertSMLinstruction(subCode * MAX_WORDS + symboltable.getLocationOf(rightIndex));
}

int Compiler::convertSymbolToInt(string &symbol)
{
    char firstChar = symbol.front();
    
    if (islower(firstChar)) {       // then it's a variable
        return static_cast<int>(firstChar);
    } else {
        try {
            int numFromString = stoi(symbol, nullptr, BASE_TEN);
            return numFromString;
        }
        catch (const std::invalid_argument& ia) {   
            cout << "Unable to convert string to integer: " << ia.what() << endl;
        }
    }
}

void Compiler::end()
{
    int operationCode = static_cast<int>(ControlTransfer::HALT);  // cast scoped enum to integer
    instructionTable.insertSMLinstruction(operationCode * MAX_WORDS);  // HALT command
}