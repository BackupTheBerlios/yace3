/*
  Copyright (C) 2003 Giuliano Gagliardi, Tobias Bahls

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

#include "loginhandler.h"
#include "stringutil.h"
#include "connection.h"
#include "yace.h"
#include "user.h"
#include "room.h"
#include "util.h"
#include "roomcont.h"
#include "usercont.h"
#include "functions.h"
#include "commandargs.h"
#include "ircfunctions.h"

Mutex loginhandler::loginmutex;
enum loginerror { banned, badnick, inuse, badpass, regneeded };

loginhandler::loginhandler(connection* c, const string& qs)
{
  con = c;
  query_string = qs;
}

void loginhandler::run()
{
  try {

    string ip = con->getIP();
    if(yace->bans().matches(ip))
      throw banned;

    yace->bans().set(ip, yace->sql().getConfNum("min_logintime"));

    string nick = urlextract(query_string, "name");
    if(yace->bans().matches(nick))
      throw banned;

    if(!goodnick(nick))
      throw badnick;

    /* CHECK FOR IRC HERE */
    
    if(yace->users().existsUser(nick))
      throw inuse;

    string id = urlextract(query_string, "id");

    if(!yace->auths().allowed(nick, id, ip))
      throw badpass;
    

    bool needreg = yace->sql().getConfNum("onlyregistered");
    bool hasreg = false;

    hasreg = yace->sql().isReg(nick);

    if(needreg)
      if(!hasreg)
	throw regneeded;

    Semaphore wait;
    Semaphore logout;

    user* hehe = new user(con, &wait, &logout, nick, ip, id);
    

    hehe->ssetProp("logintime", c_time());

    string regstrings = yace->sql().getConfStr("regstrings");
    string regnums = yace->sql().getConfStr("regnums");

    {
      if(hasreg)
	yace->sql().updateTime(nick);

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
    
    string gotua;

    for(int i = 0; i < 20; ++i) {
      std::getline(*con, gotua);
      unsigned long tmp;
      if((tmp = (tolower(gotua)).find("user-agent: ")) != string::npos) {
	gotua = gotua.substr(tmp + 12);
	hehe->ssetProp("uagent", gotua);
	break;
      }
    }

    loginmutex.enterMutex();
    if(yace->users().existsUser(nick)) {
      loginmutex.leaveMutex();
      delete hehe;
      throw inuse;
    }
    
    yace->log("Login: " + hehe->getName() + " (" + hehe->getIP() + ")");

    hehe->IncRef();
    yace->users().insertUser(hehe);
    yace->rooms().joinRoom(hehe->getName(), yace->sql().getConfStr("stdroom"));
    //NEW IRC USER HERE
	newIRCUser(hehe);
	//NEW IRC USER HERE
	loginmutex.leaveMutex();

    //ok
    hehe->send(getHeader());
    hehe->send(yace->sql().getText("motd"));
    //ok

    enters(hehe);

    logout.wait();

    // quitIRCUser(hehe);

    leaves(hehe);
    yace->rooms().leaves(hehe->getName());
    yace->users().removeUser(hehe->getName());
    
    yace->log("Logout: " + hehe->getName());
    hehe->DecRef();
    
    wait.wait();
  }
  catch(loginerror e) {
    switch(e) {
    case banned:
      *con << getHeader();
      *con << yace->sql().getText("banned");
      break;
    case badnick:
      *con << getHeader();
      *con << yace->sql().getText("badnick");
      break;
    case inuse:
      *con << getHeader();
      *con << yace->sql().getText("inuse");
      break;
    case regneeded:
      *con << getHeader();
      *con << yace->sql().getText("onlyregistered");
      break;
    case badpass:
      *con << getHeader();
      *con << yace->sql().getText("badpass");
      break;
    }
    con->sync();
  }
  catch(...) {
  }
}
