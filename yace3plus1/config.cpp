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

#include "config.h"
#include <fstream>
#include <cc++/thread.h>
#include <exception>
#include <iostream>

config::config(const string& filename)
{
  ifstream file(filename.c_str(), ios::in);
  string line;
  string key;
  string val;

  if(!file)
    throw exception();

  while(!file.eof()) {
    std::getline(file, line);
    
    unsigned long pos;
    if ((pos = line.find("=")) == string::npos)
      continue;

    key = line.substr(0, pos);
    val = line.substr(pos + 1);
    hm[key] = val;
  }
}

string config::get(const string& key)
{
  m_hm.enterMutex();
  string ret = hm[key];
  m_hm.leaveMutex();
  return ret;
}
