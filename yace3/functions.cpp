/*
  Copyright (C) 2003 Giuliano Gagliardi, Tobias Bahls, Giuliano Montecarlo, Thomas Yao

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

#include "functions.h"
#include "util.h"
#include "yace.h"
#include "stringutil.h"
#include "room.h"
#include "user.h"
#include "ircfunctions.h"
#include <sstream>
#include <iostream>

void enters(user* u)
{
  string snd = yace->sql().getString("enters");
  snd = replaceCommon(snd);
  snd = replaceUser(u->getName(), snd);
  sendRoom(u->getRoom(), snd);
}

void leaves(user* u)
{
  string snd = yace->sql().getString("leaves");
  snd = replaceCommon(snd);
  snd = replaceUser(u->getName(), snd);
  sendRoom(u->getRoom(), snd);
}

void returns(user* u)
{
  string snd = yace->sql().getString("back");
  snd = replaceCommon(snd);
  snd = replaceUser(u->getName(), snd);
  snd = replace(snd, "%TEXT%", u->sgetProp("away"));
  
  yace->irc().send(":" + u->getName() + " AWAY");
  u->ssetProp("away", "");
  sendRoomI(u->getName(), snd);
}

void sendUser(const string& who, const string& what, bool raw)
{
  string tosend;
  if(raw)
    tosend = what;
  else
    tosend = replace(yace->sql().getString("stdsend"), "%STUFF%", replaceAll(what));

  user* u = yace->users().getUser(who);
  if(u == NULL)
    return;

  u->send(tosend);
  u->DecRef();
}

void sendAll(const string& what, bool raw)
{
  string tosend;
  if(raw)
    tosend = what;
  else
    tosend = replace(yace->sql().getString("stdsend"), "%STUFF%", replaceAll(what));

  set<string> all = yace->users().getAllUsers();
  set<string>::iterator it = all.begin();
  for(;it != all.end();++it) {
    user* t = yace->users().getUser(*it);
    if(t == NULL)
      continue;
    t->send(tosend);
    t->DecRef();
  }
}

void sendR(int r, const string& what, bool raw)
{
  string tosend;
  if(raw)
    tosend = what;
  else
    tosend = replace(yace->sql().getString("stdsend"), "%STUFF%", replaceAll(what));

  set<string> all = yace->users().getAllUsers();
  set<string>::iterator it = all.begin();
  for(;it != all.end();++it) {
    user* t = yace->users().getUser(*it);
    if(t == NULL)
      continue;
    if(t->igetProp("rights") >= r)
      t->send(tosend);
    t->DecRef();
  }
}

void sendRoom(const string& room, const string& what, bool raw)
{
  try {
    string tosend;
    if(raw)
      tosend = what;
    else
      tosend = replace(yace->sql().getString("stdsend"), "%STUFF%", replaceAll(what));

    ::room* r;
    r = yace->rooms().getRoom(room);
    if(r == NULL) return;
	set<string> all = r->getAllUsers();
    r->DecRef();
    set<string>::iterator it = all.begin();
    for(;it != all.end();++it) {
      user* t = yace->users().getUser(*it);
      if(t == NULL || t->isIRC())
        continue;
      t->send(tosend);
      t->DecRef();
    }
  } catch(...) {
	  return;
  }
}

void sendRoomU(const string& user, const string& what, bool raw)
{
  ::user* u = yace->users().getUser(user);
  if(u == NULL)
    return;
  string r = u->getRoom();
  u->DecRef();

  sendRoom(r, what, raw);
}

string replaceCommon(const string& what)
{
  string ret(what);
  ret = replace(ret, "%TIME%", c_time());
  return ret;
}

string replaceUser(const string& user, const string& what, const string& suffix)
{
  ::user* u = yace->users().getUser(user);
  if(u == NULL)
    return what;

  string ret(what);
  string colstring = yace->sql().getString("colname");
  string colnick(colstring);
  
  colstring = replace(colstring, "%NAME%", u->getName());
  colstring = replace(colstring, "%COLOR%", u->sgetProp("color"));
  
  colnick = replace(colnick, "%NAME%", (u->sgetProp("nick") == "") ? u->getName() : u->sgetProp("nick"));
  colnick = replace(colnick, "%COLOR%", u->sgetProp("color"));

  ret = replace(ret, "%NAME" + suffix + "%", u->getName());
  ret = replace(ret, "%NICK" + suffix + "%", (u->sgetProp("nick") == "") ? u->getName() : u->sgetProp("nick"));
  ret = replace(ret, "%CNAME" + suffix + "%", colstring);
  ret = replace(ret, "%CNICK" + suffix + "%", colnick);
  ret = replace(ret, "%IP" + suffix + "%", u->getIP());

  ret = replace(ret, "%UAGENT" + suffix + "%", u->sgetProp("uagent"));

  ret = replace(ret, "%COLOR" + suffix + "%", u->sgetProp("color"));
  ret = replace(ret, "%ROOM" + suffix + "%", u->getRoom());
  ostringstream os;
  os << u->igetProp("rights");
  ret = replace(ret, "%RIGHTS" + suffix + "%", os.str());
  ret = replace(ret, "%LOGINTIME" + suffix + "%", u->sgetProp("logintime"));
  ostringstream os2;
  os2 << u->igetProp("silence");
  ret = replace(ret, "%SILENCE" + suffix + "%", os2.str());

  u->DecRef();
  return ret;
}

string getIPList()
{
  string ret;
  
  string i_top = yace->sql().getText("iplist_top");
  string i_user = yace->sql().getText("iplist_user");
  string i_bottom = yace->sql().getText("iplist_bottom");

  ret.append(i_top);

  set<string> users = yace->users().getAllUsers();
  for(set<string>::const_iterator it = users.begin(); it != users.end(); ++it) {
    user* u = yace->users().getUser(*it);
    string app = replaceUser(*it, i_user);
    ret.append(app);
    u->DecRef();
  }

  ret.append(i_bottom);
  return ret;
}

string getRoomListU(const string& user)
{
  ::user* u = yace->users().getUser(user);
  string r = u->getRoom();
  u->DecRef();
  return getRoomList(r);
}

string getRoomList(const string& room)
{
  ::room* r = yace->rooms().getRoom(room);
  if(r == NULL)
    return "";

  string ret;

  string r_top = yace->sql().getText("roomlist_top");
  string r_user = yace->sql().getText("roomlist_user");
  string r_user_norm = yace->sql().getText("roomlist_user_norm");
  string r_user_away = yace->sql().getText("roomlist_user_away");
  string r_bottom = yace->sql().getText("roomlist_bottom");

  ret.append(replace(r_top, "%ROOM%", r->getName()));
  set<string> members = r->getAllUsers();
  for(set<string>::const_iterator it = members.begin(); it != members.end(); ++it) {
    user* u = yace->users().getUser(*it);
    string line = replaceUser(*it, r_user);
    
    ostringstream os;
    os << "R_" << u->igetProp("rights") << "_R";
    string status = yace->sql().getText(os.str());
    line = replace(line, "%STATUS%", status);

    string silencing;
    if(u->sgetProp("away") == "")
      silencing = yace->sql().getText("roomlist_user_norm");
    else
      silencing = replace(yace->sql().getText("roomlist_user_away"), "%AWAY%", u->sgetProp("away"));
    
    silencing = replaceUser(*it, silencing);
    
    line = replace(line, "%SILENCING%", silencing);

    ret.append(line);

    u->DecRef();
  }
  ret.append(replace(r_bottom, "%ROOM%", r->getName()));
  r->DecRef();
  return ret;
}

string getChatList()
{
  string ret;

  string c_top = yace->sql().getText("chatlist_top");
  string c_room = yace->sql().getText("chatlist_room");
  string c_bottom = yace->sql().getText("chatlist_bottom");
  string c_topic = yace->sql().getText("chatlist_topic");
  string c_mod = yace->sql().getText("chatlist_mod");
  string c_locked = yace->sql().getText("chatlist_locked");

  ret.append(c_top);

  set<string> roomz = yace->rooms().getAllRooms();
  for(set<string>::const_iterator it = roomz.begin(); it != roomz.end(); ++it) {
    room* r = yace->rooms().getRoom(*it);
    if(r == NULL)
      continue;

    string line = replace(c_room, "%ROOM%", r->getName());
    if(r->igetProp("modtalk"))
      line = replace(line, "%MODERATED%", c_mod);
    else
      line = replace(line, "%MODERATED%", "");
    if(r->igetProp("locked"))
      line = replace(line, "%LOCKED%", c_locked);
    else
      line = replace(line, "%LOCKED%", "");
    if(r->sgetProp("topic") != "") {
      string topstr = replace(c_topic, "%TOPIC%", r->sgetProp("topic"));
      line = replace(line, "%TOPIC%", topstr);
    }
    else {
      line = replace(line, "%TOPIC%", "");
    }
    
    string memberlist;
    set<string> members = r->getAllUsers();
    for(set<string>::const_iterator ita = members.begin(); ita != members.end(); ++ita) {
      user* u = yace->users().getUser(*ita);
      if(u == NULL)
	continue;

      string ustring;
      int i = u->igetProp("rights");
      ostringstream os;
      os << "R_" << i << "_C";

      string rstring = yace->sql().getText(os.str());
      if(rstring != "")
	ustring = replace(rstring, "%STUFF%", u->getName());
      else
	ustring = u->getName();
      
      memberlist.append(ustring);
      memberlist.append(" ");
      u->DecRef();
    }

    line = replace(line, "%CHATTERS%", memberlist);
    ret.append(line);
    r->DecRef();
  }

  ret.append(c_bottom);
  return ret;
}

void sendRoomI(const string& user, const string& what, bool raw)
{
  ::user* u = yace->users().getUser(user);
  if(u == NULL)
    return;
  
  room* r = yace->rooms().getRoom(u->getRoom());
  if(r == NULL) { 
    u->DecRef(); 
    return;
  }

  string modsend = yace->sql().getString("modsend");

  set<string> members = r->getAllUsers();

  if(!u->igetProp("gag")) {

    bool cantalk = (u->igetProp("rights")) >= (r->igetProp("modtalk"));

    for(set<string>::iterator it = members.begin(); it != members.end(); ++it) {
      ::user* act = yace->users().getUser(*it);
      if(!ignores(*it, user)) {
	if(cantalk) {
	  sendUser(*it, what, raw);
	}
	else if(((act->igetProp("rights")) >= (r->igetProp("modhear"))) || (act == u)) {
	  sendUser(*it, replace(modsend, "%TEXT%", what), raw);
	}
      }
      act->DecRef();
    }
  }

  u->DecRef();
  r->DecRef();
}

void sendUserI(const string& who, const string& whom, const string& what, bool raw)
{
  if(!ignores(whom, who))
    sendUser(whom, what, raw);
}

void setRights(const string& who, int i)
{
  user* u = yace->users().getUser(who);
  if(u == NULL)
    return;
  u->isetProp("rights", i);
  u->DecRef();
}

void setRightsH(const string& who, int i)
{
  user* u = yace->users().getUser(who);
  if(u == NULL)
    return;
  if(u->igetProp("rights") < i)
    u->isetProp("rights", i);
  u->DecRef();
}

void setRightsL(const string& who, int i)
{
  user* u = yace->users().getUser(who);
  if(u == NULL)
    return;
  if(u->igetProp("rights") <= i)
    u->isetProp("rights", 0);
  u->DecRef();
}

bool hasRights(const string& who, int i, bool msg)
{
  user* u = yace->users().getUser(who);
  if (u == NULL) {
   return false;
  }
  if (u->igetProp("rights") < i) {
    if(msg) {
      ostringstream os;
      os << "R_" << i << "_R";
      string status = yace->sql().getText(os.str());

      if(status == "")
        sendUser(who, yace->sql().getString("no_rights_alt"));
      else
        sendUser(who, replace(yace->sql().getString("no_rights"), "%STATUS%", status));

      u->DecRef();
      return false;
    }
  }
  else {
    u->DecRef();
    return true;
  }
  return false;
}


string replaceAll(const string& in)
{
  string ret = in;
  hash_map<string, string> reps = yace->sql().getReplaces();
  for(hash_map<string, string>::iterator it = reps.begin(); it != reps.end(); ++it) {
	//ret = replaceI(ret, it->first, it->second);
	try {
	  while(tolower(ret).find(tolower(it->first)) != string::npos) 
	  {
	    //cout << "HIER REPLACE" << endl;
		ret = replaceI(ret, it->first, it->second);
	  }
    } catch(...) {
	  //cout << "FUCK!" << endl;
	  continue;
	}
  }
  //cout << "HIER REPLACE ENDE" << endl;
  return ret;
	//return  in;
}

string roomof(const string& in)
{
  user* u = yace->users().getUser(in);
  if(u == NULL)
    return "";
  
  string ret = u->getRoom();
  u->DecRef();
  return ret;
}

string inviteof(const string& in)
{
  user* u = yace->users().getUser(in);
  if(u == NULL)
    return "";
  
  string ret = u->sgetProp("lastinv");
  u->DecRef();
  return ret;
}

bool exists(const string& who, const string& errorto)
{
  if(yace->users().existsUser(who))
    return true;
  
  if(errorto != "")
    sendUser(errorto, yace->sql().getString("no_target"));
  
  return false;
}

void setTopic(const string& room, const string& what)
{
  ::room* r = yace->rooms().getRoom(room);
  if(r == NULL)
    return;
  
  r->ssetProp("topic", what);
  r->DecRef();
}

bool ignores(const string& who, const string& whom)
{
  user* u = yace->users().getUser(who);
  if(u == NULL)
    return false;

  bool ret;
  set<string> igs = u->xgetProp("ignores");
  if(igs.count(tolower(whom)))
    ret = true;
  else
    ret = false;

  u->DecRef();
  return ret;
}

void setColor(const string& who, const string& what)
{
  user* u = yace->users().getUser(who);
  if(u == NULL)
    return;
  u->ssetProp("color", what);
  
  if(yace->sql().isReg(who))
    yace->sql().setRegStr(who, "color", what);

  u->DecRef();
  return;
}

void addIgnore(const string& who, const string& whom)
{
  user* u = yace->users().getUser(who);
  if(u == NULL)
    return;

  
  set<string> igs = u->xgetProp("ignores");
  igs.insert(tolower(whom));
  u->xsetProp("ignores", igs);
  u->DecRef();
}

void delIgnore(const string& who, const string& whom)
{
  user* u = yace->users().getUser(who);
  if(u == NULL)
    return;

  set<string> igs = u->xgetProp("ignores");
  igs.erase(tolower(whom));
  u->xsetProp("ignores", igs);
  u->DecRef();
}

string getBanList()
{
  string ret;
  ret.append(yace->sql().getText("banlist_top"));
  string each = yace->sql().getText("banlist_each");
  string perm = yace->sql().getText("banlist_each_perm");

  hash_map<string, long> bans = yace->bans().getAll();
  for(hash_map<string, long>::iterator it = bans.begin(); it != bans.end(); ++it) {
    if(it->second == 86400) {
      string cur(perm);
      cur = replace(cur, "%NAME%", it->first);
      ret.append(cur);
    }
    else {
      ostringstream os;
      string cur(each);
      os << it->second;
      cur = replace(cur, "%NAME%", it->first);
      cur = replace(cur, "%SECONDS%", os.str());
      ret.append(cur);
    }
  }
  ret.append(yace->sql().getText("banlist_bottom"));
  return ret;
}

void gag(const string& s)
{
  user* u = yace->users().getUser(s);
  if(u == NULL)
    return;

  u->isetProp("gag", 1);
  u->DecRef();
}

void ungag(const string& s)
{
  user* u = yace->users().getUser(s);
  if(u == NULL)
    return;

  u->isetProp("gag", 0);
  u->DecRef();
}

bool gagged(const string& s)
{
  user* u = yace->users().getUser(s);
  if(u == NULL)
    return false;;

  bool ret;
  ret = u->igetProp("gag");
  u->DecRef();
  return ret;
}

bool sameRoom(const string& caller, const string& target)
{
  if(roomof(caller) == roomof(target))
    return true;

  sendUser(caller, replaceUser(target, yace->sql().getString("not_sameroom")));
  
  return false;
}

void setMod(const string& room, int talk, int hear)
{
  ::room* r = yace->rooms().getRoom(room);
  if(r == NULL)
    return;

  r->isetProp("modtalk", talk);
  r->isetProp("modhear", hear);
}

bool hasMod(const string& room)
{
  ::room* r = yace->rooms().getRoom(room);
  if(r == NULL)
    return false;
  
  bool ret = r->igetProp("modtalk");
  r->DecRef();
  return ret;
}

bool isAway(const string& who)
{
  user* u = yace->users().getUser(who);
  if(u == NULL)
    return false;

  bool ret;
  if(u->sgetProp("away") == "")
    ret = false;
  else
    ret = true;
  u->DecRef();
  return ret;
}

string getAway(const string& who)
{
  user* u = yace->users().getUser(who);
  if(u == NULL)
    return "";

  string ret = u->sgetProp("away");
  u->DecRef();
  return ret;
}

void setAway(const string& who, const string& reason)
{
  user* u = yace->users().getUser(who);
  if(u == NULL)
    return;
  
  u->ssetProp("away", reason);
  u->DecRef();
}

void lock(const string& rm)
{
  room* r = yace->rooms().getRoom(rm);
  if(r == NULL)
    return;

  r->isetProp("locked", 1);
  r->DecRef();
}

void unlock(const string& rm)
{
  room* r = yace->rooms().getRoom(rm);
  if(r == NULL)
    return;

  r->isetProp("locked", 0);
  r->DecRef();
}

bool locked(const string& rm)
{
  room* r = yace->rooms().getRoom(rm);
  if(r == NULL)
    return false;

  bool ret = r->igetProp("locked");
  r->DecRef();
  return ret;
}

void moveRoom(const string& usr, const string& rm)
{
  user* u = yace->users().getUser(usr);
  if(u == NULL)
    return;

  yace->rooms().joinRoom(usr, rm);
  
  u->DecRef();
}

void joinRoom(const string& usr, const string& rm, int rights)
{
  user* u = yace->users().getUser(usr);
  if(u == NULL)
    return;

  bool newflag = false;
  room* r = yace->rooms().getRoom(rm);
  if(r == NULL)
    newflag = true;
  else
    r->DecRef();

  moveRoom(usr, rm);
  if(newflag)
    setRightsH(usr, rights);
  else
    setRightsL(usr, rights);
}

bool isInvited(const string& room, const string& who)
{
  ::room* r = yace->rooms().getRoom(room);
  if(r == NULL)
    return false;

  set<string> inv = r->xgetProp("invites");
  r->DecRef();

  if(inv.count(tolower(who)))
    return true;
  else
    return false;
}

void addInvite(const string& room, const string& who)
{
  ::room* r = yace->rooms().getRoom(room);
  if(r == NULL)
    return;
  
  set<string> inv = r->xgetProp("invites");
  inv.insert(tolower(who));
  r->xsetProp("invites", inv);
  r->DecRef();
}

void setLastInv(const string& who, const string& room)
{
  user* u = yace->users().getUser(who);
  if(u == NULL)
    return;

  u->ssetProp("lastinv", room);
  u->DecRef();
}

bool canJoin(const string& room, const string& who)
{
  if(!locked(room))
    return true;
  if(isInvited(room, who))
    return true;

  sendUser(who, replace(yace->sql().getString("locked"),"%TEXT%",room));
  return false;
}

int quitUser(const string &user)
{
  ::user *u = yace->users().getUser(user);
  u->quit();
  u->DecRef();
  return 0;
}

int killUser(const string &user,const string who)
{
  ::user *u = yace->users().getUser(user);
	u->kill(who);
	u->DecRef();
	return 0;
}

string getIP(const string &user)
{
  ::user *u = yace->users().getUser(user);
  if(u == NULL) return "";
  string ip = u->getIP();
  u->DecRef();
  return ip;
}
