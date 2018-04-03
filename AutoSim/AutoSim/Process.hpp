//
//  Process.hpp
//  AutoSim
//
//  Created by Benjamin G Fields on 4/2/18.
//  Copyright © 2018 Benjamin G Fields. All rights reserved.
//
//  Description: 

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

//Description:
class Process{
private:
  int processID;
  Buffer process_Buffer;
  std::vector<int> upStreamDependencies;//array of indices in process array
  std::vector<int> downStreamDependencies;//array of indices in process array
  int distType;//defines how the times are generated
  int processType;//where the process is in the line
  float average;
  float minimum;
  float upper;
public:
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
};

#endif /* Process_hpp */
