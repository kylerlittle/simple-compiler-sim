/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "Simulator.h"

Simulator::Simulator()
{
    memory = new int[MAX_WORDS];        // allocate memory
    for (int i = 0; i < MAX_WORDS; ++i) {
        memory[i] = 0;      // initialize each entry of memory to 0
    }
    Registers.accumulator = 0;
    Registers.instructionCounter = 0;
    Registers.instructionRegister = 0;
    Registers.operationCode = 0;
    Registers.operand = 0;
}

Simulator::~Simulator()
{
    delete[] memory;
}

void Simulator::runApplication(fstream &input)
{
    this->registerAndMemoryDump();
    if (this->loadInstructionsFromFile(input))       // LOADING PHASE
        this->executeIntructions();     // EXECUTION PHASE
}

// This function prints out instructions if you are using the Simulator
// directly (i.e. typing machine code instructions directly into the program).
// Otherwise, this function doesn't need to be used.
void Simulator::printStartMenu()
{
    cout << "*** Welcome to Simpletron! ***" << endl << endl;
    cout << "*** Please enter your program one instruction ***" << endl;
    cout << "*** (or data word) at a time. I will type the ***" << endl;
    cout << "*** location number and a question mark (?). ***" << endl;
    cout << "*** You then type the word for that location. ***" << endl;
    cout << "*** Type the sentinel -" << SENTINEL_VALUE << " to stop entering ***" << endl;
    cout << "*** your program. ***" << endl;
}

// This function loads all instructions from MachineCode.txt. If an error
// occurs, false is returned. Otherwise, true is returned.
// Possible errors include: unsuccessful opening of the file, entering a 
// word larger than the allottable 6 digits, or a program without a HALT
// code. All would result in fatal errors and thus must be checked.
bool Simulator::loadInstructionsFromFile(fstream &inputMachineCode)
{
    int instructionCounter = 0, word = 0;
    char tempString[MAX_LINE_SIZE] = "";
    
    if (inputMachineCode.is_open()) {   // check for successful opening
        while (!inputMachineCode.eof() && instructionCounter < MAX_WORDS) {   // check for end of file and memory overload
            inputMachineCode.getline(tempString, MAX_LINE_SIZE - 1);
            word = atoi(tempString);
            if (this->isValidInstruction(word)) {
                memory[instructionCounter++] = word;
            } else {
                cout << "*** Attempted enter of invalid word ***" << endl;
                cout << "*** Loading terminated early ***" << endl;
                return false;
            }   
        }
        cout << "*** Program loading completed ***" << endl;
    } else {
        cout << "*** MachineCode.txt couldn't be opened ***" << endl;
        return false;
    }
    
    // Before continuing, check to see if program instructions include 'HALT' command.
    if (!this->haltInstructionIsPresent()) {
        cout << "*** Program instructions didn't include 'halt' command ***" << endl;
        return false;
    }
    
    return true;        // Safe to continue.
}

void Simulator::loadInstructionsFromKeyboard()
{
    int instructionCounter = 0, word = 0;
    
    while (word != SENTINEL_VALUE) {        // continue asking for instructions
        do
        {
            cout << setfill('0') << setw(MAX_OPERAND_WIDTH) << instructionCounter << " ? ";
            cin >> word;
        } while (!this->isValidInstruction(word));  // not valid instruction
        memory[instructionCounter] = word;
        instructionCounter++;
    }
}

bool Simulator::isValidInstruction(int word)
{
    if ((word >= -MAX_INTEGER && word <= MAX_INTEGER) || (word == SENTINEL_VALUE))
        return true;
    return false;
}

bool Simulator::haltInstructionIsPresent()
{
    for (int i = 0; i < MAX_WORDS; ++i) {
        if (memory[i] == HALT)
            return true;
    }
    return false;
}

void Simulator::executeIntructions()
{
    cout << "*** Program execution begins ***" << endl;
    
    bool fatalErrorHasOccurred = false;
    
    while (Registers.instructionRegister != HALT && fatalErrorHasOccurred == false)
    {                                                      
        // Get current instruction from memory
        Registers.instructionRegister = memory[Registers.instructionCounter];
        
        // Extract the operation code and its corresponding operand
        Registers.operationCode = Registers.instructionRegister / MAX_WORDS;
        Registers.operand = Registers.instructionRegister % MAX_WORDS;
        
        // Check for any fatal errors.
        fatalErrorHasOccurred = this->fatalErrorOccurred();
        
        if (fatalErrorHasOccurred == false) {
            this->performOperation();
        } else {
            cout << "*** Simpletron execution abnormally terminated ***" << endl;
            this->registerAndMemoryDump();
        }
    }
}

// Returns true if fatal error has occurred. False if fatal error hasn't occurred.
bool Simulator::fatalErrorOccurred()
{
    bool result = false;

    if (this->divisionByZero()) {   // important to check this FIRST... before checking for arithmetic overflow
        result = true;
        cout << "*** Attempt to divide by zero ***" << endl;
    } else if (this->arithmeticOverflowOccurred()) {
        result = true;
        cout << "*** Result of arithmetic operation too large ***" << endl;
    } else if (this->isInvalidOperationCode()) {
        result = true;
        cout << "*** Attempt to execute invalid operation code ***" << endl;
    }

    return result;
}

// If attempted division by zero, true is returned. Else, false.
bool Simulator::divisionByZero()
{
    if (Registers.operationCode == 32 && memory[Registers.operand] == 0)
        return true;
    return false;
}

// This function is unfortunately not very readable, as C++ doesn't offer
// a great way for exception handling with arithmetic overflow. Thus, in order
// to check for such, I move one of the numbers to the other side of the equation
// and perform the check. In this way, the result of the operation will not cause
// an overflow if it exists.
// Note: modulo will not result in overflow
bool Simulator::arithmeticOverflowOccurred()
{
    if (Registers.operationCode >= 30 && Registers.operationCode <= 35) {
        switch (Registers.operationCode)
        {
            case 30:    // ADDITION
                return (MAX_INTEGER - Registers.accumulator < memory[Registers.operand] ||
                        -MAX_INTEGER - Registers.accumulator > memory[Registers.operand]);
                break;
            case 31:    // SUBTRACTION
                return (MAX_INTEGER - Registers.accumulator < -memory[Registers.operand] ||
                        MAX_INTEGER + Registers.accumulator < memory[Registers.operand]);
                break;
            case 32:    // DIVISION
                return (abs(MAX_INTEGER * memory[Registers.operand]) < abs(Registers.accumulator));
                break;
            case 33:    // MULTIPLICATION
                return (abs(MAX_INTEGER / Registers.accumulator) < abs(memory[Registers.operand]));
                break;
            case 35:    // EXPONENTIATE
                return (log(MAX_INTEGER) / memory[Registers.operand] < log(Registers.accumulator));
                break;
        }
    }
    return false;
}

bool Simulator::isInvalidOperationCode()
{
    int validCodes[NUM_OPERATIONS] = {10,11,20,21,30,31,32,33,34,35,40,41,42,43};
    for (int i = 0; i < NUM_OPERATIONS; ++i) {
        if (validCodes[i] == Registers.operationCode)   // check that operationCode is one of valid codes
            return false;
    }
    return true;
}

void Simulator::performOperation()
{
    switch (Registers.operationCode)
    {
        case 10:        // READ: from keyboard to location in memory
            this->readInstructionFromKeyboard();
            ++Registers.instructionCounter;     // advance to next instruction
            break;
        case 11:        // WRITE: from location in memory to screen
            this->writeValueToScreen();
            ++Registers.instructionCounter;     // advance to next instruction
            break;
        case 20:        // LOAD: from location in memory to accumulator
            Registers.accumulator = memory[Registers.operand];
            ++Registers.instructionCounter;     // advance to next instruction
            break;
        case 21:        // STORE: from accumulator into location in memory
            memory[Registers.operand] = Registers.accumulator;
            Registers.accumulator = 0;      // reset accumulator to 0
            ++Registers.instructionCounter;     // advance to next instruction
            break;
        case 30:        // ADD: from location in memory to word in accumulator
            Registers.accumulator += memory[Registers.operand];
            ++Registers.instructionCounter;     // advance to next instruction
            break;
        case 31:        // SUBTRACT: from accumulator a word from memory
            Registers.accumulator -= memory[Registers.operand];
            ++Registers.instructionCounter;     // advance to next instruction
            break;
        case 32:        // DIVIDE: word in accumulator by word in memory 
            Registers.accumulator /= memory[Registers.operand];
            ++Registers.instructionCounter;     // advance to next instruction
            break;
        case 33:        // MULTIPLY: word in memory by word in accumulator
            Registers.accumulator *= memory[Registers.operand];
            ++Registers.instructionCounter;     // advance to next instruction
            break;
        case 34:        // MOD: word in accumulator by word in memory
            Registers.accumulator %= memory[Registers.operand];
            ++Registers.instructionCounter;     // advance to next instruction
            break;
        case 35:        // EXPONENTIATE: word in accumulator by word in memory
            Registers.accumulator ^= memory[Registers.operand];
            ++Registers.instructionCounter;     // advance to next instruction
            break;
        case 40:        // BRANCH: to location in memory
            Registers.instructionCounter = Registers.operand;
            break;
        case 41:        // BRANCHNEG: to location in memory if accumulator is negative
            if (Registers.accumulator < 0)
                Registers.instructionCounter = Registers.operand;
            else
                ++Registers.instructionCounter;     // advance to next instruction
            break;
        case 42:        // BRANCHZERO: to location in memory if accumulator is zero
            if (Registers.accumulator == 0)
                Registers.instructionCounter = Registers.operand;
            else
                ++Registers.instructionCounter;     // advance to next instruction
            break;
        case 43:        // HALT: program has completed its task
            cout << "*** Simpletron execution terminated ***" << endl << endl;
            this->registerAndMemoryDump();
            break;
        default:        // Ignore. Bypass until HALT signal is reached.
            ++Registers.instructionCounter;     // advance to next instruction
            break;
    }
}

void Simulator::readInstructionFromKeyboard()
{
    do
    {
        cout << "Enter word <int between -" << MAX_INTEGER << " & " << MAX_INTEGER << " to be saved in memory location ";
        cout << setfill('0') << setw(MAX_OPERAND_WIDTH) << Registers.operand << " ? ";
        cin >> memory[Registers.operand];
    } while (!this->isValidInstruction(memory[Registers.operand]));  // not valid instruction
}

void Simulator::writeValueToScreen()
{
    cout << "Location: ";
    cout << setfill('0') << setw(MAX_OPERAND_WIDTH) << Registers.operand << "\t";
    cout << "Value: ";
    cout << showpos;    // Set stream to display sign.
    cout << setfill('0') << internal << setw(MAX_WORD_WIDTH + 1) << memory[Registers.operand] << endl;
    cout << noshowpos;      // Reset stream to not display sign.
}

void Simulator::registerAndMemoryDump()
{
    // Print out all the values of the registers.
    cout << "REGISTERS:" << endl;
    cout << "accumulator\t\t" << showpos << setfill('0') << internal << setw(MAX_WORD_WIDTH + 1) << Registers.accumulator << endl;
    cout << "instructionCounter\t   " << noshowpos << setfill('0') << setw(MAX_OPERAND_WIDTH) << Registers.instructionCounter << endl;
    cout << "instructionRegister\t" << showpos << setfill('0') << internal << setw(MAX_WORD_WIDTH + 1) << Registers.instructionRegister << endl;
    cout << "operationCode\t\t   " << noshowpos << setfill('0') << setw(MAX_OPERAND_WIDTH) << Registers.operationCode << endl;
    cout << "operand\t\t\t   " << noshowpos << setfill('0') << setw(MAX_OPERAND_WIDTH) << Registers.operand << endl << endl;
    
    // Print out entire contents of memory array.
    cout << "MEMORY:" << endl;
    cout << "\t  0\t  1\t  2\t  3\t  4\t  5\t  6\t  7\t  8\t  9";
    for (int i = 0; i < MAX_WORDS; ++i)
    {
        if (i % 10 == 0) {  // FIRST COLUMN, so print row headers with
            cout << endl << setfill('0') << setw(MAX_OPERAND_WIDTH) << i << "  ";      // endl to account for previous line
        }
        cout << showpos;    // Set stream to display sign.
        if (memory[i] != SENTINEL_VALUE)
            cout << setfill('0') << internal << setw(MAX_WORD_WIDTH + 1) << memory[i] << "  ";
        else
            cout << "+******" << "  ";
        cout << noshowpos;  // Reset to not display sign.
    }
    cout << endl;   // final end line
}