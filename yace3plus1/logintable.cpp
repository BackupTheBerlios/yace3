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

#include "logintable.h"
#include "stringutil.h"

bool logintable::allowed(const string& name, const string& id, const string& ip)
{
  m_tab.enterMutex();
  
  bool ret = false;
  logininfo copy = tab[tolower(name)];
  if(copy.ip != "") {
    if (/*(copy.id == id) && */copy.ip == ip) {
      ret = true;
      tab.erase(tolower(name));
    }
  }
  m_tab.leaveMutex();
  return ret;
}

void logintable::allow(const string& name, const string& id, const string& ip)
{
  m_tab.enterMutex();
  tab[tolower(name)] = logininfo(id, ip);
  m_tab.leaveMutex();
}

void logintable::clear()
{
  m_tab.enterMutex();
  tab.clear();
  m_tab.leaveMutex();
}

void logintable::remove(const string& name)
{
	m_tab.enterMutex();
	tab.erase(tolower(name));
	m_tab.leaveMutex();
}
