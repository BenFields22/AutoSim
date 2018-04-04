//
//  Event.cpp
//  AutoSim
//
//  Created by Benjamin G Fields on 4/2/18.
//  Copyright © 2018 Benjamin G Fields. All rights reserved.
//
//  Description: 

#include "Event.hpp"

//Description:
Event::Event(int pID, std::string jID,int eventType,float pTime){
  this->processID = pID;
  this->jobID = jID;
  this->eventType = eventType;
  this->processTime = pTime;
}

//Description:
int Event::getEventType(){
  return eventType;
}

//Description:
float Event::getProcessTime(){
  return processTime;
}

//Description:
std::string Event::getJobID(){
  return jobID;
}

//Description:
int Event::getProcessID(){
  return processID;
}

//Description:
void Event::printEvent(){
  std::cout<<"********Printing Event*********\n";
  std::cout<<"PID: "<<processID<<"\n";
  std::cout<<"JOB_ID: "<<jobID<<"\n";
  if (eventType == PUSH_BUFFER) {
    std::cout<<"EventType: PUSH_BUFFER \n";
  }
  if (eventType == PULL_BUFFER) {
    std::cout<<"EventType: PULL_BUFFER \n";
  }
  if (eventType == START) {
    std::cout<<"EventType: START \n";
  }
  if (eventType == FINISH) {
    std::cout<<"EventType: FINISH \n";
  }
  std::cout<<"Process Time: "<<processTime<<"\n";
}
