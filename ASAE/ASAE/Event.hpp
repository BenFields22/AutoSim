//
//  Event.hpp
//  ASAE
//
//  Created by Benjamin G Fields on 4/2/18.
//  Copyright © 2018 Benjamin G Fields. All rights reserved.
//
//  Description: describes the structure of the event entity

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
  int previousBuffer;
  int timeAtProcess;
}nextEventInfo;

//Description: class used to contain information important for processing an event in the system
class Event{
private:
  int processID;
  int timesAtCurrentState;
  std::string jobID;
  int eventType;
  float processTime;
public:
  int previousBuffer;
  Event(int pID, std::string jID,int eventType,float PTime,int numAtCurrentState,int previousBuffer = -1);
  float getProcessTime();
  int getProcessID();
  std::string getJobID();
  void printEvent();
  int getEventType();
  int getTimesAtCurrentState();
};

//Description: utility class used to compare the simtime for events when placing into the priority queue
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
