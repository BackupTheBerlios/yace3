/*
  Copyright (C) 2003 Giuliano Gagliardi, Tobias Bahls

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

#ifndef _gogi_yace3_ROOMCONT_H
#define _gogi_yace3_ROOMCONT_H

#include <map>
#include <string>
#include <vector>
#include <cc++/thread.h>
#include "hash_map.h"
#include <set>
#include "room.h"

#ifdef  CCXX_NAMESPACES
using namespace std;
using namespace ost;
using namespace __gnu_cxx;
#endif


class user;
class room;

class roomcont
{
 private:
  hash_map<string, room*> rooms;
  Mutex m_rooms;

  void check(room* r);

 public:
  room* getRoom(const string& name);
  void joinRoom(const string& usr, const string& name, bool irc =false);
  bool leaves(const string& usr, bool irc =false, string room ="");
  set<string> getAllRooms();
};


#endif
