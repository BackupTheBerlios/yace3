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

#ifndef _gogi_yace3_USER_H
#define _gogi_yace3_USER_H

#include <cc++/thread.h>
#include <string>
#include <set>
#include "hash_map.h"
#include "yace.h"

#ifdef CCXX_NAMESPACES
using namespace ost;
using namespace std;
using namespace __gnu_cxx;
#endif

#include "connection.h"

class room;
class connection;

class user
{
 private:
  long refcount;
  Mutex m_refcount;

  Semaphore* sem;
  Semaphore* logout;
  Mutex m_sems;

  string name;
  string ip;
  string sid;
  string joined_room;
  connection* con;
  Mutex m_room;
 
  Mutex m_sprops;
  Mutex m_iprops;
  Mutex m_xprops;
  hash_map< string, set<string> > xprops;
  hash_map<string, string> sprops;
  hash_map<string, int> iprops;
  
 public:
  user(connection* c, Semaphore* s, Semaphore* l, const string& n, const string& addr, const string& id);
  string getID();
  string getName();
  string getIP();
  string getRoom();
  void setRoom(const string& n);
  bool send(const string& data);

  void IncRef();
  void DecRef();
  
  void incrProp(const string& key);
  void decrProp(const string& key);
  int igetProp(const string& key);
  void isetProp(const string& key, int val);
  string sgetProp(const string& key);
  void ssetProp(const string& key, const string& val);
  void xsetProp(const string& key, const set<string>& val);
  set<string> xgetProp(const string& key);

  void quit();
};

#endif
