/*
  Copyright (C) 2003 Giuliano Montecarlo

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

#include <sstream>
#include "stringutil.h"
#include "ircfunctions.h"
#include "room.h"
#include "user.h"
#include "yace.h"
#include "irccon.h"

string getChannel(const string& yaceroom)
{
  return yace->irc().getChannel(yaceroom);
}



void sendUserIRC(const string& user, const string& what)
{
  ostringstream toirc;
  
  ::user* u = yace->users().getUser(user);
  
  string tosendto = getChannel(u->getRoom());
  if(tosendto == "") return;
  
  toirc << ":" << user << " PRIVMSG " << tosendto << " :" << what;
  yace->irc().send(toirc.str());  
  
  u->DecRef();
  return;
}

void newIRCUser(const string& who)
{
  ostringstream toirc;
  
  ::user* u = yace->users().getUser(who);
  
  string ircroom = getChannel(u->getRoom());
  if(ircroom == "") return;
  
  toirc << "NICK " << who << " 1 1 " << who << " " << u->getIP() << " " << yace->irc().getServerName() << " 1 :Yace-User";
  yace->irc().send(toirc.str());  
  toirc.clear();

  toirc << ":" << who << " SETHOST yace.filbboard.de";
  yace->irc().send(toirc.str());
  toirc.clear();

  toirc << ":" << who << " JOIN " << ircroom;
  yace->irc().send(toirc.str());
  
  u->DecRef();
  return;
}
