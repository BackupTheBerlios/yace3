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

#include <iostream>
#include "stringutil.h"
#include "connection.h"
#include "yace.h"
#include "user.h"
#include "room.h"
#include "util.h"
#include "roomcont.h"
#include "usercont.h"
#include "adminhandler.h"
#include "functions.h"
#include "adminactions.h"
#include <cc++/digest.h>
#include <cc++/misc.h>

adminhandler::actionmap adminhandler::actions;

void adminhandler::load_actions()
{
  register_adminactions(actions);
}

adminhandler::adminhandler(connection* c, const string& qs)
{
  con = c;
  query_string = qs;
}

void adminhandler::run()
{
  const unsigned int some_error = 0;

  try {
    *con << "HTTP/1.1 200 OK\r\n";
    *con << "Server: YaCE 3\r\n";
    *con << "Cache-control: no-cache\r\n";
    *con << "Content-type: text/plain\r\n";
    *con << "\r\n";

    string gotpass = urlextract(query_string, "auth");
    gotpass = urldecode(gotpass);

    if(gotpass != yace->sql().getConfStr("adminauth"))
      throw some_error;

    string action = urlextract(query_string, "action");
     
    if(!actions.count(action))
      throw some_error;

    adminaction toinvoke = actions[action];
    toinvoke(*con, query_string);
  }
  catch(...) {
  }
}
