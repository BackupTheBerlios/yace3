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
#include <iostream>
#include "stringutil.h"
#include "ircfunctions.h"
#include "functions.h"
#include "yace.h"
#include "irccon.h"

string getChannel(const string& yaceroom)
{
  return yace->irc().getChannel(yaceroom);
}



void sendUserIRC(const string& user, const string& what)
{
  ostringstream toirc;
  
  //::user* u = yace->users().getUser(user);
  
  //string tosendto = getChannel(u->getRoom());
  string tosendto = "#lounge";
  if(tosendto == "") return;
  
  string tosend = what;
  
  //try {
    //iconv("ISO-8859-1", "UTF-8", tosend, what);
  //} catch(std::runtime_error e) {
  //  yace->irc().send(":" + user + " PRIVMSG " + tosendto + ":KAPUTT");
  //}
  

  toirc << ":" << user << " PRIVMSG " << tosendto << " :" << tosend;
  yace->irc().send(toirc.str());  
  
  return;
}

void sendIRCUserPrivate(const string& user, const string& what, const string& to)
{
  ostringstream toirc;

  toirc << ":" << user << " PRIVMSG " << to << " :" << what;
  yace->irc().send(toirc.str());

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
  
  //yace->irc().send(":" + u->getName() + " JOIN " + yace->irc().getChannel(u->getRoom()));
  yace->irc().send(":" + u->getName() + " JOIN #lounge");
  
  return;
}

void quitIRCUser(user* u)
{
  yace->irc().send(":" + u->getName() + "QUIT :Logged out");
}

void i2y_say(const string& who, const string& what, const string& where)
{
  string color = "da1337";
  string text = what;
  string tosend = replace(yace->sql().getString("say"), "%NAME%", who);
  iconv("UTF-8", "ISO-8859-1", text, what);
  tosend = replace(tosend, "%TEXT%", text);
  tosend = replace(tosend, "%COLOR%", color);
  tosend = replaceAll(tosend);
  sendRoom(yace->irc().getRoom(where), tosend);
  return;
}

void i2y_whisper(const string& who, const string& what, const string& user)
{
  string color = "da1337";
  string toyace = replaceCommon(yace->sql().getString("whisperfrom"));
  toyace = replace(toyace, "%CNAME", "<span style=\"color:" + color + "\">" + who + "</span>");
  toyace = replace(toyace, "%TEXT%", what);
  sendUser(user, toyace);
  return;
}

