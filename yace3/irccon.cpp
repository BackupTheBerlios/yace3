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

#include <vector>
#include <sstream>
#include "irccon.h"
#include "functions.h"
#include "commandargs.h"
#include "stringutil.h"
#include "ircargs.h"
#include "yace.h"
#include "user.h"
#include "connection.h"
#include "ircargs.h"
#include "stringutil.h"

irccon::irccon(const string& h, int p, const string& n, const string& pwd)
{
  connected = false;
  host = h;
  port = p;
  name = n;
  pass = pwd;
}

void irccon::run()
{
  connect();
  string got;
  for(;;) {
    std::getline(*irc, got);
    
    commandargs ca(got);
    ircargs ia(got);
    if(ca.arg(0) == "PING") {
      string pong = ":" + name + " PONG " + name + " " + ca.arg(1);
      ircargs ia(got);
      cout << ia.command() << endl;
    } 
    /*if(ia.command() == "PING") {
      string pong = ":" + name + " PONG " + name + " " + ia.arg(2);
      (*irc) << pong << endl;
    } */
    if(ia.command() == "PRIVMSG") {
      string from = ia.prefix();
      string to = ia.arg(0);
      string what = ia.rest();
      // cout << "DEBUG: Got Privmsg from " << from << " to " << to << ": " << what << endl;
      
      if (to[0] == '#') {
        // Public
	ostringstream tosend;
	tosend << "(" << from << ") " << what;
	sendAll(tosend.str());
      }
      else {
        // Private
        string tosend = replaceUser(from, yace->sql().getString("whisperfrom"));
	// FIXME: replaceUser needs user-class as first argument.

        tosend = replace(tosend, "%TEXT%", what);
        sendUser(to, tosend);
      }
    }
    if (got != "") {
        cout << got << endl;
    }
  }
}

string irccon::getChannel(const string& room)
{
  if(c_rooms.count(room))
    return c_rooms[room];
  else
    return "";
}

string irccon::getRoom(const string& channel)
{
  for(map<string, string>::const_iterator it = c_rooms.begin(); it != c_rooms.end(); ++it) {
    if (it->second == channel)
      return it->first;
  }
  return "";
}

void irccon::connectRC(const string& room, const string& channel)
{
  if(channel == "")
    if(c_rooms.count(room)) {
      c_rooms.erase(room);
      return;
    }
  
  string a;
  if((a = getRoom(channel)) != "") {
    c_rooms.erase(a);
  }
  
  c_rooms[room] = channel;
}

void irccon::send(const string& str)
{

  cout << "DEBUG: " << str << endl;
  (*irc) << str << endl;
}

bool irccon::connect()
{
  irc = new TCPStream(InetHostAddress(host.c_str()), port);
  if(!irc->isConnected()) {
    delete irc;
    return false;
  }

  try {
    (*irc) << "PASS " << pass << endl;
    (*irc) << "SERVER " << name << " 1 :YaCE Connection (alpha)" << endl;

    (*irc) << "NICK YaCE 1 1 yace " << name << " " << name << " 1 :YaCE-Testfreak" << endl;
    (*irc) << ":YaCE JOIN #yace" << endl;
    
  }
  catch(...) {
    delete irc;
    return false;
  }
  
  if(!irc->isConnected()) {
    delete irc;
    return false;
  }
  
  connected = true;
  return true;
}

