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

#include "sqlcon.h"
#include "stringutil.h"
#include <iostream>

sqlcon::sqlcon(const string& database, const string& host, const string& user, const string& pass) :
  con(database.c_str(), host.c_str(), user.c_str(), pass.c_str())
{
  have_replaces = false;
}

string sqlcon::getString(const string& key)
{
  m.enterMutex();

  if(stringcache.count(key)) {
    string ret = stringcache[key];
    m.leaveMutex();
    return ret;
  }

  Query q = con.query();
  q << "SELECT value FROM strings WHERE `key` = '" << key << "';";

  Result r = q.store();
  string ret;
  Result::iterator it = r.begin();
  while(it != r.end()) {
    ret = string((*it)[0]);
    ++it;
  }

  stringcache[key] = ret;  

  m.leaveMutex();
  return ret;
}

bool sqlcon::insertRegistry(const string& nick, const string& password, const string& mail) {
  password = replace(password, "`", " ");
	password = replace(password, "'", " ");
	mail = replace(mail, "`", " ");
	mail = replace(mail, "'", " ");
  m.enterMutex();
  Query q = con.query();
	try {
	  q << "INSERT INTO `registry` (`handle`,`password`,`access`,`rights`,`color`,`email`) VALUES ('" << nick << "', MD5('" << password << "'),NOW(),'0','#da1337','" << mail << "');";
		cout << "DEBUG: Query: " << q.preview() << endl;
		Result r = q.store();
	}
	catch (...) {
		return false;
	}
  m.leaveMutex();
	return true;
}

string sqlcon::getText(const string& key)
{
  m.enterMutex();

  if(textcache.count(key)) {
    string ret = textcache[key];
    m.leaveMutex();
    return ret;
  }

  Query q = con.query();
  q << "SELECT value FROM texts WHERE `key` = '" << key << "'";
  Result r = q.store();
  string ret;
  Result::iterator it = r.begin();
  while(it != r.end()) {
    ret = string((*it)[0]);
    ++it;
  }
  
  textcache[key] = ret;

  m.leaveMutex();
  return ret;
}


hash_map<string, string> sqlcon::getReplaces()
{
  m.enterMutex();
  hash_map<string, string> ret;

  if(have_replaces) {
    ret = replacescache;
    m.leaveMutex();
    return ret;
  }

  Query q = con.query();
  q << "SELECT `key`, `value` FROM replaces";
  Result r = q.store();
  Result::iterator it = r.begin();
  while(it != r.end()) {
    ret[string((*it)["key"])] = string((*it)["value"]);
    ++it;
  }
  
  replacescache = ret;
  have_replaces = true;
  
  m.leaveMutex();
  return ret;
}
  
long sqlcon::getConfNum(const string& key)
{
  m.enterMutex();

  if(numcache.count(key)) {
    long ret = numcache[key];
    m.leaveMutex();
    return ret;
  }

  Query q = con.query();
  q << "SELECT value FROM confnum WHERE `key` = '" << key << "'";
  Result r = q.store();
  long ret = 0;
  Result::iterator it = r.begin();
  while(it != r.end()) {
    ret = long((*it)[0]);
    ++it;
  }

  numcache[key] = ret;

  m.leaveMutex();
  return ret;
}

string sqlcon::getConfStr(const string& key)
{
  m.enterMutex();

  if(cscache.count(key)) {
    string ret = cscache[key];
    m.leaveMutex();
    return ret;
  }

  Query q = con.query();
  q << "SELECT value FROM confstr WHERE `key` = '" << key << "'";
  Result r = q.store();
  string ret;
  Result::iterator it = r.begin();
  while(it != r.end()) {
    ret = string((*it)[0]);
    ++it;
  }
  
  cscache[key] = ret;

  m.leaveMutex();
  return ret;
}

void sqlcon::wipeCache()
{
  m.enterMutex();
  have_replaces = false;
  stringcache.clear();
  cscache.clear();
  numcache.clear();
  textcache.clear();
  m.leaveMutex();
}

bool sqlcon::isReg(const string& handle)
{
  m.enterMutex();
  
  bool ret = false;
  try {
    Query q = con.query();
    q << "SELECT * FROM registry WHERE `handle` = '" << handle << "'";
    Result r = q.store();
    Result::iterator it = r.begin();
    if(it != r.end()) {
      ret = true;
    }
  }
  catch(...) {}
  m.leaveMutex();
  return ret;
}

string sqlcon::getRegStr(const string& handle, const string& key)
{
  m.enterMutex();
  string ret;

  try {
    Query q = con.query();
    q << "SELECT " << key << " FROM registry WHERE `handle` = '" << handle << "'";
    Result r = q.store();
    Result::iterator it = r.begin();
    if(it != r.end()) {
      ret = string((*it)[0]);
    }
    
  }
  catch(...) {}

  m.leaveMutex();
  return ret;
}

long sqlcon::getRegNum(const string& handle, const string& key)
{
  m.enterMutex();
  long ret = 0;

  try {
    Query q = con.query();
    q << "SELECT " << key << " FROM registry WHERE `handle` = '" << handle << "'";
    Result r = q.store();
    Result::iterator it = r.begin();
    if(it != r.end()) {
      ret = long((*it)[0]);
    }
  }catch(...) {
	  std::cout << "AFFAFF\n";
	  }

  m.leaveMutex();
  return ret;
}

bool sqlcon::setRegStr(const string& handle, const string& key, const string& val)
{
  m.enterMutex();

  bool ret = true;
  try {
    Query q = con.query();
    q << "UPDATE registry SET " << key << "='" << val << "' WHERE  `handle` = '" << handle << "'";
    q.store();
  }
  catch(...) {
    ret = false;
  }
  m.leaveMutex();
  return ret;
}

bool sqlcon::setRegNum(const string& handle, const string& key, long val)
{
  m.enterMutex();

  bool ret = true;
  try {
    Query q = con.query();
    q << "UPDATE registry SET " << key << "=" << val << " WHERE  `handle` = '" << handle << "'";
    q.store();
  }
  catch(...) {
    ret = false;
  }
  m.leaveMutex();
  return ret;
}

bool sqlcon::updateTime(const string& handle)
{
  m.enterMutex();
  
  bool ret = true;
  try {
    Query q = con.query();
    q << "UPDATE registry SET access=NOW() WHERE `handle` = '" << handle << "'";
    q.store();
  }
  catch(...) {
    ret = false;
  }
  m.leaveMutex();
  return ret;
}
