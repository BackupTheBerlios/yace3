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

//string color = "da1337";

string
i2y_convert (const string & text)
{
	string newtext;
	iconv ("UTF-8", "ISO-8859-1", newtext, text);
	return newtext;
}

string
getChannel (const string & yaceroom)
{
	return yace->irc ().getChannel (yaceroom);
}



void
sendUserIRC (const string & user, const string & what)
{
	ostringstream toirc;

	::user* u = yace->users().getUser(user);

	//string tosendto = getChannel(u->getRoom());
	string tosendto = replace('#' + u->getRoom(), " ", "_");
	if (tosendto == "")
		return;

	string tosend = what;

	//try {
	//iconv("ISO-8859-1", "UTF-8", tosend, what);
	//} catch(std::runtime_error e) {
	//  yace->irc().send(":" + user + " PRIVMSG " + tosendto + ":KAPUTT");
	//}


	toirc << ":" << u->sgetProp("nick") << " PRIVMSG " << tosendto << " :" << tosend;

	yace->irc().send (toirc.str ());
  u->DecRef();
	return;
}

void
sendIRCUserPrivate (const string & user, const string & what,
		    const string & to)
{
	ostringstream toirc;

	toirc << ":" << user << " PRIVMSG " << to << " :" << what;
	yace->irc ().send (toirc.str ());

	return;
}

void
newIRCUser (user * u)
{
	ostringstream toirc;

	toirc << "NICK " << u->sgetProp("nick") << " 1 1 " << u->getName() << " " << u->getIP() << " " << yace->irc().getServerName () << " 1 :YaCE-User";
	yace->irc ().send (toirc.str ());

	ostringstream toirc2;
	string newip = string(u->sgetProp("nick") + u->getIP().substr(u->getIP().find("."), u->getIP().length()-u->getIP().find(".")));
	toirc2 << ":" << u->sgetProp("nick") << " SETHOST " << newip;
	yace->irc ().send (toirc2.str ());

	//yace->irc().send(":" + u->sgetProp("nick") + " JOIN " + yace->irc().getChannel(u->getRoom()));
	yace->irc ().send (":" + u->sgetProp("nick") + " JOIN " + replace('#' + u->getRoom(), " ", "_"));

	yace->irc ().send (":" + u->sgetProp("nick") + " MODE :+xt");

	return;
}

void
quitIRCUser (user * u)
{
	yace->irc().send(":" + u->sgetProp("nick") + " QUIT :Logged out");
}

void
i2y_say (const string & who, const string & what, const string & where)
{
	::user* u = yace->users().getUser(who);
	string text = what;
	string tosend =
		replace (yace->sql ().getString ("say"), "%NAME%", who);
	tosend = replace (tosend, "%TEXT%", replaceAll (text));
	tosend = replace (tosend, "%COLOR%", u->sgetProp("color"));
	tosend = i2y_convert (tosend);
	sendRoom (replace(where.substr(1, where.length()-1), "_", " "), tosend);
	return;
}

void
i2y_whisper (const string & who, const string & what, const string & user)
{
	::user* u = yace->users().getUser(who);
	string toyace =
		replaceCommon (yace->sql ().getString ("whisperfrom"));
	toyace = replace (toyace, "%CNAME%",
			  "<span style=\"color:" + u->sgetProp("color") + "\">" + who +
			  "</span>");
	toyace = replace (toyace, "%TEXT%", what);
	sendUser (user, toyace);
	return;
}

void
i2y_away (const string & who, const string & what)
{
	string toyace;
	if(yace->rooms().getRoom(yace->users().getUser(who)->getRoom()) == NULL)
	{
		setAway(who, i2y_convert (what));
		return;
	}
	
	if(isAway(who))
	{
		toyace = replaceCommon(yace->sql().getString("back"));
		toyace = replaceUser(who, toyace);
		toyace = replace(toyace, "%TEXT%", replaceAll(getAway(who)));
	} else {	
		toyace = replaceCommon (yace->sql ().getString ("away"));
		setAway(who, i2y_convert (what));
		toyace = replace (toyace, "%TEXT%", replaceAll (i2y_convert (what)));
		toyace = replaceUser(who, toyace);
    }
	try {
		sendRoom (yace->users().getUser(who)->getRoom(), toyace);
	} catch(...) {
	}
	return;
}

void
y2i_away (const string & who, const string & why)
{
	yace->irc ().send (":" + who + " AWAY :" + why);
	//yace->irc().send(":" + who + " PRIVMSG " + getChannel(u->getRoom()) + " :" + chr(3) + "ACTION: " +  why + chr(3));
	return;
}

void
y2i_back (const string & who)
{
	yace->irc ().send (":" + who + " AWAY");
	return;
}

string IRCrtoYaCEr(const string& which) {
  string foo = which;
  foo = replace(foo.substr(1,foo.length()-1), "_", "");
  foo = replace(foo, "\r", "");
  if(foo.find(" ") == foo.length()) foo = foo.substr(0,foo.length()-1);
  return foo;

}
void
i2y_me(const string& who, const string& what, const string& where)
{
	::user* u = yace->users().getUser(who);
	string toyace = replace(yace->sql().getString("me"), "%COLOR%", u->sgetProp("color"));
	toyace = replace(toyace, "%NAME%", who);
	toyace = replace(toyace, "%TEXT%", i2y_convert(what));
	sendRoom(replace(where.substr(1, where.length()-1), "_", " "), toyace);
	return;
}
