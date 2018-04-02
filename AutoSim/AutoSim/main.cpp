//
//  main.cpp
//  AutoSim
//
//  Created by Benjamin G Fields on 4/2/18.
//  Copyright © 2018 Benjamin G Fields. All rights reserved.
//
//  Description: 

#include <iostream>
#include "Simulation.hpp"
#include <fstream>
#include <stdexcept>

//Description:
std::vector<processInfo> modelDefinition(){
  std::vector<processInfo> model;
  std::fstream myFile;
  myFile.open("./AutoSim/model.txt");
  if(!myFile.is_open()){
    throw std::runtime_error("ERROR: failed to open file!");
  }
  std::cout<<"\tModel file found.\n";
  myFile.close();
  return model;
}

int main(int argc, const char * argv[]) {
  try {
    std::vector<processInfo> modelDef = modelDefinition();
    Simulation mySim;
    mySim.constructModel(modelDef);
    mySim.run(10);
  } catch (const std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return 0;
}
