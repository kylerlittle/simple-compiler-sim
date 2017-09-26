/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "SIMPLE.h"

void SimpleComputer::openFile(fstream &file, char * fileName, fileMode mode)
{
    if (file.is_open())
        file.close();
    else
        switch (mode)
        {
            case fileMode::INPUT_MODE:
                file.open(fileName, std::fstream::in);
                break;
            case fileMode::OUTPUT_MODE:
                file.open(fileName, std::fstream::out);
                break;
        }
}

void SimpleComputer::runApplication()
{   
    // 1. Open file in Simple language for reading mode. Open machine language file
    //    for writing mode. Compile. Close both files.
    this->openFile(simpleFile, "/home/kyler/NetBeansProjects/Simpletron/SimpleCode.txt", fileMode::INPUT_MODE);
    this->openFile(SMLfile, "/home/kyler/NetBeansProjects/Simpletron/nbproject/MachineCode.txt", fileMode::OUTPUT_MODE);
    convertSimpleToSML.compile(simpleFile, SMLfile);
    simpleFile.close();
    SMLfile.close();
    
    // 2. Open the same machine language file as above for reading mode. Open file
    //    (which will contains the results) for writing mode. Run in simulator.
    this->openFile(SMLfile, "/home/kyler/NetBeansProjects/Simpletron/nbproject/MachineCode.txt", fileMode::INPUT_MODE);
    executeSML.runApplication(SMLfile);
    SMLfile.close();
    
    
    
    // once I'm done testing, simply use my compile public function.
    // Then make everything else private.
}