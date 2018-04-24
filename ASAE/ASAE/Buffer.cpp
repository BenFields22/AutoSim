//
//  Buffer.cpp
//  ASAE
//
//  Created by Benjamin G Fields on 4/2/18.
//  Copyright © 2018 Benjamin G Fields. All rights reserved.
//
//  Description: defines the implementatin of the buffer object

#include "Buffer.hpp"

//Description:return the state of the buffer by saying if it is full,empty or still has space
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

//Description:return the next event in the queue
Event Buffer::GetNext(){
  Event E = queue.front();
  queue.pop();
  return E;
}

//Description: put the event in the buffer queue
void Buffer::placeInBuffer(Event E){
  queue.push(E);
}
