/*
  Copyright (C) 2003 Giuliano Gagliardi

  This file is part of YaCE 3

  YaCE 3 is free software, you can redistribute it and/or
  modify it under the terms of the Affero General Public License as
  published by Affero, Inc., either version 1 of the License, or
  (at your option) any later version.

  YaCE 3 is distributed in the hope that it will be
  useful, but WITHOUT ANY WARRANTY, without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  Affero General Public License for more details.

  You should have received a copy of the Affero General Public
  License in the COPYING file that comes with YaCE3. If
  not, write to Affero, Inc., 510 Third Street, Suite 225, San
  Francisco, CA 94107 USA.
*/

#include "timer.h"
#include <sys/time.h>
#include <iostream>
#include "timedactions.h"

void timer::init()
{
  register_timedactions(actions);
}

void timer::run()
{
  unsigned long tick = 0;
  unsigned long start = 0;
  unsigned long elapsed = 0;

  while(!false) {
    ++tick;
    start = getCurrentMillis();

    for(actionmap::iterator it = actions.begin(); it != actions.end(); ++it) {
      int mod = it->second;
      if((tick % mod) == 0) {
	try {
	it->first();
	}
	catch(...) {
	  cout << "SHIT!" << endl;
	}
      }
    }
    
    elapsed = getCurrentMillis() - start;
    if(elapsed >= 1000)
      continue;
    else
      Thread::sleep(1000 - elapsed);
  }
}

unsigned long timer::getCurrentMillis()
{
  timeval t;
  gettimeofday(&t, NULL);
  return (t.tv_sec * 1000) + (t.tv_usec / 1000);
}
