//
//  Process.cpp
//  ASAE
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
  if(this->distType == TRIANGULAR)std::cout<<"\tDist Type: TRIANGULAR\n";
  else if(this->distType == NORMAL)std::cout<<"\tDist Type: NORMAL\n";
  else if(this->distType == CONSTANT)std::cout<<"\tDist Type: CONSTANT\n";
  
  if(this->processType == FRONT)std::cout<<"\tPos Type: FRONT\n";
  else if(this->processType == TERMINAL)std::cout<<"\tPos Type: TERMINAL\n";
  else std::cout<<"\tPos Type: MIDDLE\n";
  
  std::cout<<"\tUpstream Dependencies: "<<this->upStreamDependencies.size()<<"\n";
  for(int i = 0;i<this->upStreamDependencies.size();++i){
    std::cout<<"\t\tProcessID: "<<upStreamDependencies[i].processID<<"\n";
    std::cout<<"\t\tBufferIndex: "<<upStreamDependencies[i].bufferIndex<<"\n";
  }
  std::cout<<"\tDownStream Dependencies: "<<this->downStreamDependencies.size()<<"\n";
  for(int i = 0;i<this->downStreamDependencies.size();++i){
    std::cout<<"\t\tProcessID: "<<downStreamDependencies[i].processID<<"\n";
    std::cout<<"\t\tPercentage: "<<downStreamDependencies[i].percentage<<"\n";
    std::cout<<"\t\tBuffer Capacity: "<<downStreamDependencies[i].capacity<<"\n";
  }
}

//Description:create the dependencies that will be upstream from a process. limited to 0-9
void Process::setUpstreamDependencies(std::string line){
  int num = std::atoi(line.substr(0,1).c_str());
  int start = 3;
  for(int i = 0;i<num;i++){
    //create each dependency
    upStreamConnection conn;
    conn.processID = std::atoi(line.substr(start,2).c_str());
    conn.bufferIndex = std::atoi(line.substr(start+3,1).c_str());
    upStreamDependencies.push_back(conn);
    start = start + 7;
  }
}

void Process::printNumInBuffers(){
  std::cout<<"Process ID "<<processID<<"\n";
  for(int i = 0;i<process_Buffers.size();++i){
    std::cout<<"Buffer Index "<<i<<" has "<<process_Buffers[i].getNumInQueue()<<" in queue \n";
  }
}


//Description:create the downstream dependencies to control flow. limited to 0-9
void Process::setDownstreamDependencies(std::string line){
  int num = std::atoi(line.substr(0,1).c_str());
  int start = 2;
  float total = 0.0;
  for(int i = 0;i<num;i++){
    //create each dependency
    downStreamConnection conn;
    
    conn.processID = std::atoi(line.substr(start,2).c_str());
    conn.percentage = std::atof(line.substr(start+3,4).c_str());
    total = total + conn.percentage;
    conn.capacity = std::atoi(line.substr(start+8,2).c_str());
    if(conn.capacity<1||conn.capacity>99){
      throw std::runtime_error("\nBUFFER CAPACITY ERROR: Cannot have a capacity less than 1 or greater than 99!\n");
    }
    downStreamDependencies.push_back(conn);
    Buffer buff;
    buff.capacity = conn.capacity;
    process_Buffers.push_back(buff);
    start = start +11;
  }
  
  if(abs(total-1.00) > 0.0001 && num != 0){
    throw std::runtime_error("\nDOWNSTREAM CONNECTION ERROR: Downstream branching percentages must equal 1.00\n");
  }
}

//Description:return a random time for a triangular dist
float getTrianglarDistribution(float a, float b, float c)
{
  float randnum = (float)rand() / (float)RAND_MAX;
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
  return val;
  
}

//Description:bassed on the type of distribution get a random time
float Process::getProcessingTimeFromDist(){
  if (distType == TRIANGULAR) {
    return getTrianglarDistribution(minimum, upper, average);
  }
  else if(distType == NORMAL){
    return distribution(generator);
  }
  else if(distType == UNIFORM){
    return U_distribution(generator);
  }
  else if(distType == CONSTANT){
    return this->constant;
  }
  return 0.0;
}

//Description:set the type of process to indicate position in line
void Process::setProcessType(int type){
  this->processType = type;
}

//Description:standard setter for buffer capacity
void Process::setBufferCapacity(int val,int ind){
  this->process_Buffers[ind].capacity = val;
}


struct greater_than_key
{
  inline bool operator() (const selectionChance& struct1, const selectionChance& struct2)
  {
    return (struct1.percent > struct2.percent);
  }
};

int Process::getBufferIndexToPush(){
  int numdep = (int)downStreamDependencies.size();
  std::vector<selectionChance> options;
  for(int i = 0;i<numdep;i++){
    selectionChance option;
    option.percent = downStreamDependencies[i].percentage;
    option.index = i;
    options.push_back(option);
  }
  std::sort(options.begin(), options.end(),greater_than_key());
  int ans = 0;
  int v2 = (rand() % 101);
  float percentR = (float)(v2)/100.0;
  float totalPercentage = 0.0;
  for(int i = 0;i<numdep;i++){
    totalPercentage = totalPercentage+options[i].percent;
    if(i == 0 && percentR <= totalPercentage){
      //select first
      ans = i;
      break;
    }
    else if(i == numdep-1){
      ans = i;
      break;
    }
    else{
      if(percentR<=totalPercentage){
        ans = i;
        break;
      }
    }
  }
  //std::cout<<"Selected index "<<ans<<" with probability of "<<options[ans].percent<<"\n";
  return ans;
}

//Description: returns index of the process buffer downstream to place into
int Process::getNumDownStreamDependencies(){
  return (int)downStreamDependencies.size();
}

//Description: returns the amount of buffers feeding a process
int Process::getNumUpStreamDependencies(){
  return (int)upStreamDependencies.size();
}

//Description: takes an event and places in the process buffer
void Process::placeEventInBuffer(Event E,int ind){
  process_Buffers[ind].placeInBuffer(E);
}

//Description: takes an event fromt he process buffer
Event Process::getEventFromBuffer(int ind){
  return process_Buffers[ind].GetNext();
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
int Process::BufferState(int i){
  return process_Buffers[i].getState();
}

//Description: set the timing parameters for distribution
void Process::setProcessParameters(std::string info){
  if (this->distType == TRIANGULAR) {
    //setParameters in process for triangular
    info.append(">");
    int done = 0;
    int index = 3;
    int front = 2;
    int num = 1;
    int length = 1;
    while(!done){
      if(info[index]=='>'){
        done =1;
        std::string max = info.substr(front,length);
        upper = atof(max.c_str());
        continue;
      }
      if(info[index]==':'){
        if(num ==1){
          std::string min = info.substr(front,length);
          minimum = atof(min.c_str());
          num++;
          length = 0;
          front = index+1;
          index++;
        }
        else if(num ==2){
          std::string avg = info.substr(front,length);
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
    info.append(">");
    int done = 0;
    int index = 3;
    int front = 2;
    int num = 1;
    int length = 1;
    while(!done){
      if(info[index]=='>'){
        done =1;
        std::string std = info.substr(front,length);
        normalStdDev = atof(std.c_str());
        continue;
      }
      if(info[index]==':'){
        if(num == 1){
          std::string avg = info.substr(front,length);
          normalAverage = std::atof(avg.c_str());
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
    std::normal_distribution<float> mydistribution(normalAverage,normalStdDev);
    distribution = mydistribution;
  }
  else if(this->distType==CONSTANT){
    //set for constant
    //setParameters in process for constant
    info.append(">");
    this->constant = std::atof(info.substr(2,info.length()-2).c_str());
  }
  else if(this->distType==UNIFORM){
    //set for Uniform
    info.append(">");
    int done = 0;
    int index = 3;
    int front = 2;
    int num = 1;
    int length = 1;
    while(!done){
      if(info[index]=='>'){
        done =1;
        std::string std = info.substr(front,length);
        uniformUpper = atof(std.c_str());
        continue;
      }
      if(info[index]==':'){
        if(num == 1){
          std::string avg = info.substr(front,length);
          uniformLower = std::atof(avg.c_str());
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
    std::uniform_real_distribution<float> myUdistribution(uniformLower,uniformUpper);
    U_distribution = myUdistribution;
  }
}
