/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SIMPLE.h
 * Author: kyler
 *
 * Created on August 3, 2017, 4:07 PM
 */

#ifndef SIMPLE_H
#define SIMPLE_H

#include "Compiler.h"
#include "Simulator.h"

enum fileMode 
{ 
    INPUT_MODE, OUTPUT_MODE 
};

class SimpleComputer
{
public:    
    void openFile(fstream &file, char * fileName, fileMode mode);
    void runApplication();
    
private:
    // Files for processing
    fstream simpleFile;
    fstream SMLfile;
    fstream outputFile;
    
    // Objects to perform the processing
    Compiler convertSimpleToSML;
    Simulator executeSML;
};


#endif /* SIMPLE_H */

