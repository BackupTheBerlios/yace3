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

#include "commands.h"
#include "yace.h"
#include "functions.h"
#include "util.h"
#include "stringutil.h"
#include "roomcont.h"
#include "ircfunctions.h"
#include <iostream>
#include <sstream>

static const int admin = 200;
static const int superglobal = 100;
static const int global = 50;
static const int local = 25;
static const int voice = 10;

static int wc(CA)
{
  sendUser(caller, getChatList());
  return 0;
}

static int w(CA)
{
  sendUser(caller, getRoomListU(caller));
  return 0;
}

static int say(CA)
{
  string s = yace->sql().getString("say");
  s = replaceUser(caller, s);
  s = replace(s, "%TEXT%", args.all());
  sendRoomI(caller, s);
  return 0;
}

static int t(CA)
{
  if(!hasRights(caller, local)) return 0;

  string msg = yace->sql().getString("settopic");
  msg = replaceCommon(msg);
  msg = replaceUser(caller, msg);
  msg = replace(msg, "%TEXT%", args.all());

  sendRoomU(caller, msg);
  setTopic(roomof(caller), args.all());
  yace->irc().send(":" + caller + " TOPIC " + getChannel(roomof(caller)) + " :" + args.all());
  return 0;
}

static int rt(CA)
{
  if(!hasRights(caller, local)) return 0;

  string msg = yace->sql().getString("deltopic");
  msg = replaceCommon(msg);
  msg = replaceUser(caller, msg);
  sendRoomU(caller, msg);
  setTopic(roomof(caller), "");
	yace->irc().send(":" + caller + " TOPIC " + getChannel(roomof(caller)) + " :");
  return 0;
}

static int m(CA)
{
  // IRC
   sendIRCUserPrivate(caller,args.rest(0),args.arg(0));
    
  
  // YaCE
  if(!exists(args.arg(0))) {
    string err = yace->sql().getString("unknown_command");
    err = replace(err, "%TEXT%", args.arg(0));
    sendUser(caller, err);
    return 0;
  }

  string tocaller = replaceUser(args.arg(0), yace->sql().getString("whisperto"));
  string touser = replaceUser(caller, yace->sql().getString("whisperfrom"));

  if(isAway(args.arg(0))) {
    string notice = yace->sql().getString("whisper_away");
    notice = replaceUser(args.arg(0), notice);
    notice = replace(notice, "%TEXT%", getAway(args.arg(0)));
    sendUser(caller, notice);
  }

  tocaller = replace(tocaller, "%TEXT%", args.rest(0));
  touser = replace(touser, "%TEXT%", args.rest(0));
  sendUser(caller, tocaller);
  sendUserI(caller, args.arg(0), touser);
  
  return 0;
}

static int shutdown(CA)
{
  if(!hasRights(caller, admin)) return 0;
  sendAll(yace->sql().getText("shutdown"));
  exit(0);
  return 0;
}

static int sendall(CA)
{
  sendAll(args.all());
  return 0;
}

static int show_list(CA)
{
  if(!hasRights(caller, global)) return 0;
  sendUser(caller, getIPList());
  return 0;
}

static int hks(CA)
{
  if(!hasRights(caller, global)) return 0;
  sendUser(caller, getBanList());
  return 0;
}

static int hkset(CA)
{
  if(!hasRights(caller, global)) return 0;

  long toset = 0;
  istringstream is(args.arg(1));
  is >> toset;

  yace->bans().set(args.arg(0), toset);

  return 0;
}

static int hkclear(CA)
{
  if(!hasRights(caller, global)) return 0;

  yace->bans().clear();
  return 0;
}

static int reloadsql(CA)
{
  if(!hasRights(caller, admin)) return 0;
  
  yace->sql().wipeCache();
  return 0;
}

static int ig(CA)
{
  if(!exists(args.arg(0), caller)) return 0;

  string tocaller;
  string totarget;

  if(ignores(caller, args.arg(0))) {
    tocaller = yace->sql().getString("unignore_sender");
    totarget = yace->sql().getString("unignore_target");
    delIgnore(caller, args.arg(0));
  }
  else {
    tocaller = yace->sql().getString("ignore_sender");
    totarget = yace->sql().getString("ignore_target");
    addIgnore(caller, args.arg(0));
  }
  
  tocaller = replaceUser(args.arg(0), tocaller);
  totarget = replaceUser(caller, totarget);
  sendUser(caller, tocaller);
  sendUserI(caller, args.arg(0), totarget);
  return 0;
}

static int col(CA)
{
  string color = toupper(args.arg(0));
  const string valid = "ABCDEF0123456789";
  if(consistsOf(valid, color) && (color.length() == 6))
  {
    setColor(caller, color);
    sendRoomI(caller, replaceUser(caller, yace->sql().getString("colorchange")));
    return 0;
  } 
  return 0;
}

static int gag(CA)
{
  string target = args.arg(0);

  if(!hasRights(caller, local)) return 0;
  if(!exists(target, caller)) return 0;
  if(!sameRoom(caller, target)) return 0;

  string notice;

  if(gagged(target)) {
    ungag(target);
    notice = yace->sql().getString("un_gag");
  }
  else {
    gag(target);
    notice = yace->sql().getString("gag");
  }

  notice = replaceUser(caller, notice, "-C");
  notice = replaceUser(target, notice, "-T");
  notice = replaceCommon(notice);

  sendRoomU(caller, notice);

  return 0;
}

static int mod(CA)
{
  if(!hasRights(caller, local)) return 0;

  string notice;
  if(hasMod(roomof(caller))) {
    notice = yace->sql().getString("remove_mod");
    setMod(roomof(caller), 0, 0);
		yace->irc().send(":" + caller + " MODE " + getChannel(roomof(caller)) + " -m");
  }
  else {
    notice = yace->sql().getString("mod");
    setMod(roomof(caller), voice, local);
		yace->irc().send(":" + caller + " MODE " + getChannel(roomof(caller)) + " +m");
  }

  notice = replaceUser(caller, notice);
  notice = replaceCommon(notice);

  sendRoomU(caller, notice);
  
  return 0;
}

static int away(CA)
{
  if(args.all() == "")
    return 0;

  y2i_away(caller, args.getPlain());
  string notice = yace->sql().getString("away");
  notice = replaceCommon(notice);
  notice = replaceUser(caller, notice);
  notice = replace(notice, "%TEXT%", args.all());
  
  setAway(caller, args.all());
  sendRoomI(caller, notice);

  return 0;
}

static int l(CA)
{
  ostringstream tosend;
  if(!hasRights(caller, local)) return 0;
  string room = roomof(caller);

  string notice;
  if(locked(room)) {
    notice = yace->sql().getString("unlock_room");
    unlock(room);
    tosend << ":" << caller << " MODE " << getChannel(room) << " -i";
    yace->irc().send(tosend.str());
  }
  else {
    notice = yace->sql().getString("lock_room");
    lock(room);
    tosend << ":" << caller << " MODE " << getChannel(room) << " +i";
    yace->irc().send(tosend.str());
  }
  
  notice = replaceCommon(notice);
  notice = replaceUser(caller, notice);

  sendRoomU(caller, notice);

  return 0;
}

static int kc(CA)
{
  if(!hasRights(caller, admin)) return 0;
  if(!exists(args.arg(0))) return 0;
  
  string notice = yace->sql().getString("kickchat");
  notice = replaceCommon(notice);
  notice = replaceUser(args.arg(0), notice);
  notice = replace(notice, "%TEXT%", args.rest(0));
  
  sendUser(caller, notice);
  moveRoom(args.arg(0), args.rest(0));
  return 0;
}

static int i_(CA)
{
  if(!hasRights(caller, local)) return 0;
  if(!exists(args.arg(0), caller)) return 0;

  string noticeself = yace->sql().getString("invite_sender");
  string noticeuser = yace->sql().getString("invite_target"); 

  noticeself = replaceCommon(noticeself);
  noticeuser = replaceCommon(noticeuser);
  noticeself = replaceUser(caller, noticeself);
  noticeuser = replaceUser(caller, noticeuser);
  noticeself = replaceUser(args.arg(0), noticeself, "-I");

  setLastInv(args.arg(0), roomof(caller));
  addInvite(roomof(caller), args.arg(0));
  sendUser(caller, noticeself);
  sendUserI(caller, args.arg(0), noticeuser);
  return 0;
}

static int j(CA)
{
  if(!canJoin(args.all(), caller)) return 0;
  
  string old = yace->sql().getString("leave_room");
  old = replaceCommon(old);
  old = replaceUser(caller, old);
  old = replace(old, "%TEXT%", args.all());
  
  string neway = yace->sql().getString("join_room");
  neway = replaceCommon(neway);
  neway = replaceUser(caller, neway);
  neway = replace(neway, "%TEXT%", roomof(caller));
  
  sendRoomU(caller, old);
  yace->irc().send(":" + caller + " PART " + replace("#" + roomof(caller), " ", "_") + " :joining " + replace("#" + args.all(), " ", "_"));
  joinRoom(caller, args.all(), local);
  yace->irc().send(":" + caller + " JOIN " + replace("#" + args.all(), " ", "_"));
  sendRoomU(caller, neway);

  return 0;
}

static int me(CA)
{
  string tosend = yace->sql().getString("me");
  ostringstream os;
  os << ":" << caller << " PRIVMSG #yace :" << (char)1 << "ACTION " << args.getPlain() << (char)1;
  yace->irc().send(os.str());
  tosend = replaceUser(caller, tosend);
  tosend = replace(tosend, "%TEXT%", args.all());
  sendRoomI(caller, tosend);
  return 0;
}

static int amsg(CA)
{
  if(!hasRights(caller, admin)) return 0;
  sendAll(replace(yace->sql().getText("admin_msg"), "%TEXT%", args.all()));
  return 0;
}

static int aq(CA)
{
  string tosend = replaceUser(caller, yace->sql().getString("question"));
  tosend = replaceUser(caller, tosend);
  tosend = replace(tosend, "%TEXT%", args.all());
  sendRoomI(caller, tosend);
	yace->irc().send(":" + caller + " PRIVMSG " + getChannel(roomof(caller)) + " :" + (char)1 + "ACTION Asks: " + args.all() + char(1));
		
  return 0;
}

static int chgcol(CA)
{
  if(!hasRights(caller, global)) return 0;
  if(!exists(args.arg(0), caller)) return 0;
  string color = toupper(args.arg(1));
  const string valid = "ABCDEF0123456789";
  if(consistsOf(valid, color) && (color.length() == 6))
  {
    string tosend = replaceUser(caller, yace->sql().getString("changecolor"), "-S");
    tosend = replaceUser(args.arg(0), tosend, "-R");
    tosend = replace(tosend, "%TEXT%", color);
    setColor(args.arg(0), color);
    tosend = replaceCommon(tosend);

    sendRoomU(caller, tosend);
    return 0;
  } 
  return 0;
}

static int hk(CA)
{
  if(!hasRights(caller, global)) return 0;
  if(!exists(args.arg(0), caller)) return 0;
  string tosend = replaceUser(caller, yace->sql().getString("hardkick"), "-S");
  tosend = replaceUser(args.arg(0), tosend, "-R");
  tosend = replaceCommon(tosend);
  sendRoomU(caller, tosend);
  killUser(args.arg(0), caller);
	// yace->irc().send(":yace.filbboard.de KILL " + args.arg(0) + " :YaCE-hk by " + caller + ": " + args.rest(0));
  return 0;
}

static int k(CA)
{
  if(!hasRights(caller, local)) return 0;
  if(!exists(args.arg(0), caller)) return 0;
  if(!canJoin(args.rest(0), args.arg(0))) return 0;

  string tosend = replaceUser(caller, yace->sql().getString("kick_leave"), "-S");
  tosend = replaceUser(args.arg(0), tosend, "-R");
  tosend = replace(tosend, "%TEXT%", args.rest(0));
  tosend = replaceCommon(tosend);

  string neway = yace->sql().getString("kick_join");
  neway = replaceCommon(neway);
  neway = replaceUser(caller, neway, "-S");
  neway = replaceUser(args.arg(0), neway, "-R");
  neway = replace(neway, "%TEXT%", roomof(args.arg(0)));

  sendRoomU(caller, tosend);
  joinRoom(args.arg(0), args.rest(0), local);
  sendRoomU(args.arg(0), neway);
  return 0;
}

static int rq(CA)
{
  string tosend = replaceUser(caller, yace->sql().getString("answer"));
  tosend = replaceUser(caller, tosend);
  tosend = replace(tosend, "%TEXT%", args.all());
	yace->irc().send(":" + caller + " PRIVMSG " + getChannel(roomof(caller)) + " :" + (char)1 + "ACTION Answers: " + args.all() + char(1));
		
  sendRoomI(caller, tosend);
  return 0;
}

static int q(CA)
{
  quitUser(caller);
  return 0;
}

static int kill(CA)
{
  if(!hasRights(caller, admin)) return 0;
  if(!exists(args.arg(0), caller)) return 0;
  string tosend = replaceUser(args.arg(0), yace->sql().getString("kill"), "-R");
  tosend = replaceUser(caller, tosend, "-S");
  tosend = replaceCommon(tosend);
  sendAll(tosend);
  quitUser(args.arg(0));
  return 0;
}
 
static int su(CA)
{
  if(!hasRights(caller, global)) return 0;
  if(!exists(args.arg(0), caller)) return 0;
  string tosend = yace->sql().getString("giverights");

  ostringstream os;
  os << "R_" << local << "_R";

  tosend = replace(tosend, "%RIGHTS%", yace->sql().getText(os.str()));

  tosend = replaceUser(args.arg(0), tosend, "-R");
  tosend = replaceUser(caller, tosend, "-S");
  tosend = replaceCommon(tosend);
  setRightsH(args.arg(0), local);
  sendRoomU(args.arg(0), tosend);
  return 0;
}

static int rsu(CA)
{
  if(!hasRights(caller, global)) return 0;
  if(!exists(args.arg(0), caller)) return 0;
  string tosend = yace->sql().getString("takerights");

  ostringstream os;
  os << "R_" << local << "_R";
  tosend = replace(tosend, "%RIGHTS%", yace->sql().getText(os.str()));
  
  tosend = replaceUser(args.arg(0), tosend, "-R");
  tosend = replaceUser(caller, tosend, "-S");
  tosend = replaceCommon(tosend);
  setRightsL(args.arg(0), local);
  sendRoomU(args.arg(0), tosend);
  return 0;
}

static int voice_c(CA)
{
  if(!hasRights(caller, local)) return 0;
  if(!exists(args.arg(0), caller)) return 0;
  string tosend = yace->sql().getString("giverights");

  ostringstream os;
  os << "R_" << voice << "_R";
  tosend = replace(tosend, "%RIGHTS%", yace->sql().getText(os.str()));
  tosend = replaceUser(args.arg(0), tosend, "-R");
  tosend = replaceUser(caller, tosend, "-S");

  tosend = replaceCommon(tosend);
  setRightsH(args.arg(0), voice);
  sendRoomU(args.arg(0), tosend);
  return 0;
}

static int rvoice(CA)
{
  if(!hasRights(caller, local)) return 0;
  if(!exists(args.arg(0), caller)) return 0;
  string tosend = yace->sql().getString("takerights");

  ostringstream os;
  os << "R_" << voice << "_R";
  tosend = replace(tosend, "%RIGHTS%", yace->sql().getText(os.str()));
  tosend = replaceUser(args.arg(0), tosend, "-R");
  tosend = replaceUser(caller, tosend, "-S");

  tosend = replaceCommon(tosend);
  setRightsL(args.arg(0), voice);
  sendRoomU(args.arg(0), tosend);
  return 0;
}

static int tmpright(CA)
{
  // tmpright and chgrights are still the same, yet. (no registry)
  if(!hasRights(caller, admin)) return 0;
  if(!exists(args.arg(0), caller)) return 0;
  string tosend = yace->sql().getString("tmpright");
  tosend = replaceUser(args.arg(0), tosend, "-R");
  tosend = replaceUser(caller, tosend, "-S");
  tosend = replaceCommon(tosend);
  istringstream is(args.arg(1));
  int ri = 0;
  is >> ri;
  setRights(args.arg(0), ri);
  sendRoomU(args.arg(0), tosend);
  return 0;
}

static int video(CA)
{
  if(!hasRights(caller, admin)) return 0;
  string tosend = replaceUser(caller, yace->sql().getString("video"));
  tosend = replace(tosend, "%TEXT%", args.arg(0));
  sendRoomU(caller, tosend);
	yace->irc().send(":" + caller + " PRIVMSG " + getChannel(roomof(caller)) + " :" + (char)1 + "ACTION Video: " + args.all() + char(1));	
  return 0;
}
  
static int sound(CA)
{
  if(!hasRights(caller, global)) return 0;
  string tosend = replaceUser(caller, yace->sql().getString("sound"));
  tosend = replace(tosend, "%TEXT%", args.arg(0));
  tosend = replaceUser(caller, tosend);
  sendRoomU(caller, tosend);
  return 0;
}

static int sepa(CA)
{
  if(!canJoin(args.all(), caller)) return 0;
  
  string old = yace->sql().getString("leave_room_lock");
  old = replaceCommon(old);
  old = replaceUser(caller, old);
  old = replace(old, "%TEXT%", args.all());
  string neway = yace->sql().getString("join_room");
  neway = replaceCommon(neway);
  neway = replaceUser(caller, neway);
  neway = replace(neway, "%TEXT%", roomof(caller));

  sendRoomU(caller, old);
  joinRoom(caller, args.all(), local);
  
  string notice = yace->sql().getString("lock_room");
  notice = replaceCommon(notice);
  notice = replaceUser(caller, notice);

  lock(roomof(caller));
  
  sendRoomU(caller, neway);
  sendRoomU(caller, notice);

  return 0;
}

static int cat(CA)
{
  if(!canJoin(args.all(), caller)) return 0;
  if(!hasRights(caller, global)) return 0;
  if(!exists(args.arg(0), caller)) return 0;  

  string old = yace->sql().getString("catch_leave");
  old = replaceCommon(old);
  old = replaceUser(caller, old, "-S");
  old = replaceUser(args.arg(0), old, "-R");
  old = replace(old, "%TEXT%", args.rest(0));
  string neway = yace->sql().getString("catch_join");
  neway = replaceCommon(neway);
  neway = replaceUser(caller, neway, "-S");
  neway = replaceUser(args.arg(0), neway, "-R");
  neway = replace(neway, "%TEXT-S%", roomof(caller));
  neway = replace(neway, "%TEXT-R%", roomof(args.arg(0)));

  sendRoomU(caller, old);
  joinRoom(caller, args.rest(0), local);
  joinRoom(args.arg(0), args.rest(0), local);
  sendRoomU(caller, neway);

  return 0;
}

static int s(CA)
{
  string tosend = yace->sql().getString("shout");
  tosend = replaceUser(caller, tosend);
  tosend = replace(tosend, "%TEXT%", args.all());
  sendRoomI(caller, tosend);
	yace->irc().send(":" + caller + " PRIVMSG " + getChannel(roomof(caller)) + " :" + (char)1 + "ACTION Shouts: " + args.all() + char(1));
		
  return 0;
}

static int p(CA)
{
  if(!hasRights(caller, global)) return 0;
  string tosend = yace->sql().getString("picture");
  tosend = replaceUser(caller, tosend);
  tosend = replace(tosend, "%TEXT%", args.all());
	yace->irc().send(":" + caller + " PRIVMSG " + getChannel(roomof(caller)) + " :" + (char)1 + "ACTION Picture: " + args.all() + char(1));
		
  sendRoomU(caller, tosend);
  return 0;
}

static int vipmsg(CA)
{
  if(!hasRights(caller, global)) return 0;
  string tosend = yace->sql().getString("vip_msg");
  tosend = replaceCommon(tosend);
  tosend = replaceUser(caller, tosend);
  tosend = replace(tosend, "%TEXT%", args.all());
  sendAll(tosend);
  return 0;
}

static int mvip(CA)
{
  if(!hasRights(caller, global)) return 0;
  string tosend = yace->sql().getString("whisper_vip");
  tosend =  replaceUser(caller, tosend);
  tosend = replace(tosend, "%TEXT%", args.all());
  sendR(global, tosend);
  return 0;
}
  
static int a(CA)
{
  if(!canJoin(args.all(), caller)) return 0;
  if(inviteof(caller) == "") return 0;
  string old = yace->sql().getString("leave_room");
  old = replaceCommon(old);
  old = replaceUser(caller, old);
  old = replace(old, "%TEXT%", inviteof(caller));
  string neway = yace->sql().getString("join_room");
  neway = replaceCommon(neway);
  neway = replaceUser(caller, neway);
  neway = replace(neway, "%TEXT%", roomof(caller));

  sendRoomU(caller, old);
  joinRoom(caller, inviteof(caller), local);
  sendRoomU(caller, neway);

  return 0;
}

static int hkban(CA)
{
  if(!hasRights(caller, global)) return 0;
  if(!exists(args.arg(0), caller)) return 0;
  long toset = 0;
  istringstream is(args.arg(1));
  is >> toset;
  yace->bans().set(args.arg(0), toset);
  
  string tosend = replaceUser(caller, yace->sql().getString("hardkick"), "-S");
  string tosend2 = replaceUser(args.arg(0), yace->sql().getString("hardkick_banned"));
  tosend2 = replaceCommon(tosend2);
  tosend = replaceUser(args.arg(0), tosend, "-R");
  tosend = replaceCommon(tosend);
  sendRoomU(caller, tosend);
  sendRoomU(caller, tosend2);
  quitUser(args.arg(0));
  return 0;
}

static int hkip(CA)
{
  if(!hasRights(caller, global)) return 0;
  if(!exists(args.arg(0), caller)) return 0;
  long toset = 0;
  istringstream is(args.arg(1));
  is >> toset;
  yace->bans().set(getIP(args.arg(0)), toset);
  
  string tosend = replaceUser(caller, yace->sql().getString("hardkick"), "-S");
  string tosend2 = replaceUser(args.arg(0), yace->sql().getString("hardkick_banned"));
  tosend2 = replaceCommon(tosend2);
  tosend = replaceUser(args.arg(0), tosend, "-R");
  tosend = replaceCommon(tosend);
  sendRoomU(caller, tosend);
  sendRoomU(caller, tosend2);
  quitUser(args.arg(0));
  return 0;
}

static int time(CA) 
{
  sendUser(caller, replaceCommon(yace->sql().getString("time")));
  return 1;
}

static int url(CA) 
{
  string tosend;
  tosend = yace->sql().getString("url");
  tosend = replaceCommon(tosend);
  tosend = replace(tosend, "%URL%", args.all());
  tosend = replaceUser(caller,tosend);
	yace->irc().send(":" + caller + " PRIVMSG " + getChannel(roomof(caller)) + " :" + (char)1 + "ACTION URL: " + args.all() + char(1));
  sendRoomU(caller, tosend);
  return 0;
}

static int hkmax(CA)
{
  if(!hasRights(caller, global)) return 0;
  if(!exists(args.arg(0), caller)) return 0;
  long toset = 86400;
  yace->bans().set(getIP(args.arg(0)), toset);
  yace->bans().set(args.arg(0), toset);
  
  string tosend = replaceUser(caller, yace->sql().getString("hardkick"), "-S");
  string tosend2 = replaceUser(args.arg(0), yace->sql().getString("hardkick_banned"));
  tosend2 = replaceCommon(tosend2);
  tosend = replaceUser(args.arg(0), tosend, "-R");
  tosend = replaceCommon(tosend);
  sendRoomU(caller, tosend);
  sendRoomU(caller, tosend2);
  quitUser(args.arg(0));
  return 0;
}

static int nick(CA)
{
    ostringstream oss;
    string nick = striphtml(args.all());
    yace->users().getUser(caller)->ssetProp("nick", nick);
    string tosend;
    tosend = yace->sql().getString("nick");
    tosend = replaceCommon(tosend);
    tosend = replace(tosend, "%NICK%", nick);
    tosend = replaceUser(caller, tosend);
    sendRoomU(caller, tosend);
		//  FIXME: User gets killed after the 2nd Time
    oss << ":" << caller << " NICK " << nick << " :" << time(NULL);
		yace->irc().send(oss.str());
    return 0;
}

static int changenick(CA)
{
    ostringstream oss;
    if(!hasRights(caller, superglobal)) return 0;
    string target = args.arg(0);
    if(!exists(target, caller)) return 0;

    yace->users().getUser(target)->ssetProp("nick", args.arg(1));
    string tosend;
    tosend = yace->sql().getString("changenick");
    tosend = replaceCommon(tosend);
    tosend = replaceUser(caller, tosend, "-C");
    tosend = replaceUser(target, tosend, "-T");
    sendRoomU(caller, tosend);
		/* FIXME: Doesnt work for IRC Users
		oss << ":" << target << " NICK " << args.arg(1) << " :" << time(NULL);
		yace->irc().send(oss.str());
		 */
    return 0;
}

static int restorenick(CA)
{
    yace->users().getUser(caller)->ssetProp("nick", "");
    string tosend;
    tosend = yace->sql().getString("nick");
    tosend = replaceCommon(tosend);
    tosend = replace(tosend, "%NICK%", yace->users().getUser(caller)->getName());
    tosend = replaceUser(caller, tosend);
    sendRoomU(caller, tosend);
    return 0;
}

static int givenickrights(CA)
{
    if(!hasRights(caller, superglobal)) return 0;
    if(!yace->sql().isReg(args.arg(0))) return 0;

    yace->sql().setRegNum(args.arg(0), "nickrights", 1);
    string tosend;
    tosend = yace->sql().getString("hasnickrights");
    tosend = replaceCommon(tosend);
    tosend = replace(tosend, "%TNICK%", args.arg(0));
    tosend = replaceUser(caller, tosend);
    sendUser(caller, tosend);
    return 0;
}

static int connectRC(CA)
{
	yace->irc().connectRC(args.arg(0), args.arg(1));
	return 0;
}

#include "commands2.cpp"

void register_commands(inputqueue::commandmap& i)
{
  // commands2 - will be put into a so later!
  i["knuddel"] = hug;
  i["kuss"] = kiss;
  // commands2

  //i["gimme_power"] = gimme_power;
 
  i["sendall"] = sendall;
  i["shutdown"] = shutdown;
  i["kc"] = kc;
  i["amsg"] = amsg;
  i["vipmsg"] = vipmsg;

  i["w"] = w;
  i["wc"] = wc;
  i["list"] = show_list;
  i["say"] = say;
  i["m"] = m;
  i["me"] = me;
  i["aq"] = aq;
  i["rq"] = rq;
  i["s"] = s;
  i["a"] = a;
  
  i["mod"] = mod;
  i["t"] = t;
  i["rt"] = rt;
  i["l"] = l;
  i["i"] = i_;
  i["j"] = j;
  i["k"] = k;
  
  i["sepa"] = sepa;
  i["cat"] = cat;  
  i["gag"] = gag;

  i["kill"] = kill;
  i["chgcol"] = chgcol;
  i["hks"] = hks;
  i["hkset"] = hkset;
  i["hkclear"] = hkclear;
  i["hkban"] = hkban;
  i["hkip"] = hkip;
  i["khmax"] = hkmax;
  i["mvip"] = mvip;

  i["reloadsql"] = reloadsql;
  i["hk"] = hk;

  i["away"] = away;
  i["ig"] = ig;
  i["col"] = col;
  i["q"] = q;

  i["su"] = su;
  i["rsu"] = rsu;
  i["voice"] = voice_c;
  i["rvoice"] = rvoice;
  i["tmpright"] = tmpright;
  i["chgrights"] = tmpright;
  
  i["video"] = video;
  i["sound"] = sound;
  i["p"] = p;
  i["time"] = time;
  i["url"] = url;
  
  i["nick"] = nick;
  i["chgnick"] = changenick;
  i["rnick"] = restorenick;
  i["cchgnick"] = givenickrights;
  i["crc"] = connectRC;
}
