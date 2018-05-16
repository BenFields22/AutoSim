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
//  Next Step: Add extraction of buffer data. Add normal distributions and constant time. Add rework.

#include <iostream>
#include "Simulation.hpp"
#include "DataCrawler.hpp"
#include <fstream>
#include <stdexcept>
#include <cctype>

enum log{
  NO_VERBOSE,
  VERBOSE
};
//***********************************************************
//Function prototypes
int indexOfClosingBracket(std::string line);
std::vector<processInfo> getModelDefinition( int* numJobs);
void printModelDef(std::vector<processInfo> model);
//***********************************************************

//Description: Main entry point in the program
int main(int argc, const char * argv[]) {
  //try block to encapsulate the simulation logic
  try {
    int numJobs;
    std::vector<processInfo> modelDef = getModelDefinition(&numJobs);
    Simulation mySim;
    mySim.constructModel(modelDef);
    mySim.printModel();
    mySim.init();
    std::cout<<"Running Simulation with "<<numJobs<<" Jobs\n";
    mySim.run(numJobs,NO_VERBOSE);
  } catch (const std::runtime_error& e) {
    std::cout<<"\nERROR in Simulation\n";
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  
  //try block to encapsulate the dataCrawler logic
  try{
    //create a dataCrawler
    DataCrawler myCrawler;
    myCrawler.run();
  }catch (const std::runtime_error& e) {
    std::cout<<"\nERROR in data Crawler!\n";
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

//Description:parse the text file containing the model definition
std::vector<processInfo> getModelDefinition( int* numJobs){
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
  *numJobs = atoi(val.c_str());
  std::getline(myFile,line);
  close = indexOfClosingBracket(line);
  val = line.substr(1,close);
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
