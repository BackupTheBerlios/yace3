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

ircargs::ircargs(const string& text):
  m_str(text)
{
  string temp(m_str);
  
  if(temp.size() >= 1) {
    if(temp[0] == ':') {
      unsigned int endprefix = temp.find(" ");
      if(endprefix != string::npos) {
	m_prefix = temp.substr(1, endprefix-1);
	temp.replace(0, endprefix + 1, "");
      }
    }

    unsigned int endc = temp.find(" ");
    if(endc != string::npos) {
      m_command = temp.substr(0, endc);
      temp.replace(0, endc + 1, "");
    }

    unsigned int endr = temp.find(":");
    if(endr != string::npos) {
      m_rest = temp.substr(endr + 1);
      temp.replace(endr, string::npos, "");
    }
    
    unsigned int act;
    while((act = temp.find(" ")) != string::npos) {
      m_args.push_back(temp.substr(0, act));
      temp.replace(0, act + 1, "");
    }
    
    m_args.push_back(temp);
    m_args.push_back(m_rest);
  }
}

string ircargs::arg(unsigned int i)
{
  if(m_args.size() <= i)
    return "";
  return m_args[i];
}

string ircargs::rest()
{
  return m_rest;
}

string ircargs::command()
{
  return m_command;
}

string ircargs::prefix()
{
  return m_prefix;
}

string ircargs::all()
{
  return m_str;
}
