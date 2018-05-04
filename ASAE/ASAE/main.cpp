//
//  main.cpp
//  ASAE (Automated Simulation Analysis Engine)
//
//  Created by Benjamin G Fields on 4/2/18.
//  Copyright © 2018 Benjamin G Fields. All rights reserved.
//
//  Description: Simulation framework that creates a simulation from a text based definition.
//  the program parses the file and creates the model. The simulation is then run producing
//  an event log of the trial run.
//  Next Step: Add normal distributions. Add rework.

#include <iostream>
#include "Simulation.hpp"
#include <fstream>
#include <stdexcept>
#include <cctype>

enum log{
  NO_VERBOSE,
  VERBOSE
};

//Description:utility function used when parsing the model file to find the end of the line
int indexOfClosingBracket(std::string line){
  int ending = 1;
  int length = 0;
  while(ending < line.length()){
    if(line[ending] == '>'){
      break;
    }
    length++;
    ending++;
  }
  return length;
}

//Description:parse the text file containing the model definition
std::vector<processInfo> getModelDefinition(){
  std::vector<processInfo> model;
  std::fstream myFile;
  myFile.open("./ASAE/model.txt");
  if(!myFile.is_open()){
    throw std::runtime_error("ERROR: failed to open file!");
  }
  std::cout<<"Model file found.\n";
  
  std::string line;
  std::getline(myFile,line);
  std::getline(myFile,line);
  int close = indexOfClosingBracket(line);
  std::string val = line.substr(1,close);
  int numProcesses = atoi(val.c_str());
  std::cout<<"Number of processes is "<<numProcesses<<std::endl;
  for(int i = 0;i<numProcesses;++i){
    processInfo info;
    std::string pTime;
    std::getline(myFile, pTime);
    std::getline(myFile, pTime);
    int ending = indexOfClosingBracket(pTime);
    info.processTime = pTime.substr(1,ending);
    std::string posType;
    std::getline(myFile, posType);
    ending = indexOfClosingBracket(posType);
    info.processPos = atoi(posType.substr(1,ending).c_str());
    std::string downStream;
    std::getline(myFile, downStream);
    ending = indexOfClosingBracket(downStream);
    info.downStream = downStream.substr(1,ending);
    std::string upStream;
    std::getline(myFile, upStream);
    ending = indexOfClosingBracket(upStream);
    info.upStream = upStream.substr(1,ending);
    model.push_back(info);
    std::getline(myFile, pTime);
    std::getline(myFile, pTime);
  }
  myFile.close();
  return model;
}

//Description:prints the model that is to be created from model file
void printModelDef(std::vector<processInfo> model){
  for (int i = 0; i<model.size(); ++i) {
    std::cout<<"Process: "<<i<<"\n";
    std::cout<<"\tProcessTime: "<<model[i].processTime<<"\n";
    std::cout<<"\tPosition: "<<model[i].processPos<<"\n";
    std::cout<<"\tDownstream connections: "<<model[i].downStream<<"\n";
    std::cout<<"\tUpstream connections: "<<model[i].upStream<<"\n";
  }
}

int main(int argc, const char * argv[]) {
  try {
    std::vector<processInfo> modelDef = getModelDefinition();
    printModelDef(modelDef);
    Simulation mySim;
    mySim.constructModel(modelDef);
    mySim.printModel();
    mySim.init();
    mySim.run(50,NO_VERBOSE);
    
  } catch (const std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  } 
  return 0;
}