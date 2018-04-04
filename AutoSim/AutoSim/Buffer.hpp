//
//  Buffer.hpp
//  AutoSim
//
//  Created by Benjamin G Fields on 4/2/18.
//  Copyright © 2018 Benjamin G Fields. All rights reserved.
//
//  Description:

#ifndef Buffer_hpp
#define Buffer_hpp

#include <stdio.h>
#include <queue>
#include "Event.hpp"

enum BufferState{
  FULL,
  EMPTY,
  SPACE_LEFT,
  CAN_PULL
};

//Description:
class Buffer{
private:
  std::queue<Event> queue;
public:
  int capacity;
  Event GetNext();
  void placeInBuffer(Event E);
  int getState();
};

#endif /* Buffer_hpp */
