/*
  Copyright (C) 2003 Giuliano Gagliardi, Giuliano Montecarlo, Thomas Yao

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

/*
  USERPROPS:
    STRING:
      color
      logintime
      away
    INT:
      rights
      silence
      said
      gag
    MAP:
      ignores

  ROOMPROPS:
    INT:
      modhear
      modtalk
      locked
    STRING:
      topic
    MAP:
      invites
      
*/

#ifndef _gogi_yace3_FUNCTIONS_H
#define _gogi_yace3_FUNCTIONS_H

#include <string>
#include <cc++/thread.h>
#include "user.h"

#ifdef  CCXX_NAMESPACES
using namespace std;
#endif

string replaceCommon(const string& what);
string replaceUser(const string& user, const string& what, const string& suffix="");

void enters(user* u);
void leaves(user* u);
void returns(user* u);

string getRoomList(const string& room);
string getRoomListU(const string& user);
string getChatList();
string getIPList();
string getBanList();

void sendAll(const string& what, bool raw=false);
void sendRoom(const string& room, const string& what, bool raw=false);
void sendRoomU(const string& user, const string& what, bool raw=false);
void sendRoomI(const string& user, const string& what, bool raw=false);
void sendR(int r, const string& what, bool raw=false);
void sendUser(const string& whom, const string& what, bool raw=false);
void sendUserI(const string& who, const string& whom, const string& what, bool raw=false);

void moveRoom(const string& u, const string& r);
void joinRoom(const string& usr, const string& rm, int rights);

void setRights(const string& who, int i);
void setRightsH(const string& who, int i);
void setRightsL(const string& who, int i);
bool hasRights(const string& who, int i, bool msg=true);
void setColor(const string& who, const string& what);
void ungag(const string& s);
void gag(const string& s);
bool gagged(const string& s);
void setAway(const string& who, const string& reason);
bool isAway(const string& who);
string getAway(const string& who);
bool ignores(const string& who, const string& whom);
void addIgnore(const string& who, const string& whom);
void delIgnore(const string& who, const string& whom);
void setLastInv(const string& who, const string& room);

void setMod(const string& room, int talk, int hear);
bool hasMod(const string& r);
void setTopic(const string& room, const string& what);
bool isInvited(const string& room, const string& who);
void addInvite(const string& room, const string& who);
bool canJoin(const string& room, const string& who);

bool exists(const string& who, const string& errorto = "");
string replaceAll(const string& in);
string roomof(const string& user);
string inviteof(const string& in);
bool sameRoom(const string& caller, const string& target);
string getIP(const string &user);

void lock(const string& r);
void unlock(const string& r);
bool locked(const string& r);

int quitUser(const string &user);
int killUser(const string &user, const string who);
#endif
