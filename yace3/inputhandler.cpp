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

#include "inputhandler.h"
#include "hash_map.h"
#include "connection.h"
#include "stringutil.h"
#include "user.h"
#include "yace.h"

inputhandler::inputhandler(connection* c, const string& qs)
{
  con = c;
  query_string = qs;
}

void inputhandler::run()
{
  const string latinbuzz = "%C4%D6%DC";
  const string utf8buzz = "%C3%84%C3%96%C3%9C";

  string id = urlextract(query_string, "id");
  string ip = con->getIP();
  string nick = urlextract(query_string, "name");
  string input = urlextract(query_string, "input");
  string buzz = urlextract(query_string, "buzz");
  replaceI(nick, "sebi", "Affe");
  
  con->send(getNoContent());
  con->close();
   
  user* u = yace->users().getUser(nick);
  
  if(u == NULL)
    return;

  string r_id = u->getID();
  string r_ip = u->getIP();
  
  u->DecRef();
  u = NULL;

  if(r_id != id)
    return;
  
  if(r_ip != ip)
    return;

  input = urldecode(input);
  input = replace(input, "<", "&lt;");
  input = replace(input, ">", "&gt;");
 
  if(buzz == latinbuzz) {
    string origin(input);
    try {
    iconv("UTF-8", "ISO8859-1", input, origin);
    }
    catch(...) {
      input = origin;
    }
  }

  yace->iq().postInput(nick, input);
}
