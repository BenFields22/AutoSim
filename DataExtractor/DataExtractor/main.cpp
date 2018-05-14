//
//  main.cpp
//  DataExtractor
//
//  Created by Benjamin G Fields on 5/4/18.
//  Copyright © 2018 Benjamin G Fields. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <unordered_set>
#include <sstream>
#include <chrono>
#include <iomanip>

std::ifstream myStartFile;
std::ifstream myFinishFile;
std::ofstream resultsFile;

typedef struct{
  std::string jobID;
  float time;
  int jobsInSystem;
  std::vector<std::string> connections;
}Start;

typedef struct{
  std::string jobID;
  float time;
  int jobsInSystem;
}Finish;

//not used
int getClosingBracket(int pos, std::string ID){
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

int posOfNextBracket(std::string line, int start){
  int ans = -1;
  for(int i = start;i<(int)line.length();++i){
    if(line[i] == '['){
      ans = i;
      break;
    }
  }
  return ans;
}

int getPosOfColon(std::string line){
  int pos = -1;
  for(int i = 1;i<line.length();++i){
    if(line[i] == ':'){
      pos = i;
      break;
    }
  }
  return pos;
}

int getPosOfDash(std::string line, int start){
  int pos = -1;
  for(int i = start;i<line.length();++i){
    if(line[i] == '-'){
      pos = i;
      break;
    }
  }
  return pos;
}

std::string getDependancy(std::string line){
  std::string ans;
  int start = getPosOfColon(line);
  //std::cout<<"Pos of colon is "<<start<<"\n";
  int end = getPosOfDash(line, start);
  int begin = end+2;
  int length = (int)line.length()-2-begin;
  ans = line.substr(begin,length);
  return ans;
}

std::vector<std::string> getUpConnections(std::string line){
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

std::vector<Start> readInStartsFile(){
  std::vector<Start> readStarts;
  std::string line;
  int lineNum = 0;
  while(std::getline(myStartFile,line)){
    Start aStart;
    std::stringstream ss(line);
    std::string token;
    std::getline(ss,token, ',');
    aStart.jobID = token;
    if(lineNum != 0)aStart.connections = getUpConnections(token);
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

std::vector<Finish> readInFinishFile(){
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

int numJobsComplete(std::vector<Finish> &myFin){
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

int getNumProcesses(std::vector<Finish> &myFin){
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

void printTransitionStateMatrix(int** mat, int size){
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

int getProcessID(std::string line){
  int start = getPosOfColon(line);
  //std::cout<<"Pos of colon is "<<start<<"\n";
  int end = getPosOfDash(line, start);
  //std::cout<<"Pos of dash is "<<end<<"\n";
  int length = end - start -1;
  //std::cout<<"length is "<<length<<"\n";
  int pid = std::atoi(line.substr(start+1,length).c_str());
  return pid;
}

std::vector<int> getPIDS(std::string line){
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

void countTransitions(int **mat, int size, std::vector<Finish> &myfin){
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

void getTransitionTimes(float** mat, int size, std::vector<Finish> &myFin, std::vector<Start> &myStart){
  //for every finish check when it begins next and is the major dependency of the start
  //add the difference to the container
  
  //get the average time based on summed time and frequency of transition
}

int main(int argc, const char * argv[]) {
  myStartFile.open("starts.txt");
  if(!myStartFile.is_open()){
    std::cout<<"Error Unable to open start event log\n";
    throw std::runtime_error("ERROR: failed to open starts.txt file!");
  }
  std::cout<<"Successfully opened start file\n";
  myFinishFile.open("Finish.txt");
  if(!myFinishFile.is_open()){
    std::cout<<"Error Unable to open finish event log\n";
    throw std::runtime_error("ERROR: failed to open finish.txt file!");
  }
  std::cout<<"Successfully opened finish file\n";
  std::cout<<"Reading in Files\n";
  std::vector<Start> myStarts = readInStartsFile();
  std::vector<Finish> myFinishs = readInFinishFile();
  if(myStarts.data() == NULL || myFinishs.data() == NULL){
    throw std::runtime_error("ERROR: Failed to create stored starts and finishes!");
  }
  std::cout<<"Successfully read in start and finish files\n";
  std::cout<<"Number of Starts: "<<myStarts.size()<<"\n";
  std::cout<<"Number of Finishes: "<<myFinishs.size()<<"\n";
  resultsFile.open("Results.txt");
  auto now = std::chrono::system_clock::now();
  std::time_t time = std::chrono::system_clock::to_time_t(now);
  resultsFile<<"Results for "<<std::ctime(&time)<<"\n";
  resultsFile<<"MODEL: TODO\n\n";
  std::cout<<"Results for "<<std::ctime(&time)<<"\n";
  resultsFile<<"Number of Start Records: "<<myStarts.size()<<"\n";
  resultsFile<<"Number of Finish Records: "<<myFinishs.size()<<"\n";
  int complete = numJobsComplete(myFinishs);
  std::cout<<"Number of complete jobs is "<<complete<<"\n";
  resultsFile<<"Complete Jobs: "<<complete<<"\n";
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
      transitionStateMatrix[i][j] = 0.0;
    }
  }
  
  std::cout<<"Transition State Matrix\n";
  resultsFile<<"\nTransition State Matrix\n";
  //printTransitionStateMatrix(transitionStateMatrix, numProcesses);
  //find count of transitions
  countTransitions(transitionStateMatrix, numProcesses,myFinishs);
  printTransitionStateMatrix(transitionStateMatrix, numProcesses);
  
  
  
  //clean up and present results
  myStartFile.close();
  myFinishFile.close();
  resultsFile.close();
  //system("open -a TextEdit Results.txt");
  return 0;
}


