/*
  Copyright (C) 2003 Giuliano Gagliardi, Tobias Bahls, Giuliano Montecarlo

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
#include "ircfunctions.h"
#include "commands.h"
#include "hash_map.h"

#define SEBI_SUCKS
irccon::irccon (const string & h, int p, const string & n, const string & pwd)
{
	connected = false;
	host = h;
	port = p;
	name = n;
	pass = pwd;
}

void
irccon::run ()
{
	connect ();
	string got;
	for (;;)
	{
		m_run.enterMutex();
		std::getline (*irc, got);
		cout << "IRC-Server: " << got << endl;
		parse (got);
		m_run.leaveMutex();
	}
}

string
irccon::getChannel (const string & room)
{
	if (c_rooms.count (room)) {
	    return c_rooms[room];
	} else {
		return "";
	}
}

string
irccon::getRoom (const string & channel)
{
	for (map < string, string >::const_iterator it = c_rooms.begin ();
	     it != c_rooms.end (); ++it)
	{
		if (it->second == channel) {
		  return it->first;
		}
	}
	return "";
}

void
irccon::connectRC (const string & room, const string & channel)
{
	if (channel == "")
		if (c_rooms.count (room))
		{
			c_rooms.erase (room);
			return;
		}

	string a;
	if ((a = getRoom (channel)) != "")
	{
		c_rooms.erase (a);
	}

	c_rooms[room] = channel;
}

void
irccon::send (const string & str)
{
  if (!connected) 
    return;
  m_connection.enterMutex();
  cout << "DEBUG: " << str << endl;
  (*irc) << str << endl;
  m_connection.leaveMutex();
}

bool
irccon::connect ()
{
	irc = new TCPStream (InetHostAddress (host.c_str ()), port);
	if (!irc->isConnected ())
	{
		delete irc;
		return false;
	}

	try
	{
		(*irc) << "PASS " << pass << endl;
		(*irc) << "SERVER " << name << " 1 :YaCE Connection (alpha)"
			<< endl;

    cout << "Setting up YaCEServ" << endl;
	  (*irc) << "NICK YaCEServ 1 1 YaCEServ " << name << " " << name << " 1 :YaCE Services" << endl;
		(*irc) << ":" << name << " SVSMODE YaCEServ +S" << endl;
		//(*irc) << ":YaCE JOIN #lounge" << endl;

	}
	catch (...)
	{
		delete irc;
		return false;
	}

	if (!irc->isConnected ())
	{
		delete irc;
		return false;
	}

	connected = true;
	return true;
}

void
irccon::insertUser (const string & name, const string & hostname)
{
  if (!connected) return;
	c_nicks[name] = hostname;
}


inline void
irccon::parse (const string & what)
{
	m_parser.enterMutex();
	ircargs ia (htmlspecialchars(what));
	/*
	  Insertme:
	  if (ia.command() == "NICK") {
	    yace->irc().insertUser(ia.arg(0), ia(arg4));
	    replace(yace->sql().getString("enters"),"%CNAME%",ia.arg(0));
	    return;
	  }
	*/
	if (ia.command () == "PRIVMSG")
	{
		  commandargs ca(ia.rest());
	  if (ia.arg(0) == "YaCEServ") {
		  if (ia.prefix() == "NickServ") {
			  yace->sql().insertRegistry(ca.arg(0),ca.arg(1),ca.arg(2));
		   	  m_parser.leaveMutex();
			  return;
			}

			// YaCEServ needs OperServ access for some Features
			commandargs ca(ia.rest());
			typedef hash_map<string, commandfunc> commandmap;
			string command;
			commandargs argz("");
			commandfunc f;
			commandmap cmds;
      register_commands(cmds);
			
			bool iscmd = false;
			if (ca.arg(0) == "help") {
			  cout << "HELP REQUEST!" << endl;
			  iscmd = false;
			  yace->irc().send(":YaCEServ PRIVMSG " + ia.prefix() + ": YaCEServ Help:");
				yace->irc().send(":YaCEServ PRIVMSG " + ia.prefix() + ": Pic <url>: Inserts picture");
				yace->irc().send(":YaCEServ PRIVMSG " + ia.prefix() + ": YaCEServ Sound <url>: Inserts Sound");
			  yace->irc().send(":YaCEServ PRIVMSG " + ia.prefix() + ": YaCEServ Vipmsg <text>: Vipmsg with <text>");
			  yace->irc().send(":YaCEServ PRIVMSG " + ia.prefix() + ": YaCEServ Amsg <text>: Admin msg with <text>");
			}
			else if (ca.arg(0) == "Pic") {
			  iscmd = true;
				command = "p";
				argz = ca.rest(0);
			}
			if (iscmd) {
			  f = cmds[command];
			  f(ia.prefix(),argz);
			}
		}

	  user* u = yace->users().getUser(ia.prefix());
		u->incrProp("said");
		u->isetProp("silence",0);

		if (ia.arg (0)[0] == '#')
		{
			if (ia.rest ()[0] == (char) 1)
			{
				string s_me = ia.rest ().substr (0, ia.rest ().length () - 2);
				s_me = s_me.substr (s_me.find (" "), s_me.length () - s_me.find ("N"));
				i2y_me (ia.prefix (), s_me, ia.arg(0));
			    m_parser.leaveMutex();
				return;
				} else {
				i2y_say (ia.prefix (), ia.rest (), ia.arg (0));
		        m_parser.leaveMutex();
				return; 
				}
			} else {
			i2y_whisper (ia.prefix (), ia.rest (), ia.arg (0));
		m_parser.leaveMutex();
		return;
		}
	} else	if (ia.command() == "PING")
	{
		string pong = "PONG " + name + " " + ia.arg (0);
		yace->irc ().send (pong);
		m_parser.leaveMutex();
		return;
	}
	else if (ia.command() == "TOPIC") 
	{
	  string msg = yace->sql().getString("settopic");
		msg = replaceCommon(msg);
		msg = replaceUser(ia.prefix(), msg);
		msg = replace(msg, "%TEXT%", ia.rest().substr(0, ia.rest().length()-1));
		sendRoomU(ia.prefix(),msg);
	  setTopic(getRoom(ia.arg(0)),ia.rest());
	  m_parser.leaveMutex();
		return;
	}
  else if (ia.command() == "NICK")
	{
	  if (ia.prefix() != "") {
			string tosend;
			tosend = yace->sql().getString("nick");
			tosend = replaceCommon(tosend);
			tosend = replace(tosend, "%NICK%", ia.arg(0));
			tosend = replaceUser(ia.prefix(), tosend);
			yace->users().getUser(ia.prefix())->ssetProp("nick", ia.arg(0));
			sendRoomU(ia.prefix(), tosend);
			m_parser.leaveMutex();
		  return;																			
		}
	  string nick = ia.arg(0);
		string host = ia.arg(4);
		bool hasreg = false;
		if (yace->users().existsUser(nick)) {
		  m_parser.leaveMutex();
			return;
		}
		
		hasreg = yace->sql().isReg(nick);

		//if (hasreg) {
		  // We need NickServ-Plugins for identify
		  // yace->irc().send(":yace.filbboard.de KILL " + nick + " :Registered. If its your nick, please reconnect and identify");
		  //return;
		//}

		if(nick.length() >= 5) {
		  if (nick.substr(nick.length()-4,4) == "Serv" || nick == "DevNull" || nick == "Global" || nick == "BrotSheep") {
		    m_parser.leaveMutex();
			  return;
		  }
	   }
		
	  user* irchehe = new user(ia.arg(0),ia.arg(4));
	  user* hehe = irchehe;
		irchehe->IncRef();
		yace->users().insertUser(irchehe);
		 yace->rooms().joinRoom(irchehe->getName(), "TEMP-ROOM"); // this is untill we got a JOIN
	   //TOLLE SACHEN
		string regstrings = yace->sql().getConfStr("regstrings");
    string regnums = yace->sql().getConfStr("regnums");

    {
      if(hasreg)
	yace->sql().updateTime(hehe->getName());

      commandargs str(regstrings);
      
      string act;
      string actval;
      for(int i = 0; (act = str.arg(i)) != ""; ++i) {
	
	unsigned long pos = act.find("=");
	if(pos != string::npos) {
	  actval = act.substr(pos + 1);
	  act = act.substr(0, pos);
	}
	else {
	  actval = "";
	}

	if(hasreg)
	  hehe->ssetProp(act, yace->sql().getRegStr(nick, act));
	else
	  hehe->ssetProp(act, actval);
      }

      commandargs num(regnums);

      for(int i = 0; (act = num.arg(i)) != ""; ++i) {
	unsigned long pos = act.find("=");
	if(pos != string::npos) {
	  actval = act.substr(pos + 1);
	  act = act.substr(0, pos);
	}
	else {
	  actval = "";
	}

	long actnumval;
	istringstream is(actval);
	is >> actnumval;
	
	if(hasreg)
	  hehe->isetProp(act, yace->sql().getRegNum(nick, act));
	else
	  hehe->isetProp(act, actnumval);
      }
    }
    // END TOLLE SACHEN
		enters(irchehe);
    	yace->rooms().leaves(irchehe->getName(), true, "TEMP-ROOM");

	    irchehe->DecRef();
	    m_parser.leaveMutex();
	    return;
	}
  else if (ia.command() == "JOIN") {
	  if (!exists(ia.prefix())/* || !yace->sql().isReg(ia.prefix())*/) return;
	  string foo = replace(ia.arg(0),","," ");
	  commandargs ca(foo);
	  user* u = yace->users().getUser(ia.prefix());
		
	    // Temporary.;
		for (int i=0; foo != ""; i++) {
		  foo = ca.arg(i);
			if (foo != "") { 
			  foo = foo.substr(1, foo.length()-1);
			  if(foo[foo.length()-1] == ' ') foo = foo.substr(0,foo.length()-1);
			  yace->rooms().joinRoom(u->getName(), foo, true);
			}
		}
		u->DecRef();
		m_parser.leaveMutex();
		return;
	}
  else if (ia.command() == "KILL") {
	  sendRoomU(ia.arg(0),"KILL from IRC: " + ia.rest());
	  user* u = yace->users().getUser(ia.arg(0));
		leaves(u);
		yace->rooms().leaves(u->getName());
		yace->users().removeUser(u->getName());
		u->DecRef();
	    m_parser.leaveMutex();
	  return;
	  }
	
	/* else if (ia.command() == "NICK") {
	  string tosend;
		tosend = yace->sql().getString("nick");
		tosend = replaceCommon(tosend);
		tosend = replace(tosend, "%NICK%", ia.arg(0));
		tosend = replaceUser(ia.prefix(), tosend);
		yace->users().getUser(ia.prefix())->ssetProp("nick", ia.arg(0));
		sendRoomU(ia.prefix(), tosend);
	    return;
	} */
	
  else if (ia.command() == "QUIT") {
	  user* u = yace->users().getUser(ia.prefix());
		leaves(u);
		yace->rooms().leaves(u->getName());
		yace->users().removeUser(u->getName());
		u->DecRef();
		m_parser.leaveMutex();										
	    return;
	  }
	else if (ia.command() == "MODE")
	{
	  if (ia.arg(0)[0] == '#') {
		  // Channelmodes
			string room = ia.arg(0);
			//cout << room << endl;
			string modes = ia.arg(1);
			bool adding = false;
			
			for (unsigned int i=0; i<modes.length(); i++) {
			  switch (modes[i]) {
				  case '+':
					  adding = true;
					break;
					
					case '-':
					  adding = false;
					break;
					
					case 'i':
					  // Needs getchannelthingy
					  if (adding)
						  lock(room);
						if (!adding)
						  unlock(room);
					break;
			
          case 'm':
					  string notice;
						if (adding) {
						  notice = yace->sql().getString("mod");
						  setMod(getRoom(room), 10, 25);
						}
						if (!adding) {
						  setMod(getRoom(room), 0, 0);
							notice = yace->sql().getString("remove_mod");
						}
						
						notice = replaceUser(ia.prefix(), notice);
						notice = replaceCommon(notice);

						sendRoomU(ia.prefix(),notice);
					break;
					
				}
			}
		}
		else {
		  string user = ia.prefix();
			string modes = ia.rest();
			//cout << ia.rest() << endl;
			bool adding = false;
			//cout << "DEBUG: User: " << user << endl;
			//cout << "DEBUG: Modes: " << modes << endl;
			for (unsigned int i=0;i<modes.length(); i++) {
			  switch (modes[i]) {
			    case '+':
				    adding = true;
				  break;
				
			  	case '-':
		  		  adding = false;
		  		break;

		  		case 'O':
		  		  if (adding)
						  setRights(user,51);
					  else
						  setRights(user,0);
		  		break;

					case 'C':
					  if (adding)
						  setRights(user,201);
						else
						  setRights(user,0);
					break;
					
				  case 'A':
					  if (adding)
						  setRights(user,200);
						else
						  setRights(user,0);
					break;

					case 'o':
					  if (adding)
						  setRights(user,25);
					  else
						  setRights(user,0);
					break;
				}
				
			}
		}
		m_parser.leaveMutex();
		return;
	} else if(ia.command() == "AWAY") {
	  // string name = ia.prefix(); // solution to fix some curious
	  i2y_away(ia.prefix(), ia.rest().substr(0, ia.rest().length()-1));
	  m_parser.leaveMutex();
		return;
	} else if (ia.command() == "PART") {
	  ::user* u = yace->users().getUser(ia.prefix());
			  string foo = replace(ia.arg(0).substr(1, ia.arg(0).length()-1), "_", " ");
				yace->rooms().leaves(u->getName(), true, foo);
		u->DecRef();
		m_parser.leaveMutex();
		return;
	} else {

	  cout << "UNHANDLED: " << what << endl;
	m_parser.leaveMutex();
	return;
	}
}
