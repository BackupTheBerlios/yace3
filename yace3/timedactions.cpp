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

#include "timedactions.h"
#include <iostream>
#include <set>
#include "yace.h"
#include "user.h"
#include "functions.h"

static int incrSilence()
{
  set<string> all = yace->users().getAllUsers();
  for(set<string>::iterator it = all.begin(); it != all.end(); ++it) {
    user* u = yace->users().getUser(*it);
    if(u == NULL)
      continue;
    u->incrProp("silence");
    u->DecRef();
  }
  return 0;
}

static int resetLines()
{
  set<string> all = yace->users().getAllUsers();
  for(set<string>::iterator it = all.begin(); it != all.end(); ++it) {
    user* u = yace->users().getUser(*it);
    if(u == NULL)
      continue;
    u->isetProp("said", 0);
    u->DecRef();
  }
  return 0;
}

static int keepAlive()
{
  sendAll("<!-- KA -->\n", true);
  return 0;
}

static int decrHKS()
{
  yace->bans().decrSeconds();
  return 0;
}

static int clearauth()
{
  yace->auths().clear();
  return 0;
}

void register_timedactions(timer::actionmap& m)
{
  m[clearauth] = 60;
  m[keepAlive] = 5;
  m[incrSilence] = 1;
  m[resetLines] = yace->sql().getConfNum("resetlines");
  m[decrHKS] = 1;
}
