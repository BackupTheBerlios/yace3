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

#include "room.h"
#include "user.h"
#include "stringutil.h"
#include <iostream>

room::room(string n)
{
  refcount = 0;
  name = n;
}

string room::getName()
{
  return name;
}

void room::insertUser(const string& u)
{
  m_members.enterMutex();
  members.insert(tolower(u));
  m_members.leaveMutex();
}

void room::removeUser(const string& u)
{
  m_members.enterMutex();
  members.erase(tolower(u));
  m_members.leaveMutex();
}

set<string> room::getAllUsers()
{
  m_members.enterMutex();
  set<string> ret = members;
  m_members.leaveMutex();
  return ret;
}

bool room::isEmpty()
{
  m_members.enterMutex();
  bool ret = members.empty();
  m_members.leaveMutex();
  return ret;
}

void room::IncRef()
{
  m_refcount.enterMutex();
  ++refcount;
  m_refcount.leaveMutex();
}

void room::DecRef()
{
  m_refcount.enterMutex();
  --refcount;

  if(refcount == 0) {
    //cout << "DELETING ROOM!" << endl;
    m_refcount.leaveMutex();
    delete this;
    return;
  }

  m_refcount.leaveMutex();
}

int room::igetProp(const string& key)
{
  int ret;
  m_iprops.enterMutex();
  if(iprops.count(key))
    ret = iprops[key];
  else
    ret = 0;
  m_iprops.leaveMutex();
  return ret;
}

void room::isetProp(const string& key, int val)
{
  m_iprops.enterMutex();
  iprops[key] = val;
  m_iprops.leaveMutex();
}

string room::sgetProp(const string& key)
{
  string ret;
  m_sprops.enterMutex();
  if(sprops.count(key))
    ret = sprops[key];
  else
    ret = "";
  m_sprops.leaveMutex();
  return ret;
}

void room::ssetProp(const string& key, const string& val)
{
  m_sprops.enterMutex();
  sprops[key] = val;
  m_sprops.leaveMutex();
}

void room::incrProp(const string& key)
{
  m_iprops.enterMutex();
  if(!iprops.count(key))
    iprops[key] = 0;
  iprops[key] = iprops[key] + 1;
  m_iprops.leaveMutex();
}

void room::decrProp(const string& key)
{
  m_iprops.enterMutex();
  if(!iprops.count(key))
    iprops[key] = 0;
  iprops[key] = iprops[key] + 1;
  m_iprops.leaveMutex();
}

void room::xsetProp(const string& key, const set<string>& val)
{
  m_xprops.enterMutex();
  xprops[key] = val;
  m_xprops.leaveMutex();
}

set<string> room::xgetProp(const string& key)
{
  set<string> ret;
  m_xprops.enterMutex();
  if(xprops.count(key))
    ret = xprops[key];
  m_xprops.leaveMutex();
  return ret;
}
