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

void sendIRCUserPrivate(const string& user, const string& what, const string& to)
{
  ostringstream toirc;

  ::user* u = yace->users().getUser(user);

  toirc << ":" << user << " PRIVMSG " << to << " :" << what;
  yace->irc().send(toirc.str());

  u->DecRef();
  return;
}

void newIRCUser(user* u)
{
  ostringstream toirc;
  
  toirc << "NICK " << u->getName() << " 1 1 yace " << u->getIP() << " " << yace->irc().getServerName() << " 1 :Yace-User";
  yace->irc().send(toirc.str());  

  ostringstream toirc2;
  toirc2 << ":" << u->getName() << " SETHOST yace.filbboard.de";
  yace->irc().send(toirc2.str());
  
}

void quitIRCUser(user* u)
{
  yace->irc().send(":" + u->getName() + "QUIT :Logged out");
}
