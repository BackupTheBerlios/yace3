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
#include "authhandler.h"
#include <cc++/misc.h>

authhandler::authhandler(connection* c, const string& qs)
{
  con = c;
  query_string = qs;
}

void authhandler::run()
{
  try {

    string ip = con->getIP();
    string nick = urlextract(query_string, "name");
    string id = urlextract(query_string, "id");
    string pass = urlextract(query_string, "pwd");


    if(goodnick(nick)) {
      
      bool auth = true;

      string md5pass;
      md5pass = md5(pass);

      if(yace->sql().isReg(nick))
	  {
	    if(md5pass != yace->sql().getRegStr(nick, "password"))
		{
	    	auth = false;
		}
	  }

      if(auth)
      {
	  	yace->auths().allow(nick, id, ip);
	  }
    }
    
    *con << "HTTP/1.1 302 Found\r\n";
    *con << "Server: YaCE 3\r\n";
    *con << "Cache-control: no-cache\r\n";
    *con << "Content-type: text/html\r\n";
    *con << "Location: /REALLOGIN?name=" + nick + "&id=" + id + "\r\n";
    *con << "1\r\n";
    *con << "\r\n";
    *con << "\r\n";
    *con << "\r\n";
    *con << "\r\n";
    *con << "\r\n";
    *con << "0\r\n";
    *con << "\r\n";
    con->sync();
  }
  catch(...) {
  }
}
