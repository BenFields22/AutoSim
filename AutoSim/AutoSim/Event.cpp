//
//  Event.cpp
//  AutoSim
//
//  Created by Benjamin G Fields on 4/2/18.
//  Copyright © 2018 Benjamin G Fields. All rights reserved.
//
//  Description: defines how the create an event and get the needed information

#include "Event.hpp"

//Description:constructor for an event to be processed in simulation
Event::Event(int pID, std::string jID,int eventType,float pTime){
  this->processID = pID;
  this->jobID = jID;
  this->eventType = eventType;
  this->processTime = pTime;
}

//Description:get the type of event (PUSH,PULL,START, FINISH)
int Event::getEventType(){
  return eventType;
}

//Description: get the simtime that the event was scheduled with
float Event::getProcessTime(){
  return processTime;
}

//Description: get the job id assiciated with the current event
std::string Event::getJobID(){
  return jobID;
}

//Description:get the process id associated with the current event
int Event::getProcessID(){
  return processID;
}

//Description:utility function to print the details of the event to the console for debug
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
