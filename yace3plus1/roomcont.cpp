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

#include "roomcont.h"
#include "room.h"
#include "user.h"
#include "yace.h"

room* roomcont::getRoom(const string& name)
{
  room* ret;

  m_rooms.enterMutex();
  if(!rooms.count(name)) 
    ret = NULL;
  else {
    ret = rooms[name];
    ret->IncRef();
  }
  m_rooms.leaveMutex();

  return ret;
}

void roomcont::joinRoom(const string& usr, const string& name, bool irc)
{
  if(!irc) {
    if(!leaves(usr))
      return;
  }

  user* pusr = yace->users().getUser(usr);
  if(pusr == NULL)
    return;

  room* newroom = getRoom(name);
  if(newroom == NULL) {
    newroom = new room(name);
    m_rooms.enterMutex();
    rooms[name] = newroom;
    m_rooms.leaveMutex();
    newroom->IncRef();
    newroom->IncRef();
  }
  newroom->insertUser(usr);
  pusr->setRoom(newroom->getName());
  newroom->DecRef();
  pusr->DecRef();
}

bool roomcont::leaves(const string& usr, bool irc, string room)
{
  user* pusr = yace->users().getUser(usr);

  if(pusr == NULL) {
    return false;
  }

  ::room* r;
  
  if(irc) {
    r = getRoom(room);
  } else {
    r = getRoom(pusr->getRoom());
  }
  
  if(r == NULL) {
    pusr->DecRef();
    return true;;
  }

  r->removeUser(usr);
  pusr->setRoom("");
  pusr->DecRef();
  check(r);
  r->DecRef();
  return true;
}

set<string> roomcont::getAllRooms()
{
  m_rooms.enterMutex();
  set<string> ret;
  
  hash_map<string, room*>::iterator it = rooms.begin();
  while(it != rooms.end()) {
    ret.insert(it->first);
    ++it;
  }
  m_rooms.leaveMutex();
  return ret;
}

void roomcont::check(room* r)
{
  if(r->isEmpty()) {
    m_rooms.enterMutex();
    rooms.erase(r->getName());
    m_rooms.leaveMutex();
    r->DecRef();
  }
}
