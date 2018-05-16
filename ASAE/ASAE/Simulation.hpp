//
//  Simulation.hpp
//  ASAE
//
//  Created by Benjamin G Fields on 4/2/18.
//  Copyright © 2018 Benjamin G Fields. All rights reserved.
//
//  Description: defines the use and structure of the simulation class

#ifndef Simulation_hpp
#define Simulation_hpp

#include <stdio.h>
#include <queue>
#include <vector>
#include <algorithm>
#include "Event.hpp"
#include "Process.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <unordered_set>
#include <sstream>

//Description: main class that runs the simulation
class Simulation{
private:
  int debug;
  std::fstream startFile;
  std::fstream finishFile;
  std::fstream resultsFile;
  int finished;
  int jobsComplete;
  int jobsInSystem;
  int createJobID;
  float simTime;
  float timeStep;
  std::priority_queue <Event, std::vector<Event>, Compare> eventQueue;
  Process* simProcesses;
  int numProcesses;
public:
  int startRecordRow;
  int FinishRecordRow;
  Simulation();
  ~Simulation();
  int constructModel(std::vector<processInfo> &processes);
  int getFeedBufferState(Process P);
  void run(int numJobs, int verbose);
  void init();
  void printModel();
  void processNextEvent();
  nextEventInfo processCurrentEvent(Event current, int Process);
  void checkIfFinished(int);
  int getNumComponents(int);
  int getNumberOfEnterPoints(int processID);
};

#endif /* Simulation_hpp */
