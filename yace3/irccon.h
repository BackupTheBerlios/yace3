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

#ifndef _gogi_yace3_IRCCON_H
#define _gogi_yace3_IRCCON_H

#include <cc++/thread.h>
#include <cc++/socket.h>
#include <map>
#include <vector>

#ifdef  CCXX_NAMESPACES
using namespace std;
using namespace ost;
using namespace __gnu_cxx;
#endif

class irccon : public Thread
{
private:
  TCPStream* irc;
  string host, name, pass;
  int port;
  bool connected;

  map<string, string> c_rooms;
  vector<string> irc_nicks;
  map<string, string> c_nicks;

  Mutex m_connection;
  Mutex m_state;
  Mutex m_parser;
  Mutex m_run;

public:
  irccon(const string& h, int p, const string& n, const string& pwd);
  void run();
  bool connect();
  void irccon::send(const string& str);

  string getServerName() 
  {
	  //m_irc.enterMutex(); 
	  string toret = this->name; 
	  //m_irc.leaveMutex();
		  return toret;
  }

  string getChannel(const string& room);
  string getRoom(const string& channel);
  void insertUser(const string& name, const string& hostname);
  void connectRC(const string& room, const string& channel);
  inline void parse(const string& what);

};

#endif
