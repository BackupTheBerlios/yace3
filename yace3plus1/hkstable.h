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

#ifndef _gogi_yace3_HKSTABLE_H
#define _gogi_yace3_HKSTABLE_H

#include "hash_map.h"
#include <cc++/thread.h>
#include <string>

#ifdef CCXX_NAMESPACES
using namespace ost;
using namespace std;
using namespace __gnu_cxx;
#endif

class hkstable
{
  hash_map<string, long> hks;
  Mutex m_hks;
  
 public:
  bool matches(const string& ip);
  long get(const string& ip);
  void set(const string& ip, long seconds);
  void clear();
  hash_map<string, long> getAll();
  void decrSeconds();
};

#endif
