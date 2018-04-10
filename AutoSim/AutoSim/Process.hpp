//
//  Process.hpp
//  AutoSim
//
//  Created by Benjamin G Fields on 4/2/18.
//  Copyright © 2018 Benjamin G Fields. All rights reserved.
//
//  Description: defines the structure of the process object

#ifndef Process_hpp
#define Process_hpp

#include <stdio.h>
#include <iostream>
#include "Buffer.hpp"
#include <string>
#include <cmath>

typedef struct{
  std::string processTime;
  std::string bufferCapacity;
  int processPos;
  std::string downStream;
  std::string upStream;
}processInfo;

enum Dist{
  TRIANGULAR,
  NORMAL,
  UNIFORM,
  CONSTANT
};

enum ProcessType{
  FRONT,
  MIDDLE,
  TERMINAL
};

//Description: class that stores info pertaining to a process in the system
class Process{
private:
  int jobNum;
  int processID;
  Buffer process_Buffer;
  
  int downStreamDependencies;//array of indices in process array
  int distType;//defines how the times are generated
  int processType;//where the process is in the line
  float average;
  float minimum;
  float upper;
public:
  Process(){
    jobNum = 1;
  }
  std::vector<int> upStreamDependencies;//array of indices in process array
  int getNumUpStreamDependencies();
  int getDownStreamProcess();
  void setProcessID(int id);
  int getProcessType();
  void setProcessType(int type);
  void setDistType(int type);
  void setProcessParameters(std::string);
  float getProcessingTimeFromDist();
  void setBufferCapacity(int val);
  void setUpstreamDependencies(std::string);
  void setDownstreamDependencies(std::string);
  void printProcessInfo();
  void placeEventInBuffer(Event E);
  Event getEventFromBuffer();
  int BufferState();
  std::string getJobNum();
  void AddOneJob();
};

#endif /* Process_hpp */
