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

#include <iostream>
#include <string>
#include "user.h"
#include "room.h"

user::user(connection* c, Semaphore* s, Semaphore* l, const string& n, const string& addr, const string& id)
{
  sem = s;
  logout = l;
  refcount = 0;
  sid = id;
  ip = addr;
  con = c;
  name = n;
	ircuser = false;
}

user::user(const string& n, const string& addr) {
  ircuser = true;
  name = n;
  ip = addr;
}

string user::getID()
{
  if (ircuser) return NULL;
  else return sid;
}

string user::getName()
{
  return name;
}

string user::getRoom()
{
  m_room.enterMutex();
  string ret = joined_room;
 	m_room.leaveMutex();
 	return ret;
 
}

void user::setRoom(const string& n)
{
  m_room.enterMutex();
  joined_room = n;
  m_room.leaveMutex();
	
}

bool user::send(const string& data)
{
  if (ircuser) return true;
  if(con->send(data))
    return true;
  
  m_sems.enterMutex();
  logout->post();
  m_sems.leaveMutex();
  return false;
}

string user::getIP()
{
  return ip;
}

void user::IncRef()
{
  m_refcount.enterMutex();
  ++refcount;
  m_refcount.leaveMutex();
}

void user::DecRef()
{
  if (ircuser) {
	  m_refcount.enterMutex();
		--refcount;
		m_refcount.leaveMutex();
		return;
	}
	
  m_refcount.enterMutex();
  --refcount;

  if(refcount == 0) {
    //cout << "DELETING USER!" << endl;
    sem->post();
    m_refcount.leaveMutex();
    delete this;
    return;
  }

  m_refcount.leaveMutex();
}

int user::igetProp(const string& key)
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

void user::isetProp(const string& key, int val)
{
  m_iprops.enterMutex();
  iprops[key] = val;
  m_iprops.leaveMutex();
}

string user::sgetProp(const string& key)
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

void user::ssetProp(const string& key, const string& val)
{
  m_sprops.enterMutex();
  sprops[key] = val;
  m_sprops.leaveMutex();
}

void user::xsetProp(const string& key, const set<string>& val)
{
  m_xprops.enterMutex();
  xprops[key] = val;
  m_xprops.leaveMutex();
}

set<string> user::xgetProp(const string& key)
{
  set<string> ret;
  m_xprops.enterMutex();
  if(xprops.count(key))
    ret = xprops[key];
  m_xprops.leaveMutex();
  return ret;
}

void user::incrProp(const string& key)
{
  if(ircuser) return;
  m_iprops.enterMutex();
  if(!iprops.count(key))
    iprops[key] = 0;
  iprops[key] = iprops[key] + 1;
  m_iprops.leaveMutex();
}

void user::decrProp(const string& key)
{
  if(ircuser) return;
  m_iprops.enterMutex();
  if(!iprops.count(key))
    iprops[key] = 0;
  iprops[key] = iprops[key] - 1;
  m_iprops.leaveMutex();
}

void user::quit()
{
  yace->irc().send(":" + name + " QUIT :YaCE3");
  logout->post();
}
