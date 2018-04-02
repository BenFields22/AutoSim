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
#include "Buffer.hpp"
#include <string>

typedef struct{
  std::string processTime;
  int bufferCapacity;
  int processPos;
  int downStream;
  int upStream;
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
  Buffer process_Buffer;
  int* upStreamDependencies;//array of indices in process array
  int* downStreamDependencies;//array of indices in process array
  int distType;//defines how the times are generated
  int processType;//where the process is in the line
public:
  float getProcessingTimeFromDist();
};

#endif /* Process_hpp */
