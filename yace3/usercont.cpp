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

#include "usercont.h"
#include "user.h"
#include "stringutil.h"

void usercont::insertUser(user* ptr)
{
  m_users.enterMutex();
  users[tolower(ptr->getName())] = ptr;
  m_users.leaveMutex();
}

void usercont::removeUser(const string& name)
{
  m_users.enterMutex();
  users.erase(tolower(name));
  m_users.leaveMutex();
}

user* usercont::getUser(const string& name)
{
  string lname = tolower(name);
  user* ret;
  m_users.enterMutex();
  hash_map<string, user*>::iterator it;
  
  /* for(it = users.begin(); it != users.end(); ++it)
  {
	  if(it->second->getName() == name) ret = it->second;
	  if(it->second->sgetProp("nick") == name) ret = it->second;
  } */
	
  if(!users.count(lname))
    ret = NULL;
  else {
    ret =  users[lname];
    ret->IncRef();
  }
  m_users.leaveMutex();

	if (ret == NULL)
	{
	    for(it = users.begin(); it != users.end(); ++it)
			{ 
			  if(it->second->getName() == name) ret = it->second;
				if(it->second->sgetProp("nick") == name) ret = it->second;
			}
									
	}
  return ret;
}

set<string> usercont::getAllUsers()
{
  m_users.enterMutex();
  set<string> ret;
  hash_map<string, user*>::iterator it = users.begin();
  while(it != users.end()) {
    ret.insert(it->first);
    ++it;
  }
    
  m_users.leaveMutex();
  return ret;
}

bool usercont::existsUser(const string& name)
{
  user* u = getUser(name);
  if(u == NULL)
    return false;
  u->DecRef();
  return true;
}
