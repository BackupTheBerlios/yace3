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
	connectRC ("lounge", "#yace");
	string got;
	for (;;)
	{
		std::getline (*irc, got);
		cout << "IRC-Server: " << got << endl;
		parse (got);
	}
}

string
irccon::getChannel (const string & room)
{
	if (c_rooms.count (room))
		return c_rooms[room];
	else
		return "";
}

string
irccon::getRoom (const string & channel)
{
	for (map < string, string >::const_iterator it = c_rooms.begin ();
	     it != c_rooms.end (); ++it)
	{
		if (it->second == channel)
			return it->first;
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

	cout << "DEBUG: " << str << endl;
	(*irc) << str << endl;
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

    cout << "Setting up Registry User.." << endl;
	  (*irc) << "NICK YaCEReg 1 1 yacereg " << name << " " << name << " 1 :YaCE-Registry" << endl;
		(*irc) << ":" << name << " SVSMODE YaCEReg +S" << endl; 
		(*irc) << ":YaCEReg MODE YaCEReg" << endl;
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
	c_nicks[name] = hostname;
	return;
}


void
irccon::parse (const string & what)
{
	ircargs ia (what);
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
	  if (ia.prefix() == "NickServ") {
		  if (ia.arg(0) == "YaCEReg") {
			  commandargs ca(ia.rest());
				yace->sql().insertRegistry(ca.arg(0),ca.arg(1),ca.arg(2));
				cout << "DEBUG: Added new Nick to Registry: " << ca.arg(0) << endl;
			}
		}
		
		if (ia.arg (0)[0] == '#')
		{
			if (ia.rest ()[0] == (char) 1)
			{
				string s_me = ia.rest ().substr (0, ia.rest ().length () - 2);
				s_me = s_me.substr (s_me.find (" "), s_me.length () - s_me.find ("N"));
				i2y_me (ia.prefix (), s_me, ia.arg(0));
			} else
				i2y_say (ia.prefix (), ia.rest (), ia.arg (0));
		}
		else
			i2y_whisper (ia.prefix (), ia.rest (), ia.arg (0));
		return;
	} else	if (ia.command() == "PING")
	{
		string pong = "PONG yace.filbboard.de " + ia.arg (0);
		yace->irc ().send (pong);
		return;
	}
  else if (ia.command() == "NICK")
	{
	  string nick = ia.arg(0);
		string host = ia.arg(4);
		bool hasreg = false;
		
		if (yace->users().existsUser(nick)) {
		  return;
		}
		
		hasreg = yace->sql().isReg(nick);

		if (hasreg) {
		  // We cant allow registered nicks at the moment.
		  return;
		}
		
	  user* irchehe = new user(ia.arg(0),ia.arg(4));
		irchehe->IncRef();
		yace->users().insertUser(irchehe);
		yace->rooms().joinRoom(irchehe->getName(), yace->sql().getConfStr("stdroom"));
		irchehe->DecRef();
	}
  else if (ia.command() == "JOIN") {
	  if (!exists(ia.prefix()) && !yace->sql().isReg(ia.prefix())) return;
	  string foo = replace(ia.arg(0),","," ");
	  commandargs ca(foo);
	  user* u = yace->users().getUser(ia.prefix());
		// Temporary.;
		for (int i=0; foo != ""; i++) {
		  foo = ca.arg(i);
			if (foo != "") { 
			  foo = foo.substr(1, foo.length()-2);
			  yace->rooms().joinRoom(u->getName(), foo);
			}
		}
		u->DecRef();
	}
	
	else if (ia.command() == "MODE")
	{
	  // FIXME: Doesnt parse ":tobi MODE #lounge -m"
	  if (ia.arg(0)[0] == '#') {
		  // Channelmodes
			string room = ia.arg(0);
			room = room.substr(1, room.length()-1);
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
					  // Needs getchannelthingy too
						if (adding)
						  setMod(room, 10, 25);
						if (!adding)
						  setMod(room, 0, 0);
					break;
					
				}
			}
		}
		else {
		  string user = ia.prefix();
			string modes = ia.rest();
			cout << ia.rest() << endl;
			bool adding = false;
			cout << "DEBUG: User: " << user << endl;
			cout << "DEBUG: Modes: " << modes << endl;
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
	}
	
	else /*if(ia.command() == "AWAY") {
	  string name = ia.prefix(); // solution to fix some curious
	  i2y_away(name, ia.rest());
	  return;
	} else*/ {
		return;
	}
}
