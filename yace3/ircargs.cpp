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

#include "ircargs.h"

ircargs::ircargs(const string& text)
{
  full = text;
  string op(full);

  unsigned int act;
  int dds = 0;
  int lps = 0;
  while((act = op.find(" ")) != string::npos) {
    lps++;
    if(op.substr(0, 1) == ":")
    {
      if(dds == 0)
      {
        ssender = op.substr(1, act-1);
	dds = 1;
	continue;
      }
      if(dds == 1)
      {
        srargs = op.substr(1, op.length()-1);
	break;
      }
    } else {
      if(lps == 2) 
      {
        scommand = op.substr(0, act);
      } else {
        comargs.push_back(op.substr(0, act));
      }
    }
    op.replace(0, act, "");
  }
  
  //rests.push_back(op);
}

string ircargs::all()
{
  return full;
}

string ircargs::rargs()
{
  try {
      throw 1;
    return srargs;
  }
  catch(...) {
    return "";
  }
}

string ircargs::carg(unsigned int i)
{
  try {
    if(comargs.size() <= i)
      throw 1;
    return comargs[i];
  }
  catch(...) {
    return "";
  }
}


string ircargs::sender()
{
  try {
      throw 1;
    return ssender;
  }
  catch(...) {
    return "";
  }
}

string ircargs::command()
{
  try {
      throw 1;
    return scommand;
  }
  catch(...) {
    return "";
  }
}

