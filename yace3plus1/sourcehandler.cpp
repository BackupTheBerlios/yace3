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

#include "yace.h"
#include "sourcehandler.h"
#include "connection.h"
#include <fstream>

sourcehandler::sourcehandler(connection* c, const string& qs)
{
  con = c;
  query_string = qs;
}

void sourcehandler::run()
{
  
  string srcfile = yace->conf().get("sourcetarball");

  ifstream ins(srcfile.c_str(), ios::in | ios::binary);
  char c;
  
  (*con) << "HTTP/1.1 200 OK\r\n";
  (*con) << "Content-type: application/x-gzip\r\n\r\n";

  while(ins.get(c)) {
    (*con).put(c);
  }
  
}
