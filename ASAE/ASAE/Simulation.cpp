//
//  Simulation.cpp
//  ASAE
//
//  Created by Benjamin G Fields on 4/2/18.
//  Copyright © 2018 Benjamin G Fields. All rights reserved.
//
//  Description: implementation of the simulation class

#include "Simulation.hpp"

//Description:constructor to create the simulation object
Simulation::Simulation(){
  simTime = 0.0;
  startFile.open("starts.txt", std::ios::out);
  finishFile.open("Finish.txt", std::ios::out);
  resultsFile.open("Results.txt",std::ios::out);
  if(!startFile.is_open()||!finishFile.is_open()){
    throw std::runtime_error("ERROR: Could not open Start and Finish log files!");
  }
  jobsComplete = 0;
  jobsInSystem = 0;
  finished = 0;
  createJobID = 1;
  startRecordRow = 1;
  FinishRecordRow = 1;
  timeStep = 0.001;
  workbook  = workbook_new("EventLog.xlsx");
  worksheet = workbook_add_worksheet(workbook, NULL);
  worksheet_write_string(worksheet, 0, 0,"JobID Start", NULL);
  worksheet_write_string(worksheet, 0, 1,"StartTime", NULL);
  worksheet_write_string(worksheet, 0, 2,"JobsInSystem", NULL);
  startFile<<"JobID Start,StartTime,Resource,JobsInSystem\n";
  worksheet_write_string(worksheet, 0, 4,"JobID END", NULL);
  worksheet_write_string(worksheet, 0, 5,"ExitTime", NULL);
  worksheet_write_string(worksheet, 0, 6,"JobsInSystem", NULL);
  finishFile<<"JobID END,ExitTime,JobsInSystem\n";
}

//Description:destructor to save and close excel workbook
Simulation::~Simulation(){
  workbook_close(workbook);
  startFile.close();
  finishFile.close();
  std::cout<<"Terminating simulation\n";
}

//Description:takes the process info from the model file and creates the simulation structure
int Simulation::constructModel(std::vector<processInfo> &processes){
  std::cout<<"Constructing the simulation model\n";
  numProcesses = (int)processes.size();
  simProcesses = new Process[numProcesses];
  for (int i = 0; i<numProcesses; ++i) {
    if(processes[i].processTime[0] == 'T'){
      if(this->debug)std::cout<<"Process "<<i<< " is Triangular \n";
      simProcesses[i].setDistType(TRIANGULAR);
    }
    else if(processes[i].processTime[0] == 'N'){
      if(this->debug)std::cout<<"Process "<<i<< " is Normal \n";
      simProcesses[i].setDistType(NORMAL);
    }
    else if(processes[i].processTime[0] == 'U'){
      if(this->debug)std::cout<<"Process "<<i<< " is Uniform \n";
      simProcesses[i].setDistType(UNIFORM);
    }
    simProcesses[i].setProcessParameters(processes[i].processTime);
    simProcesses[i].setProcessType(processes[i].processPos);
    simProcesses[i].setUpstreamDependencies(processes[i].upStream);
    simProcesses[i].setDownstreamDependencies(processes[i].downStream);
    simProcesses[i].setProcessID(i);
  }
  return 0;
}

//Description:initializes the simulation with start jobs and begins each process to wait for a job to arrive
void Simulation::init(){
  for (int i = 0; i< numProcesses; ++i) {
    if (simProcesses[i].getProcessType() == FRONT) {
      std::string id = "[" + simProcesses[i].getJobNum();
      id.append(":");
      id.append(std::to_string(i));
      id.append("-(x)]");
      Event E(i,id,START,simTime);
      eventQueue.push(E);
    }
    else{
      //schedule pull to start cycle
      Event E(i,"-1",PULL_BUFFER,simTime);
      eventQueue.push(E);
    }
  }
}

//Description:gets the state of the upstream buffers that feed a process
int Simulation::getFeedBufferState(Process proc){
  //check for pull
  if (proc.getNumUpStreamDependencies()==0) {
    return -1;
  }
  int state = EMPTY;
  for (int i = 0; i<proc.getNumUpStreamDependencies(); ++i) {
    int pos = proc.upStreamDependencies[i].processID;
    int buffIndex = proc.upStreamDependencies[i].bufferIndex;
    int buffState = simProcesses[pos].BufferState(buffIndex);
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

//Description: recursive helper function to determine how many components are leaving system
int Simulation::getNumberOfEnterPoints(int processID){
  if(simProcesses[processID].getProcessType()==FRONT){
    return 1;
  }
  int num = 0;
  for(int i = 0;i<simProcesses[processID].getNumUpStreamDependencies();++i){
    int parent = simProcesses[processID].upStreamDependencies[i].processID;
    num = num + getNumberOfEnterPoints(parent);
  }
  return num;
}

//Description: return the number of components the part is composed of
int Simulation::getNumComponents(int current){
  //need to determine how many components are in the current job
  return getNumberOfEnterPoints(current);
}


//Description:process the event and then create the info for the next event to be scheduled
nextEventInfo Simulation::processCurrentEvent(Event currentEvent, int currentProcess){
  if(this->debug) std::cout<<"Process Type: "<<simProcesses[currentProcess].getProcessType()<<std::endl;
  if(this->debug) currentEvent.printEvent();
  nextEventInfo info;
  std::string pid = std::to_string(currentProcess);
  std::string jid = currentEvent.getJobID();
  std::string resource = "worker "+pid;
  int FeedBufferState = getFeedBufferState(simProcesses[currentProcess]);
  if (this->debug) {
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
  }
    
    //select the buffer to place into if need to push
    int BuffertoPush;
    if(currentEvent.previousBuffer != -1)
    {
      BuffertoPush = currentEvent.previousBuffer;
    }
    else{
      if(simProcesses[currentProcess].getNumDownStreamDependencies()<2){
        BuffertoPush = 0;
        currentEvent.previousBuffer = 0;
      }
      else{
        BuffertoPush = simProcesses[currentProcess].getBufferIndexToPush();
        currentEvent.previousBuffer = BuffertoPush;
      }
    }
    int currentBufferState;
    if(simProcesses[currentProcess].getNumDownStreamDependencies()==0){
      currentBufferState = -1;
    }
    else{
      currentBufferState = simProcesses[currentProcess].BufferState(BuffertoPush);
    }
  
    if(this->debug){
      if (currentBufferState == EMPTY) {
        std::cout<<"pushBufferState: EMPTY\n";
      }
      if (currentBufferState == FULL) {
        std::cout<<"pushBufferState: FULL\n";
      }
      if (currentBufferState == SPACE_LEFT) {
        std::cout<<"pushBufferState: SPACE_LEFT\n";
      }
      if (currentBufferState == -1) {
        std::cout<<"pushBufferState: NO BUFFER\n";
      }
    }
  
  if (currentEvent.getEventType() == PULL_BUFFER) {
    //try to pull
    
    //if there is a job then pull
    if (FeedBufferState == CAN_PULL) {
      //pull job from each buffer upstream and create new start with compound id
      std::string cid = "(";
      for (int i =0; i<simProcesses[currentProcess].getNumUpStreamDependencies(); ++i) {
        int depend = simProcesses[currentProcess].upStreamDependencies[i].processID;
        int buff = simProcesses[currentProcess].upStreamDependencies[i].bufferIndex;
        Event E = simProcesses[depend].getEventFromBuffer(buff);
        cid.append(E.getJobID());
      }
      cid.append(")");
      std::string id = "[" + simProcesses[currentProcess].getJobNum();
      id.append(":");
      id.append(std::to_string(currentProcess));
      id.append("-");
      id.append(cid);
      id.append("]");
      
      info.eventType = START;
      info.jobID = id;
      info.NextProcess = currentProcess;
      info.nextTime = simTime;
      info.triggerEventType = PULL_BUFFER;
      info.previousBuffer = -1;
    }
    else{
      //if no jobs then schedule another pull
      info.eventType = PULL_BUFFER;
      info.jobID = currentEvent.getJobID();
      info.NextProcess = currentProcess;
      info.triggerEventType = PULL_BUFFER;
      info.nextTime = simTime + timeStep;
      info.previousBuffer = -1;
    }
  }
  else if(currentEvent.getEventType() == PUSH_BUFFER){
    //try to push into buffer or wait if full
    
    //check if full
    if (currentBufferState != FULL) {
      //if not full then place into buffer and schedule pull unless at beginning you schedule a start
      
      //push on buffer
      currentEvent.previousBuffer = -1;
      simProcesses[currentProcess].placeEventInBuffer(currentEvent,BuffertoPush);
      if (simProcesses[currentProcess].getProcessType()==FRONT) {
        //schedule a start
        info.eventType = START;
        info.NextProcess = currentProcess;
        info.nextTime = simTime;
        std::string id = "[" + simProcesses[currentProcess].getJobNum();
        id.append(":");
        id.append(std::to_string(currentProcess));
        id.append("-(x)]");
        info.jobID = id;
        info.triggerEventType = PUSH_BUFFER;
        info.previousBuffer = -1;
      }
      else{
        //schedule a pull
        info.eventType = PULL_BUFFER;
        info.NextProcess = currentProcess;
        info.nextTime = simTime;
        info.jobID = "-1";
        info.triggerEventType = PUSH_BUFFER;
        info.previousBuffer = -1;
      }
    }
    else{
      //if full then schedule another push
      info.eventType = PUSH_BUFFER;
      info.jobID = jid;
      info.NextProcess = currentProcess;
      info.nextTime = simTime+timeStep;
      info.triggerEventType = PUSH_BUFFER;
      info.previousBuffer = BuffertoPush;
    }
  }
  else if(currentEvent.getEventType() == START){
    if(simProcesses[currentProcess].getProcessType()==FRONT){
      jobsInSystem++;
    }
    //schedule finish event
    worksheet_write_string(worksheet, startRecordRow, 0,jid.c_str(), NULL);
    worksheet_write_string(worksheet, startRecordRow, 1,std::to_string(currentEvent.getProcessTime()).c_str(), NULL);
    worksheet_write_string(worksheet, startRecordRow, 2,std::to_string(jobsInSystem).c_str(), NULL);
    startFile<<jid.c_str()<<","
              <<std::to_string(currentEvent.getProcessTime()).c_str()<<","
              <<std::to_string(jobsInSystem).c_str()<<"\n";
    startRecordRow++;
    
    info.eventType = FINISH;
    info.jobID = currentEvent.getJobID();
    info.NextProcess = currentProcess;
    info.nextTime = simTime+simProcesses[currentProcess].getProcessingTimeFromDist();
    info.triggerEventType = START;
    info.previousBuffer = -1;
    
  }
  else if(currentEvent.getEventType() == FINISH){
    //try to push onto buffer or wait or nothing if terminal
    worksheet_write_string(worksheet, FinishRecordRow, 4,jid.c_str(), NULL);
    worksheet_write_string(worksheet, FinishRecordRow, 5,std::to_string(currentEvent.getProcessTime()).c_str(), NULL);
    finishFile<<jid.c_str()<<","
    <<std::to_string(currentEvent.getProcessTime()).c_str()<<",";
    
    simProcesses[currentProcess].AddOneJob();
    if (simProcesses[currentProcess].getProcessType()==TERMINAL) {
      int numComponents = getNumComponents(currentProcess);
      //std::cout<<"Count of entrys is "<<numComponents<<"\n";
      jobsInSystem = jobsInSystem-numComponents;
      jobsComplete++;
      std::cout<<"\tJobs Complete: "<<jobsComplete<<"\n";
      std::cout<<"\tJobs in System: "<<jobsInSystem<<"\n";
      //check if at end of line if so then just assign -1 and return
      //schedule a pull
      info.eventType = PULL_BUFFER;
      info.jobID = "-1";
      info.NextProcess = currentProcess;
      info.nextTime = simTime;
      info.triggerEventType = FINISH;
      info.previousBuffer = -1;
    }
    else{
      //if not terminal need to try to schedule push
      info.eventType = PUSH_BUFFER;
      info.jobID = jid;
      info.NextProcess = currentProcess;
      info.nextTime = simTime;
      info.triggerEventType = FINISH;
      info.previousBuffer = BuffertoPush;
    }
    //write how many jobs are in the system
    worksheet_write_string(worksheet, FinishRecordRow, 6,std::to_string(jobsInSystem).c_str(), NULL);
    finishFile<<std::to_string(jobsInSystem).c_str()<<"\n";
    FinishRecordRow++;
  }
  
  //return -1 -1 if no event is to be scheduled
  return info;
}

//Description:function that takes the next job in the queue and processes the event
void Simulation::processNextEvent(){
  if(this->debug) std::cout<<"\n**********PROCESSING EVENT***********\n";
  int size = (int)eventQueue.size();
  if(this->debug) std::cout<<"Queue Size is "<<size<<"\n";
  if(this->debug) std::cout<<"Jobs in system: "<<jobsInSystem<<std::endl;
  if(this->debug) std::cout<<"Jobs complete: "<<jobsComplete<<std::endl;
  
  Event currentEvent = eventQueue.top();
  eventQueue.pop();
  int currentProcess = currentEvent.getProcessID();
  simTime = currentEvent.getProcessTime();
  nextEventInfo next = processCurrentEvent(currentEvent,currentProcess);
  
  if (next.NextProcess != -1) {
    Event next_E(next.NextProcess,next.jobID,next.eventType,next.nextTime,next.previousBuffer);
    eventQueue.push(next_E);
  }
}

//Description:utility function to print out the simulation model as created in the simulation
void Simulation::printModel(){
  std::cout<<"Printing Model \n";
  for (int i = 0; i<numProcesses; ++i) {
    std::cout<<"Process "<<i<<"\n";
    simProcesses[i].printProcessInfo();
  }
}

//Description:checks to see if the simulation has reached the terminated conditions
void Simulation::checkIfFinished(int num){
  if (jobsComplete == num) {
    finished = 1;
    std::cout<<"\n****************DONE*****************\n";
    std::cout<<"Simulation has reached finished state\n";
  }
  if (jobsComplete > num) {
    throw std::runtime_error("ERROR: Exceeded the number of jobs complete!");
  }
}

//Description:launch the simulaiton for a certain number of jobs
void Simulation::run(int numJobs, int verbose){
  this->debug = verbose;
  std::cout<<"\nBeginning Simulation with "<<numJobs<<" Jobs\n";
  while(!finished){
    processNextEvent();
    checkIfFinished(numJobs);
  }
  return;
}


