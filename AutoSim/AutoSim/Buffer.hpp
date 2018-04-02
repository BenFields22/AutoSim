//
//  Buffer.hpp
//  AutoSim
//
//  Created by Benjamin G Fields on 4/2/18.
//  Copyright © 2018 Benjamin G Fields. All rights reserved.
//

#ifndef Buffer_hpp
#define Buffer_hpp

#include <stdio.h>
#include <queue>
#include "Event.hpp"

//Description:
class Buffer{
private:
  std::queue<Event> queue;
  int capacity;
public:
  Event GetNext();
  void placeInBuffer(Event E);
  int isFull();
  int isEmpty();
};

#endif /* Buffer_hpp */
