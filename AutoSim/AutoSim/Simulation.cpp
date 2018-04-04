//
//  Simulation.cpp
//  AutoSim
//
//  Created by Benjamin G Fields on 4/2/18.
//  Copyright © 2018 Benjamin G Fields. All rights reserved.
//
//  Description:

#include "Simulation.hpp"

//Description:
Simulation::Simulation(){
  simTime = 0.0;
  jobsComplete = 0;
  jobsInSystem = 0;
  finished = 0;
  createJobID = 1;
  startRecordRow = 1;
  FinishRecordRow = 1;
  timeStep = 0.002;
  workbook  = workbook_new("EventLog.xlsx");
  worksheet = workbook_add_worksheet(workbook, NULL);
  worksheet_write_string(worksheet, 0, 0,"JobID Start", NULL);
  worksheet_write_string(worksheet, 0, 1,"StartTime", NULL);
  worksheet_write_string(worksheet, 0, 2,"Resource", NULL);
  worksheet_write_string(worksheet, 0, 4,"JobID END", NULL);
  worksheet_write_string(worksheet, 0, 5,"ExitTime", NULL);
}

//Description:
Simulation::~Simulation(){
  workbook_close(workbook);
  std::cout<<"Terminating simulation\n";
}

//Description:
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
    else
      simProcesses[i].setBufferCapacity(-1);
    simProcesses[i].setProcessType(processes[i].processPos);
    simProcesses[i].setUpstreamDependencies(processes[i].upStream);
    simProcesses[i].setDownstreamDependencies(processes[i].downStream);
    simProcesses[i].setProcessID(i);
  }
  return 0;
}

//Description:
void Simulation::init(){
  for (int i = 0; i< numProcesses; ++i) {
    if (simProcesses[i].getProcessType() == FRONT) {
      std::string id = simProcesses[i].getJobNum();
      id.append(":");
      id.append(std::to_string(i));
      id.append("-");
      Event E(i,id,START,simTime);
      eventQueue.push(E);
      jobsInSystem++;
    }
    else{
      //schedule pull to start cycle
      Event E(i,"-1",PULL_BUFFER,simTime);
      eventQueue.push(E);
    }
  }
}

//Description:
int Simulation::getFeedBufferState(Process proc){
  //check for pull
  if (proc.getNumUpStreamDependencies()==0) {
    return -1;
  }
  int state = EMPTY;
  for (int i = 0; i<proc.getNumUpStreamDependencies(); ++i) {
    int pos = proc.upStreamDependencies[i];
    int buffState = simProcesses[pos].BufferState();
    if (buffState == EMPTY) {
      state = EMPTY;
      break;
    }
    else{
      state = CAN_PULL;
      continue;
    }
  }
  return state;
}

//Description:
nextEventInfo Simulation::processCurrentEvent(Event currentEvent, int currentProcess){
  std::cout<<"Process Type: "<<simProcesses[currentProcess].getProcessType()<<std::endl;
  currentEvent.printEvent();
  nextEventInfo info;
  std::string pid = std::to_string(currentProcess);
  std::string jid = currentEvent.getJobID();
  std::string resource = "worker "+pid;
  int FeedBufferState = getFeedBufferState(simProcesses[currentProcess]);
  if (FeedBufferState == EMPTY) {
    std::cout<<"FeedBufferState: EMPTY\n";
  }
  if (FeedBufferState == FULL) {
    std::cout<<"FeedBufferState: FULL\n";
  }
  if (FeedBufferState == CAN_PULL) {
    std::cout<<"FeedBufferState: CAN_PULL\n";
  }
  if (FeedBufferState == -1) {
    std::cout<<"FeedBufferState: NO BUFFER\n";
  }
  int currentBufferState = simProcesses[currentProcess].BufferState();
  if (currentBufferState == EMPTY) {
    std::cout<<"currentBufferState: EMPTY\n";
  }
  if (currentBufferState == FULL) {
    std::cout<<"currentBufferState: FULL\n";
  }
  if (currentBufferState == SPACE_LEFT) {
    std::cout<<"currentBufferState: SPACE_LEFT\n";
  }
  if (currentBufferState == -1) {
    std::cout<<"currentBufferState: NO BUFFER\n";
  }
  if (currentEvent.getEventType() == PULL_BUFFER) {
    //try to pull
    
    //if there is a job then pull
    if (FeedBufferState == CAN_PULL) {
      //pull job from each buffer upstream and create new start with compound id
      std::string cid;
      for (int i =0; i<simProcesses[currentProcess].getNumUpStreamDependencies(); ++i) {
        int depend = simProcesses[currentProcess].upStreamDependencies[i];
        Event E = simProcesses[depend].getEventFromBuffer();
        cid.append(E.getJobID());
      }

      std::string id = simProcesses[currentProcess].getJobNum();
      id.append(":");
      id.append(std::to_string(currentProcess));
      id.append("-");
      id.append(cid);
      info.eventType = START;
      info.jobID = id;
      info.NextProcess = currentProcess;
      info.nextTime = simTime;
      info.triggerEventType = PULL_BUFFER;
    }
    else{
      //if no jobs then schedule another pull
      info.eventType = PULL_BUFFER;
      info.jobID = currentEvent.getJobID();
      info.NextProcess = currentProcess;
      info.triggerEventType = PULL_BUFFER;
      info.nextTime = simTime + timeStep;
    }
  }
  else if(currentEvent.getEventType() == PUSH_BUFFER){
    //try to push into buffer or wait if full
    
    //check if full
    if (currentBufferState != FULL) {
      //if not full then place into buffer and schedule pull unless at beginning you schedule a start
      
      //push on buffer
      simProcesses[currentProcess].placeEventInBuffer(currentEvent);
      if (simProcesses[currentProcess].getProcessType()==FRONT) {
        //schedule a start
        jobsInSystem++;
        info.eventType = START;
        info.NextProcess = currentProcess;
        info.nextTime = simTime;
        std::string id = simProcesses[currentProcess].getJobNum();
        id.append(":");
        id.append(std::to_string(currentProcess));
        id.append("-");
        info.jobID = id;
        info.triggerEventType = PUSH_BUFFER;
      }
      else{
        //schedule a pull
        info.eventType = PULL_BUFFER;
        info.NextProcess = currentProcess;
        info.nextTime = simTime;
        info.jobID = "-1";
        info.triggerEventType = PUSH_BUFFER;
      }
    }
    else{
      //if full then schedule another push
      info.eventType = PUSH_BUFFER;
      info.jobID = jid;
      info.NextProcess = currentProcess;
      info.nextTime = simTime+timeStep;
      info.triggerEventType = PUSH_BUFFER;
    }
  }
  else if(currentEvent.getEventType() == START){
    //schedule finish event
    worksheet_write_string(worksheet, startRecordRow, 0,jid.c_str(), NULL);
    worksheet_write_string(worksheet, startRecordRow, 1,std::to_string(currentEvent.getProcessTime()).c_str(), NULL);
    worksheet_write_string(worksheet, startRecordRow, 2,resource.c_str(), NULL);
    startRecordRow++;
    
    info.eventType = FINISH;
    info.jobID = currentEvent.getJobID();
    info.NextProcess = currentProcess;
    info.nextTime = simTime+simProcesses[currentProcess].getProcessingTimeFromDist();
    info.triggerEventType = START;
    
  }
  else if(currentEvent.getEventType() == FINISH){
    //try to push onto buffer or wait or nothing if terminal
    worksheet_write_string(worksheet, FinishRecordRow, 4,jid.c_str(), NULL);
    worksheet_write_string(worksheet, FinishRecordRow, 5,std::to_string(currentEvent.getProcessTime()).c_str(), NULL);
    FinishRecordRow++;
    simProcesses[currentProcess].AddOneJob();
    if (simProcesses[currentProcess].getProcessType()==TERMINAL) {
      jobsComplete++;
      jobsInSystem--;
      //check if at end of line if so then just assign -1 and return
      //schedule a pull
      info.eventType = PULL_BUFFER;
      info.jobID = "-1";
      info.NextProcess = currentProcess;
      info.nextTime = simTime;
      info.triggerEventType = FINISH;
    }
    else{
      //if not terminal need to try to schdule push
      info.eventType = PUSH_BUFFER;
      info.jobID = jid;
      info.NextProcess = currentProcess;
      info.nextTime = simTime;
      info.triggerEventType = FINISH;
    }
  }
  //return -1 -1 if no event is to be scheduled
  return info;
}

//Description:
void Simulation::processNextEvent(){
  std::cout<<"\n**********PROCESSING EVENT***********\n";
  int size = (int)eventQueue.size();
  std::cout<<"Queue Size is "<<size<<"\n";
  std::cout<<"Jobs in system: "<<jobsInSystem<<std::endl;
  std::cout<<"Jobs complete: "<<jobsComplete<<std::endl;
  
  Event currentEvent = eventQueue.top();
  eventQueue.pop();
  int currentProcess = currentEvent.getProcessID();
  simTime = currentEvent.getProcessTime();
  nextEventInfo next = processCurrentEvent(currentEvent,currentProcess);
  
  if (next.NextProcess != -1) {
    Event next_E(next.NextProcess,next.jobID,next.eventType,next.nextTime);
    eventQueue.push(next_E);
  }
  //jobsComplete++;
  //jobsInSystem--;
}

//Description:
void Simulation::printModel(){
  std::cout<<"Printing Model \n";
  for (int i = 0; i<numProcesses; ++i) {
    std::cout<<"Process "<<i<<"\n";
    simProcesses[i].printProcessInfo();
  }
}

//Description:
void Simulation::checkIfFinished(int num){
  if (jobsComplete == num) {
    finished = 1;
    std::cout<<"\nSimulation has reached finished state\n";
  }
}

//Description:
void Simulation::run(int numJobs){
  std::cout<<"Beginning Simulation with "<<numJobs<<" Jobs\n";
  //int count = 0;
  while(!finished){
    processNextEvent();
    checkIfFinished(numJobs);
    //std::cin.get();
    /*count++;
    if (count == 4) {
      finished = 1;
    }*/
  }
  return;
}
