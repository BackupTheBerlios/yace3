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

#include "hkstable.h"
#include "stringutil.h"
#include <iostream>

long hkstable::get(const string& ip)
{
  m_hks.enterMutex();
  long test = hks.count(tolower(ip));
  if(test)
    test = hks[tolower(ip)];
  m_hks.leaveMutex();

  if(test)
    return hks[tolower(ip)];
  else
    return 0;
}

bool hkstable::matches(const string& ip)
{
  string lip = tolower(ip);
  bool ret = false;;
  m_hks.enterMutex();
  for(hash_map<string, long>::iterator it = hks.begin(); it != hks.end(); ++it) {
    if(hostmatch(it->first, lip))
      if(it->second) {
	ret = true;
	break;
      }
  }
  m_hks.leaveMutex();
  return ret;
}

void hkstable::set(const string& ip, long seconds)
{
  if(!consistsOf("abcdefghijklmnopqrstuvwxyz0123456789-.*", tolower(ip)))
    return;

  if(seconds > 86400)
    seconds = 86400;
  m_hks.enterMutex();
  hks[tolower(ip)] = seconds;
  m_hks.leaveMutex();
}

void hkstable::decrSeconds()
{
  bool again = true;
  while(again) {
    again = false;
    
    hash_map<string, long>::iterator it = hks.begin();
  inner: while(it != hks.end()) {
      if(it->second >= 86400) {
	++it;
	goto inner;
      }
      --(it->second);
      if(it->second <= 0) {
	hks.erase(it);
	again = true;
	break;
      }
      ++it;
    }
  }
}

hash_map<string, long> hkstable::getAll()
{
  hash_map<string, long> ret;
  m_hks.enterMutex();
  ret = hks;
  m_hks.leaveMutex();
  return ret;
}

void hkstable::clear()
{
  m_hks.enterMutex();
  hks.clear();
  m_hks.leaveMutex();
}
