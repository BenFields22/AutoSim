//
//  Simulation.cpp
//  AutoSim
//
//  Created by Benjamin G Fields on 4/2/18.
//  Copyright © 2018 Benjamin G Fields. All rights reserved.
//
//  Description:

#include "Simulation.hpp"

Simulation::Simulation(){
  workbook  = workbook_new("EventLog.xlsx");
  lxw_worksheet *worksheet = workbook_add_worksheet(workbook, NULL);
  int row = 0;
  int col = 0;
  worksheet_write_string(worksheet, row, col,     "This is a new string", NULL);
  worksheet_write_number(worksheet, row, col + 1, 1.234, NULL);
}

Simulation::~Simulation(){
  workbook_close(workbook);
  std::cout<<"\tTerminating simulation\n";
}

int Simulation::constructModel(std::vector<processInfo> &processes){
  return 0;
}

void Simulation::run(int numJobs){
  std::cout<<"\tBeginning Simulation\n";
  return;
}
