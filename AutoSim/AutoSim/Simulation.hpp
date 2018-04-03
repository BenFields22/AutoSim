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
  int finished;
  int jobsComplete;
  int jobsInSystem;
  int createJobID;
  int startRecordRow;
  int FinishRecordRow;
  float simTime;
  float timeStep;
  lxw_workbook  *workbook;
  std::priority_queue <Event, std::vector<Event>, Compare> eventQueue;
  Process* simProcesses;
  int numProcesses;
public:
  Simulation();
  ~Simulation();
  int constructModel(std::vector<processInfo> &processes);
  void run(int numJobs);
  void init();
  void printModel();
  void processNextEvent();
  void checkIfFinished(int);
};

#endif /* Simulation_hpp */
