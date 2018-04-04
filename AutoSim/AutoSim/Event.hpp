//
//  Event.hpp
//  AutoSim
//
//  Created by Benjamin G Fields on 4/2/18.
//  Copyright © 2018 Benjamin G Fields. All rights reserved.
//
//  Description: 

#ifndef Event_hpp
#define Event_hpp

#include <stdio.h>
#include <iostream>
#include "xlsxwriter.h"


enum EventType{
  PULL_BUFFER,
  PUSH_BUFFER,
  START,
  FINISH
};

typedef struct{
  int triggerEventType;
  int NextProcess;
  int eventType;
  std::string jobID;
  float nextTime;
}nextEventInfo;

//Description:
class Event{
private:
  int processID;
  std::string jobID;
  int eventType;
  float processTime;
public:
  Event(int pID, std::string jID,int eventType,float PTime);
  float getProcessTime();
  int getProcessID();
  std::string getJobID();
  void printEvent();
  int getEventType();
};

//Description:
class Compare {
public:
  bool operator()(Event &a, Event &b)
  {
    if (a.getProcessTime() > b.getProcessTime())
    {
      return true;
    }
    else
    {
      return false;
    }
  }
};

#endif /* Event_hpp */
