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

#ifndef _gogi_yace3_SQLCON_H
#define _gogi_yace3_SQLCON_H

#include <sqlplus.hh>
#include <string>
#include <cc++/thread.h>
#include "hash_map.h"

#ifdef  CCXX_NAMESPACES
using namespace std;
using namespace ost;
using namespace __gnu_cxx;
#endif

class sqlcon
{
 private:
  Connection con; //SQL Connection!!
  Mutex m;

  // Note: Registry access is NOT cached.

  hash_map<string, string> stringcache;
  hash_map<string, string> textcache;
  hash_map<string, long> numcache;
  hash_map<string, string> cscache;

  bool have_replaces;
  hash_map<string, string> replacescache;


 public:
  
  sqlcon(const string& database, const string& host, const string& user, const string& pass);

  string getString(const string& key);
  string getText(const string& key);
  hash_map<string, string> getReplaces();
  long getConfNum(const string& key);
  string getConfStr(const string& key);

  void wipeCache();

  bool isReg(const string& handle);
  string getRegStr(const string& handle, const string& key);
  long getRegNum(const string& handle, const string& key);

  bool updateTime(const string& handle);
  bool setRegStr(const string& handle, const string& key, const string& val);
  bool setRegNum(const string& handle, const string& key, long val);

};

#endif
