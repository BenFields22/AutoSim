//
//  Buffer.cpp
//  AutoSim
//
//  Created by Benjamin G Fields on 4/2/18.
//  Copyright © 2018 Benjamin G Fields. All rights reserved.
//
//  Description:

#include "Buffer.hpp"

//Description:
int Buffer::getState(){
  if (queue.size() == capacity) {
    return FULL;
  }
  else if(queue.size()== 0){
    return EMPTY;
  }
  else{
    return SPACE_LEFT;
  }
}

//Description:
Event Buffer::GetNext(){
  Event E = queue.front();
  queue.pop();
  return E;
}

//Description:
void Buffer::placeInBuffer(Event E){
  queue.push(E);
}
