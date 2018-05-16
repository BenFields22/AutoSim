//
//  Process.hpp
//  ASAE
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
#include <algorithm>

typedef struct{
  std::string processTime;
  int processPos;
  std::string downStream;
  std::string upStream;
}processInfo;

typedef struct{
  int processID;
  float percentage;
  int capacity;
}downStreamConnection;

typedef struct{
  int processID;
  int bufferIndex;
}upStreamConnection;

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
  
  
  int distType;//defines how the times are generated
  int processType;//where the process is in the line
  float average;
  float minimum;
  float upper;
public:
  Process(){
    jobNum = 1;
  }
  std::vector<Buffer> process_Buffers;
  std::vector<downStreamConnection> downStreamDependencies;
  std::vector<upStreamConnection> upStreamDependencies;
  int getNumUpStreamDependencies();
  int getNumDownStreamDependencies();
  void setProcessID(int id);
  int getProcessType();
  int getBufferIndexToPush();
  void setProcessType(int type);
  void setDistType(int type);
  void setProcessParameters(std::string);
  float getProcessingTimeFromDist();
  void setBufferCapacity(int val,int ind);
  void setUpstreamDependencies(std::string);
  void setDownstreamDependencies(std::string);
  void printProcessInfo();
  void placeEventInBuffer(Event E,int ind);
  Event getEventFromBuffer(int ind);
  int BufferState(int i);
  void printNumInBuffers();
  std::string getJobNum();
  void AddOneJob();
};

#endif /* Process_hpp */
