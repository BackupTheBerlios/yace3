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

#ifndef _gogi_yace3_INPUTQUEUE_H
#define _gogi_yace3_INPUTQUEUE_H

#include <cc++/thread.h>

#ifdef  CCXX_NAMESPACES
using namespace std;
using namespace ost;
using namespace __gnu_cxx;
#endif

#include <string>
#include <queue>
#include "hash_map.h"
#include "commandargs.h"

class inner_input
{
 public:
  string user;
  string text;
};

typedef int(*commandfunc)(const string&, commandargs);

class inputqueue : public Thread
{
 public:
  typedef hash_map<string, commandfunc> commandmap;
 private:
  
  commandmap cmds;
  Semaphore sem;
  queue<inner_input> q;
  Mutex m_q;
  
 public:
  void init();
  void postInput(const string& user, const string& text); //EXT
  void run();
  void final();
};

#endif
