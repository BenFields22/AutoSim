//
//  DataCrawler.cpp
//  DataExtractor
//
//  Created by Benjamin G Fields on 5/15/18.
//  Copyright © 2018 Benjamin G Fields. All rights reserved.
//

#include "DataCrawler.hpp"

//Description: Constructor to open and read files
DataCrawler::DataCrawler(){
  myStartFile.open("starts.txt");
  if(!myStartFile.is_open()){
    std::cout<<"Error Unable to open start event log\n";
    throw std::runtime_error("ERROR: failed to open starts.txt file!");
  }
  std::cout<<"Successfully opened start file\n";
  myFinishFile.open("Finish.txt");
  if(!myFinishFile.is_open()){
    std::cout<<"ERROR: Unable to open finish event log\n";
    throw std::runtime_error("ERROR: failed to open finish.txt file!");
  }
  std::cout<<"Successfully opened finish file\n";
  resultsFile.open("Results.txt");
  if(!resultsFile.is_open()){
    std::cout<<"ERROR: Unable to open Results.txt\n";
    throw std::runtime_error("ERROR: failed to open Results.txt file!");
  }
  std::cout<<"Reading in Files\n";
  myStarts = readInStartsFile();
  myFinishs = readInFinishFile();
  if(myStarts.data() == NULL || myFinishs.data() == NULL){
    throw std::runtime_error("ERROR: Failed to create stored starts and finishes!");
  }
  std::cout<<"Successfully read in start and finish files\n";
}

//Description:Destructor to close and present files
DataCrawler::~DataCrawler(){
  std::cout<<"Shutting down crawler\n";
  //clean up and present results
  myStartFile.close();
  myFinishFile.close();
  resultsFile.close();
  //system("open -a TextEdit ./ASAE/model.txt");
  //system("open -a TextEdit starts.txt");
  //system("open -a TextEdit Finish.txt");
  system("open -a TextEdit Results.txt");
}

//Description: Main runner to extract all process information from event data
void DataCrawler::run(){
  std::cout<<"Number of Starts: "<<myStarts.size()<<"\n";
  std::cout<<"Number of Finishes: "<<myFinishs.size()<<"\n";
  
  auto now = std::chrono::system_clock::now();
  std::time_t time = std::chrono::system_clock::to_time_t(now);
  resultsFile<<"Results for "<<std::ctime(&time)<<"\n";
  std::cout<<"Results for "<<std::ctime(&time)<<"\n";
  
  resultsFile<<"Number of Start Records: "<<myStarts.size()<<"\n";
  resultsFile<<"Number of Finish Records: "<<myFinishs.size()<<"\n";
  
  int complete = numJobsComplete(myFinishs);
  std::cout<<"Number of complete jobs is "<<complete<<"\n";
  resultsFile<<"Complete Jobs: "<<complete<<"\n";
  
  float endTime = myFinishs.back().time;
  std::cout<<"Simulation runtime: "<<endTime<<"\n";
  resultsFile<<"Simulation runtime: "<<endTime<<"\n";
  std::cout<<"Average time per job: "<<endTime/(float)complete<<"\n";
  resultsFile<<"Average time per job: "<<endTime/(float)complete<<"\n";
  
  int numProcesses = getNumProcesses(myFinishs);
  std::cout<<"There are "<<numProcesses<<" processes\n";
  resultsFile<<"Number of Processes: "<<numProcesses<<"\n";
  
  //create the transition state matrix
  int** transitionStateMatrix = new int*[numProcesses];
  for(int i =0;i<numProcesses;++i){
    transitionStateMatrix[i] = new int[numProcesses];
    for(int j = 0; j<numProcesses;++j){
      transitionStateMatrix[i][j] = 0;
    }
  }
  
  //create the transition time matrix
  float** transitionTimeMatrix = new float*[numProcesses];
  for(int i =0;i<numProcesses;++i){
    transitionTimeMatrix[i] = new float[numProcesses];
    for(int j = 0; j<numProcesses;++j){
      transitionTimeMatrix[i][j] = 0.0;
    }
  }
  
  std::cout<<"Transition State Matrix\n";
  resultsFile<<"\nTransition State Matrix\n";
  //printTransitionStateMatrix(transitionStateMatrix, numProcesses);
  //find count of transitions
  countTransitions(transitionStateMatrix, numProcesses,myFinishs);
  printTransitionStateMatrix(transitionStateMatrix, numProcesses);
  
  getTransitionTimes(transitionTimeMatrix, numProcesses, myFinishs, myStarts);
  std::cout<<"Transition Times summed\n";
  resultsFile<<"\nTotal Transition Times Matrix\n";
  printTransitionTimeMatrix(transitionTimeMatrix, numProcesses);
  averageTransitionTimes(transitionTimeMatrix, numProcesses, transitionStateMatrix);
  std::cout<<"Transition Times averaged\n";
  resultsFile<<"\nTransition Times Matrix\n";
  printTransitionTimeMatrix(transitionTimeMatrix, numProcesses);
  
  int maxJIS = getMaxJobsInSystem(myFinishs);
  std::cout<<"Max Number of Components in System: "<<maxJIS<<"\n";
  resultsFile<<"\nMax Number of Components in System: "<<maxJIS<<"\n";
}

//Description: Helper function to get the closing bracket of an initial opening bracket
int DataCrawler::getClosingBracket(int pos, std::string ID){
  int ans = -1;
  int ignore = 0;
  for(int i = pos;i<ID.length();++i){
    if(ID.substr(i,1)=="]" && ignore==0){
      ans = i;
      break;
    }
    else if(ID.substr(i,1)=="]" && ignore>0){
      ignore--;
    }
    else if(ID.substr(i,1)=="["){
      ignore++;
    }
  }
  return ans;
}

//Description: helper function used to find the next '[' character from a given position
int DataCrawler::posOfNextBracket(std::string line, int start){
  int ans = -1;
  for(int i = start;i<(int)line.length();++i){
    if(line[i] == '['){
      ans = i;
      break;
    }
  }
  return ans;
}

//Description: find the position of the first ':' in the string
int DataCrawler::getPosOfColon(std::string line){
  int pos = -1;
  for(int i = 1;i<line.length();++i){
    if(line[i] == ':'){
      pos = i;
      break;
    }
  }
  return pos;
}

//Description: get the position of the first '-' in the string
int DataCrawler::getPosOfDash(std::string line, int start){
  int pos = -1;
  for(int i = start;i<line.length();++i){
    if(line[i] == '-'){
      pos = i;
      break;
    }
  }
  return pos;
}

//Description: extract the dependency string from the job id string
std::string DataCrawler::getDependancy(std::string line){
  std::string ans;
  int start = getPosOfColon(line);
  //std::cout<<"Pos of colon is "<<start<<"\n";
  int end = getPosOfDash(line, start);
  int begin = end+2;
  int length = (int)line.length()-2-begin;
  ans = line.substr(begin,length);
  return ans;
}

//Description: get all dependencies within dependency string
std::vector<std::string> DataCrawler::getUpConnections(std::string line){
  std::vector<std::string> depend;
  std::string dependStr = getDependancy(line);
  //std::cout<<"When creating depend the string is "<<dependStr<<"\n";
  if(dependStr=="x"){
    depend.push_back("x");
    return depend;
  }
  int index = 0;
  while(index != dependStr.length()-1){
    index = posOfNextBracket(dependStr, index);
    //std::cout<<"Index is "<<index<<"\n";
    int closingPos = getClosingBracket(index+1, dependStr);
    //std::cout<<"closing bracket is "<<closingPos<<"\n";
    std::string val = dependStr.substr(index,closingPos-index+1);
    depend.push_back(val);
    //std::cout<<"depend is: "<<val<<"\n";
    index = closingPos;
  }
  return depend;
}

//Description: read in the start events into memory for storage
std::vector<Start> DataCrawler::readInStartsFile(){
  std::vector<Start> readStarts;
  std::string line;
  int lineNum = 0;
  while(std::getline(myStartFile,line)){
    Start aStart;
    std::stringstream ss(line);
    std::string token;
    std::getline(ss,token, ',');
    aStart.jobID = token;
    if(lineNum != 0){
      aStart.connections = getUpConnections(token);
      /*std::cout<<"created "<<aStart.connections.size()<<" connections\n";
       for(int i = 0;i<aStart.connections.size();++i){
       std::cout<<"connection: "<< aStart.connections[i]<<"\n";
       }*/
    }
    std::getline(ss,token, ',');
    aStart.time = std::atof(token.c_str());
    std::getline(ss,token, ',');
    aStart.jobsInSystem = std::atoi(token.c_str());
    readStarts.push_back(aStart);
    //std::cout<<"Line Number: "<<lineNum<<"\n";
    lineNum++;
  }
  return readStarts;
}

//Description: read in finish events into memory for storage and use
std::vector<Finish> DataCrawler::readInFinishFile(){
  std::vector<Finish> readFinish;
  std::string line;
  int lineNum = 0;
  while(std::getline(myFinishFile,line)){
    Finish aFinish;
    std::stringstream ss(line);
    std::string token;
    std::getline(ss,token, ',');
    aFinish.jobID = token;
    std::getline(ss,token, ',');
    aFinish.time = std::atof(token.c_str());
    std::getline(ss,token, ',');
    aFinish.jobsInSystem = std::atoi(token.c_str());
    readFinish.push_back(aFinish);
    //std::cout<<"Line Number: "<<lineNum<<"\n";
    lineNum++;
  }
  return readFinish;
}

//Description: return the number of jobs completed in simulation run from data
int DataCrawler::numJobsComplete(std::vector<Finish> &myFin){
  int ans = 0;
  //std::cout<<"Getting the number of jobs complete with jobID "<<myFin.back().jobID<<"\n";
  std::string line = myFin.back().jobID;
  int length = 0;
  int index = 1;
  while(line[index] != ':'){
    length++;
    index++;
  }
  ans = std::atoi(line.substr(1,length).c_str());
  return ans;
}

//Description: Return the number of unique processes in the system
int DataCrawler::getNumProcesses(std::vector<Finish> &myFin){
  std::unordered_set<int> myPIDs;
  for(int i = 1;i<(int)myFin.size();++i){
    std::string line = myFin[i].jobID;
    //std::cout<<"line is "<<line<<"\n";
    int start = getPosOfColon(line);
    //std::cout<<"Pos of colon is "<<start<<"\n";
    int end = getPosOfDash(line, start);
    //std::cout<<"Pos of dash is "<<end<<"\n";
    int length = end - start -1;
    //std::cout<<"length is "<<length<<"\n";
    int pid = std::atoi(line.substr(start+1,length).c_str());
    //std::cout<<"PID: "<<pid<<"\n";
    myPIDs.insert(pid);
  }
  return (int)myPIDs.size();
}

//Description:Print the transition frequency matrix to results and console
void DataCrawler::printTransitionStateMatrix(int** mat, int size){
  std::cout<<std::setw(3)<<"PID ";
  resultsFile<<std::setw(3)<<"PID ";
  for(int i = 0;i<size;++i){
    std::cout<<std::setw(3)<<i<<" ";
    resultsFile<<std::setw(3)<<i<<" ";
  }
  std::cout<<"\n";
  resultsFile<<"\n";
  for(int i = 0;i<size;++i){
    std::cout<<std::setw(3)<<i<<"|";
    resultsFile<<std::setw(3)<<i<<"|";
    for(int j = 0;j<size;++j){
      std::cout<<std::setw(3)<<mat[i][j]<<"|";
      resultsFile<<std::setw(3)<<mat[i][j]<<"|";
    }
    std::cout<<"\n";
    resultsFile<<"\n";
  }
}

//Description:Print the transition time matrix to results and console
void DataCrawler::printTransitionTimeMatrix(float** mat, int size){
  std::cout<<std::setw(9)<<std::right<<" PID";
  resultsFile<<std::setw(9)<<std::right<<" PID";
  for(int i = 0;i<size;++i){
    std::cout<<std::fixed<<std::setw(10)<<std::right<<i;
    resultsFile<<std::fixed<<std::setw(10)<<std::right<<i;
  }
  std::cout<<"\n";
  resultsFile<<"\n";
  for(int i = 0;i<size;++i){
    std::cout<<std::setw(9)<<std::right<<i<<"|";
    resultsFile<<std::setw(9)<<std::right<<i<<"|";
    for(int j = 0;j<size;++j){
      std::cout<<std::setfill(' ')<<std::fixed<<std::right<<std::setw(9)<<std::setprecision(3)<<mat[i][j]<<"|";
      resultsFile<<std::setfill(' ')<<std::fixed<<std::right<<std::setw(9)<<std::setprecision(3)<<mat[i][j]<<"|";
    }
    std::cout<<"\n";
    resultsFile<<"\n";
  }
}

//Description: Return the pid from a given jid string
int DataCrawler::getProcessID(std::string line){
  int start = getPosOfColon(line);
  //std::cout<<"Pos of colon is "<<start<<"\n";
  int end = getPosOfDash(line, start);
  //std::cout<<"Pos of dash is "<<end<<"\n";
  int length = end - start -1;
  //std::cout<<"length is "<<length<<"\n";
  int pid = std::atoi(line.substr(start+1,length).c_str());
  return pid;
}

//Description: get all the process IDs in a provided string
std::vector<int> DataCrawler::getPIDS(std::string line){
  std::vector<int> ans;
  //std::cout<<"extracting pids from line: "<<line<<"\n";
  int index = 0;
  while(index != line.length()-1){
    index = posOfNextBracket(line, index);
    //std::cout<<"Index is "<<index<<"\n";
    int closingPos = getClosingBracket(index+1, line);
    //std::cout<<"closing bracket is "<<closingPos<<"\n";
    int pid = getProcessID(line.substr(index+1,closingPos-index-1));
    ans.push_back(pid);
    //std::cout<<"PID of depend: "<<pid<<"\n";
    index = closingPos;
  }
  return ans;
}

//Description: sum all transition counts in matrix
void DataCrawler::countTransitions(int **mat, int size, std::vector<Finish> &myfin){
  for(int i = 1;i<myfin.size();++i){
    std::string jid = myfin[i].jobID;
    //std::cout<<"Jid is "<<jid<<"\n";
    int pid = getProcessID(jid);
    //std::cout<<"Receiving transition pid is "<<pid<<"\n";
    //need to get upstream transitions
    std::string depend = getDependancy(jid);
    //std::cout<<"Dependency is "<<depend<<"\n";
    if(depend == "x")continue;
    //need to parse dependency to get the PID of each upstram task if not x
    std::vector<int> upstream = getPIDS(depend);
    //std::cout<<"There are "<<upstream.size()<<" connections\n";
    for(int j = 0;j<upstream.size();++j){
      int process = upstream[j];
      mat[process][pid] = mat[process][pid]+1;
    }
  }
}

//Description: get the next start event info for a given finish job id
startInfo DataCrawler::getStart(std::string id,std::vector<Start> &myStart){
  startInfo info;
  for(int i=1;i<(int)myStart.size();++i){
    int depNum = (int)myStart[i].connections.size();
    //std::cout<<"There are "<<depNum<<" dependencies\n";
    for(int j = 0;j<depNum;++j){
      if(myStart[i].connections[j].compare(id)==0){
        //they are equal and return info
        //std::cout<<"***********************They are equal\n";
        //std::cout<<myStart[i].connections[j]<<" and "<<id<<"\n";
        info.startTime = myStart[i].time;
        info.Pid = getProcessID(myStart[i].jobID);
        return info;
      }
    }
  }
  info.startTime = -999.999;
  info.Pid = -99;
  return info;
}

//Description: Sum the transition times into the transition time matrix
void DataCrawler::getTransitionTimes(float** mat, int size, std::vector<Finish> &myFin, std::vector<Start> &myStart){
  //for every finish except last position check when it begins next and is the major dependency of the start
  for(int i = 1;i<(int)myFin.size()-1;++i){
    int pid = getProcessID(myFin[i].jobID);
    //std::cout<<"pid is "<<pid<<"\n";
    float finishTime = myFin[i].time;
    //std::cout<<"finishTime is "<<finishTime<<"\n";
    startInfo info = getStart(myFin[i].jobID,myStart);
    if(info.Pid <0)continue;
    //std::cout<<"Start time is "<<info.startTime<< " and the id is "<<info.Pid<<"\n";
    float elapsedTime = info.startTime - finishTime;
    //std::cout<<"elapsed time is "<<elapsedTime<<"\n";
    //std::cout<<"Previous value is "<<mat[pid][info.Pid];
    mat[pid][info.Pid] =  mat[pid][info.Pid] + elapsedTime;
    //std::cout<<"New value is "<<mat[pid][info.Pid]<<"\n";
  }
}

//Description: Use frequencies and summed amounts to provide an average transition time matrix
void DataCrawler::averageTransitionTimes(float** mat, int size,int** matFreq){
  for(int i = 0;i<size;++i){
    for(int j = 0;j<size;++j){
      if(matFreq[i][j]==0)continue;
      mat[i][j] = mat[i][j]/(float)matFreq[i][j];
    }
  }
}

//Description: Return the max seen number of jobs in system from finish events
int DataCrawler::getMaxJobsInSystem(std::vector<Finish> &myFin){
  int max = myFin[0].jobsInSystem;
  for(int i = 1;i<(int)myFin.size();++i){
    if(myFin[i].jobsInSystem > max){
      max = myFin[i].jobsInSystem;
    }
  }
  return max;
}


