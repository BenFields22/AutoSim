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
  simTime = 0.0;
  jobsComplete = 0;
  jobsInSystem = 0;
  finished = 0;
  createJobID = 1;
  startRecordRow = 1;
  FinishRecordRow = 1;
  timeStep = 0.02;
  workbook  = workbook_new("EventLog.xlsx");
  lxw_worksheet *worksheet = workbook_add_worksheet(workbook, NULL);
  worksheet_write_string(worksheet, 0, 0,"JobID Start", NULL);
  worksheet_write_string(worksheet, 0, 1,"ProcessID", NULL);
  worksheet_write_string(worksheet, 0, 2,"StartTime", NULL);
  worksheet_write_string(worksheet, 0, 3,"Resource", NULL);
  worksheet_write_string(worksheet, 0, 5,"JobID", NULL);
  worksheet_write_string(worksheet, 0, 6,"ProcessID", NULL);
  worksheet_write_string(worksheet, 0, 7,"ExitTime", NULL);
}

Simulation::~Simulation(){
  workbook_close(workbook);
  std::cout<<"Terminating simulation\n";
}

int Simulation::constructModel(std::vector<processInfo> &processes){
  std::cout<<"Constructing the simulation model\n";
  numProcesses = (int)processes.size();
  simProcesses = new Process[numProcesses];
  for (int i = 0; i<numProcesses; ++i) {
    if(processes[i].processTime[0] == 'T'){
      std::cout<<"Process "<<i<< " is Triangular \n";
      simProcesses[i].setDistType(TRIANGULAR);
    }
    else if(processes[i].processTime[0] == 'N'){
      std::cout<<"Process "<<i<< " is Normal \n";
      simProcesses[i].setDistType(NORMAL);
    }
    else if(processes[i].processTime[0] == 'U'){
      std::cout<<"Process "<<i<< " is Uniform \n";
      simProcesses[i].setDistType(UNIFORM);
    }
    simProcesses[i].setProcessParameters(processes[i].processTime);
    if(processes[i].bufferCapacity != "X")
      simProcesses[i].setBufferCapacity(atoi(processes[i].bufferCapacity.c_str()));
    simProcesses[i].setProcessType(processes[i].processPos);
    simProcesses[i].setUpstreamDependencies(processes[i].upStream);
    simProcesses[i].setDownstreamDependencies(processes[i].downStream);
    simProcesses[i].setProcessID(i);
  }
  return 0;
}

void Simulation::init(){
  for (int i = 0; i< numProcesses; ++i) {
    if (simProcesses[i].getProcessType() == FRONT) {
      //Event E(i,createJobID,START,simTime);
      Event E(i,createJobID,FINISH,simTime);
      eventQueue.push(E);
    }
    else{
      //schedule pull to start cycle
    }
  }
}

void Simulation::processNextEvent(){
  int size = (int)eventQueue.size();
  std::cout<<"\nQueue Size is "<<size<<"\n";
  Event currentEvent = eventQueue.top();
  if (currentEvent.getEventType() == FINISH) {
    jobsComplete++;
  }
  currentEvent.processEvent();
  eventQueue.pop();
}

void Simulation::printModel(){
  std::cout<<"Printing Model \n";
  for (int i = 0; i<numProcesses; ++i) {
    std::cout<<"Process "<<i<<"\n";
    simProcesses[i].printProcessInfo();
  }
}

void Simulation::checkIfFinished(int num){
  if (jobsComplete == 2 && jobsInSystem == 0) {
    finished = 1;
    std::cout<<"\nSimulation has reached finished state\n";
  }
}

void Simulation::run(int numJobs){
  std::cout<<"Beginning Simulation with "<<numJobs<<" Jobs\n";
  while(!finished){
    processNextEvent();
    checkIfFinished(numJobs);
  }
  return;
}
