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

enum EventType{
  WAIT_PULL,
  WAIT_PUSH,
  START,
  FINISH
};

//Description:
class Event{
private:
  int processID;
  int jobID;
  int eventType;
  float processTime;
public:
  Event(int pID, int jID,int eventType,float PTime);
  float getProcessTime();
  void processEvent();
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
