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

#ifndef _gogi_yace3_YACE_H
#define _gogi_yace3_YACE_H

#include "hash_map.h"
#include <map>
#include <string>
#include <fstream>
#include <cc++/thread.h>

#ifdef  CCXX_NAMESPACES
using namespace std;
#endif

#include "logintable.h"
#include "usercont.h"
#include "roomcont.h"
#include "hkstable.h"
#include "config.h"
#include "sqlcon.h"
#include "inputqueue.h"
#include "timer.h"
#include "irccon.h"

class YaCE
{
private:
  sqlcon* sqlc;
  hkstable hks;
  usercont userc;
  roomcont roomc;
  logintable auth;
  config cfg;
  ofstream* logfilep;
  
  Mutex m_log;

  inputqueue inqu;
  timer thetimer;

  irccon* ic;
  
public:
  inputqueue& iq()
  {
    return inqu;
  }

  sqlcon& sql()
  {
    return *sqlc;
  }

  irccon& irc()
  {
    return *ic;
  }
  
  config& conf()
  {
    return cfg;
  }
  
  usercont& users()
  {
    return userc;
  }
  
  roomcont& rooms()
  {
    return roomc;
  }
  
  hkstable& bans()
  {
    return hks;
  }

  logintable& auths()
  {
    return auth;
  }
  
  YaCE(const string &cfgfile);
  void run();
  void log(const string& add);
  ~YaCE();
};

extern YaCE* yace;

#endif //a
