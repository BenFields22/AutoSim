//
//  Event.cpp
//  AutoSim
//
//  Created by Benjamin G Fields on 4/2/18.
//  Copyright © 2018 Benjamin G Fields. All rights reserved.
//
//  Description: 

#include "Event.hpp"

Event::Event(int pID, int jID,int eventType,float pTime){
  this->processID = pID;
  this->jobID = jID;
  this->eventType = eventType;
  this->processTime = pTime;
}

int Event::getEventType(){
  return eventType;
}

float Event::getProcessTime(){
  return processTime;
}

void Event::processEvent(){
  printEvent();
}

void Event::printEvent(){
  std::cout<<"********Printing Event*********\n";
  std::cout<<"PID: "<<processID<<"\n";
  std::cout<<"JOB_ID: "<<jobID<<"\n";
  std::cout<<"EventType: "<<eventType<<"\n";
  std::cout<<"Process Time: "<<processTime<<"\n";
}
