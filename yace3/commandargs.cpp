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

#include "commandargs.h"

commandargs::commandargs(const string& text)
{
  full = text;
  string op(full);

  unsigned int act;
  while((act = op.find(" ")) != string::npos) {
    args.push_back(op.substr(0, act));
    rests.push_back(op.substr(act + 1));
    op.replace(0, act + 1, "");
  }
  args.push_back(op);
  //rests.push_back(op);
}

string commandargs::all()
{
  return full;
}

string commandargs::rest(unsigned int i)
{
  try {
    if(rests.size() <= i)
      throw 1;
    return rests[i];
  }
  catch(...) {
    return "";
  }
}

string commandargs::arg(unsigned int i)
{
  try {
    if(args.size() <= i)
      throw 1;
    return args[i];
  }
  catch(...) {
    return "";
  }
}
