/*
  Copyright (C) 2003 Giuliano Gagliardi, Thomas Yao

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

#ifndef _gogi_yace3_STRINGUTIL_H
#define _gogi_yace3_STRINGUTIL_H

#include <string>
#include <cc++/thread.h>
#include <stdexcept>

#ifdef  CCXX_NAMESPACES
using namespace std;
using namespace ost;
#endif

string md5(const string& in);

string tolower(const string& str);
string toupper(const string& str);
string replace(const string& str, const string& pattern, const string& repl);
string replaceI(const string& str, const string& pattern, const string& repl);
string urlextract(const string& query_string, const string& key);

bool goodnick(const string& str);
bool consistsOf(const string& alphabet, const string& str);
string getHeader();
string getNoContent();

void iconv(const char* to_charset, const char* from_charset, std::string& target, const std::string& source) throw (std::runtime_error);
string urldecode(const string& in);
bool hostmatch(string pattern, string str);
string htmlspecialchars(const string& toedit);
string striphtml(const string& toedit);

#endif
