//
//  Process.cpp
//  AutoSim
//
//  Created by Benjamin G Fields on 4/2/18.
//  Copyright © 2018 Benjamin G Fields. All rights reserved.
//
//  Description: Implementation of the process object

#include "Process.hpp"

//Description:defines what type of distribution a process adheres to (triangular,normal, uniform)
void Process::setDistType(int type){
  this->distType = type;
}

//Description: standard function to set the process ID
void Process::setProcessID(int id){
  this->processID = id;
}

//Description: standard function to return the type of process indicating where in the line it is
int Process::getProcessType(){
  return this->processType;
}

//Description:prints the parameters of the current process
void Process::printProcessInfo(){
  std::cout<<"\tProcess ID: "<<this->processID<<"\n";
  std::cout<<"\tDist Type: "<<this->distType<<"\n";
  std::cout<<"\tPos Type: "<<this->processType<<"\n";
  std::cout<<"\tBuffer Cap: "<<this->process_Buffer.capacity<<"\n";
  std::cout<<"\tDownStream Dependency: "<< this->downStreamDependencies<<"\n";
  std::cout<<"\tUpStream Dependencies: ";
  for(int i = 0;i<this->upStreamDependencies.size();++i){
    std::cout<<std::to_string(upStreamDependencies[i]) + " ";
  }
  std::cout<<"\n";
  
}

//Description:create the dependencies that will be upstream from a process
void Process::setUpstreamDependencies(std::string line){
  std::cout<<"\tSetting up upstream dependencies with string "<<line<<"\n";
  int num = atoi(line.substr(0,1).c_str());
  int index = 2;
  for (int i = 0; i<num; ++i) {
    int val = atoi(line.substr(index,1).c_str());
    upStreamDependencies.push_back(val);
    index = index +2;
  }
  std::cout<<"\tDependencies: ";
  for (int i = 0; i<upStreamDependencies.size(); ++i) {
    std::cout<<upStreamDependencies[i]<<" ";
  }
  std::cout<<"\n";
}

//Description:create the downstream dependencies to control flow
void Process::setDownstreamDependencies(std::string line){
  std::cout<<"\tSetting up downstream dependencies with string "<<line<<"\n";
  if (line == "X") {
    downStreamDependencies = -1;
    return;
  }
  int num = atoi(line.substr(0,1).c_str());
  downStreamDependencies = num;
  //TODO: Need to account for multiple output buffers
}

//Description:return a random time for a triangular dist
float getTrianglarDistribution(float a, float b, float c)
{
  //std::cout << "Generating a random number in the triangular distribution with low " << a << " and high " << b << " and mean of " << c << std::endl;
  float randnum = (float)rand() / (float)RAND_MAX;
  //std::cout << "Rand num: " << randnum << std::endl;
  float fc = (c - a) / (b - a);
  float val;
  if (randnum < fc && fc > 0.0)
  {
    val = a + sqrt(randnum*(b - a) / (c - a));
  }
  else
  {
    val = b - sqrt((1.0 - randnum)*(b - a) / (c - a));
  }
  //std::cout << "Value is " << val << std::endl;
  return val;
  
}

//Description:bassed on the type of distribution get a random time
float Process::getProcessingTimeFromDist(){
  if (distType == TRIANGULAR) {
    return getTrianglarDistribution(minimum, upper, average);
  }
  else if(distType == NORMAL){
    //TODO
  }
  else if(distType == UNIFORM){
    //TODO
  }
  return 0.0;
}

//Description:set the type of process to indicate position in line
void Process::setProcessType(int type){
  this->processType = type;
  std::cout<<"\tSetting process type to "<<type<<"\n";
}

//Description:standard setter for buffer capacity
void Process::setBufferCapacity(int val){
  std::cout<<"\tSetting buffer capacity of "<<val<<"\n";
  this->process_Buffer.capacity = val;
}

//Description: returns index of the process downstream
int Process::getDownStreamProcess(){
  return downStreamDependencies;
}

//Description: returns the amount of buffers feeding a process
int Process::getNumUpStreamDependencies(){
  return (int)upStreamDependencies.size();
}

//Description: takes an event and places in the process buffer
void Process::placeEventInBuffer(Event E){
  process_Buffer.placeInBuffer(E);
}

//Description: takes an event fromt he process buffer
Event Process::getEventFromBuffer(){
  return process_Buffer.GetNext();
}

//Description: add one to the jobs complete parameter
void Process::AddOneJob(){
  jobNum++;
}

//Description: get the job number for the process
std::string Process::getJobNum(){
  return std::to_string(jobNum);
}

//Description: return the state of the process buffer (full,empty, space left)
int Process::BufferState(){
  return process_Buffer.getState();
}

//Description: set the timing parameters for distribution
void Process::setProcessParameters(std::string info){
  if (this->distType == TRIANGULAR) {
    //setParameters in process for triangular
    info.append(">");
    std::cout<<"Setting up triangular with string "<<info<< "\n";
    int done = 0;
    int index = 3;
    int front = 2;
    int num = 1;
    int length = 1;
    while(!done){
      if(info[index]=='>'){
        done =1;
        std::string max = info.substr(front,length);
        std::cout<<"\tMax:"<<max<<"\n";
        upper = atof(max.c_str());
        continue;
      }
      if(info[index]==':'){
        if(num ==1){
          std::string min = info.substr(front,length);
          std::cout<<"\tMinimum:"<<min<<"\n";
          minimum = atof(min.c_str());
          num++;
          length = 0;
          front = index+1;
          index++;
        }
        else if(num ==2){
          std::string avg = info.substr(front,length);
          std::cout<<"\tAverage:"<<avg<<"\n";
          average = atof(avg.c_str());
          num++;
          length = 0;
          front = index+1;
          index++;
        }
      }
      else{
        length++;
        index++;
      }
    }
  }
  else if(this->distType==NORMAL){
    //set for normal
    //TODO
  }
  else if(this->distType==UNIFORM){
    //set for uniform
    //TODO
  }
}
