//
//  Simulation.hpp
//  AutoSim
//
//  Created by Benjamin G Fields on 4/2/18.
//  Copyright © 2018 Benjamin G Fields. All rights reserved.
//
//  Description: 

#ifndef Simulation_hpp
#define Simulation_hpp

#include <stdio.h>
#include <queue>
#include <vector>
#include "Event.hpp"
#include "Process.hpp"
#include "xlsxwriter.h"
#include <iostream>

//Description:
class Simulation{
private:
  float simTime;
  lxw_workbook  *workbook;
  std::priority_queue <Event, std::vector<Event>, Compare> eventQueue;
  Process* simProcesses;
public:
  Simulation();
  ~Simulation();
  int constructModel(std::vector<processInfo> &processes);
  void run(int numJobs);
};

#endif /* Simulation_hpp */
