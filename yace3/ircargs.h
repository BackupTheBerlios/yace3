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

#ifndef _gogi_yace3_IRCARGS_H
#define _gogi_yace3_IRCARGS_H

#include <cc++/thread.h>
#include <string>
#include <vector>

#ifdef  CCXX_NAMESPACES
using namespace std;
#endif

class ircargs
{
  string full;
  string ssender;
  string srargs;
  string scommand;
  vector<string> comargs;

 public:
  ircargs(const string& text);
  string carg(unsigned int i);
  string rargs();
  string command();
  string sender();
  string all();
};

#endif
