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

#include "stringutil.h"
#include <cc++/urlstring.h>
#include <iostream>
#include <sstream>
#include <iconv.h>
#include <fnmatch.h>
#include <errno.h>
#include <cc++/digest.h>

#define VERSION "v0.00 - Beta 1 (GOGIUS PIGER EST)"

string md5(const string& in)
{
  ostringstream ret;
  MD5Digest dig;
  dig << in;
  ret << dig;
  return ret.str();
}

string tolower(const string& strd)
{
  string str(strd);

  string::iterator it = str.begin();
  while(it != str.end()) {
    *it = tolower(*it);

    ++it;
  }

  return str;
}

string toupper(const string& strd)
{
  string str(strd);

  string::iterator it = str.begin();
  while(it != str.end()) {
    *it = toupper(*it);
    
    ++it;
  }

  return str;
}

string replace(const string& str, const string& pattern, const string& repl)
{
  string op(str);
  unsigned long plen = pattern.length();
  unsigned long replen = repl.length();
  unsigned long lastpos = 0;
  unsigned long f = 0;
  while( (f = op.find(pattern, lastpos)) != string::npos ) {
    op.replace(f, plen, repl);
    lastpos = f + replen;
  }
  
  return op;
}

string replaceI(const string& str, const string& pattern, const string& repl)
{
  string op(tolower(str));
  string toret(str);
  unsigned long plen = pattern.length();
  unsigned long replen = repl.length();
  unsigned long lastpos = 0;
  unsigned long f = 0;
  while( (f = op.find(pattern, lastpos)) != string::npos ) {
    toret.replace(f, plen, repl);
    lastpos = f + replen;
  }

  return toret;
}

string urlextract(const string& query_string, const string& key)
{
  unsigned long pos;
  string rawqs;
  if((pos = query_string.find("?")) != string::npos) {
    rawqs = query_string.substr(pos + 1);
  }
  else {
    rawqs = query_string;
  }
  
  if((pos = rawqs.find(key + "=")) == string::npos)
    return "";

  long startpos = pos + key.length() + 1;
  if((pos = rawqs.find_first_of("& ", startpos)) == string::npos)
    return rawqs.substr(startpos);

  long len = pos - startpos;
  return rawqs.substr(startpos, len);

}

static inline bool good(const string& alphabet, const char& c)
{
  if(alphabet.find(c) != string::npos)
    return true;
  return false;
}

bool goodnick(const string& str)
{
  if(str.length() < 3)
    return false;

  if(!consistsOf("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-123456789", str))
      return false;
  
  return true;
}

bool consistsOf(const string& alphabet, const string& str)
{
  string::const_iterator it = str.begin();
   while(it != str.end()) {
     if(!good(alphabet, *it))
      return false;
    ++it;
  }
  return true;
}

string getNoContent()
{
  string ret;
  ret.append("HTTP/1.1 204 No Content\r\n");
  ret.append("Server: YaCE 3\r\n");
  ret.append("\r\n");
  return ret;
}


#define NIHIL
string getHeader()
{
  string ret;
  ret.append("HTTP/1.1 200 OK\r\n");
  ret.append("Server: YaCE 3\r\n");
  ret.append("Cache-control: no-cache\r\n");
  ret.append("Content-type: text/html\r\n");
  ret.append("\r\n");
  ret.append("\r\n");


  ret.append("<HTML>\n");
  ret.append("<HEAD>\n");
  ret.append("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n");
  ret.append("<TITLE>YaCE</TITLE>\n");
  ret.append("<SCRIPT LANGUAGE=\"JavaScript\" TYPE=\"text/javascript\">\n");
  ret.append("<!--\n");
  ret.append("scrolling=true;\n");
  ret.append("function movesnew(){\n");
  ret.append("if (scrolling != false) {window.scroll(1, 500000);}\n");
  ret.append("window.setTimeout(\"movesnew()\", 100);\n");
  ret.append("}\n");
  ret.append("movesnew();\n");
  ret.append("//-->\n");
  ret.append("</SCRIPT>\n");
  ret.append("</HEAD>\n");
  ret.append("<BODY BGCOLOR=\"#ffffff\" ALINK=\"#000000\" LINK=\"#000000\" VLINK=\"#00000\" onFocus=\"scrolling = false\" onBlur=\"scrolling = true\" onMouseOver=\"scrolling = false\" onMouseOut=\"scrolling = true\">\n");
  ret.append("<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" style=\"padding-left: 5px; border-collapse: collapse\" width=\"625\"><tr><td style=\"width: 100%\"><a href=\"/SOURCE/source.tar.gz\" style=\"text-decoration: none; color: #000000;\" target=\"_blank\">\n");
  ret.append("<span style=\"font-family: Tahoma, Arial, Times New Roman; font-size: 8pt\"><b><span style=\"color: #0099FF\">" NIHIL "YaCE 3" NIHIL "</span> " NIHIL VERSION NIHIL " - Copyright &copy; 2003 Giuliano Gagliardi, Tobias Bahls, Giuliano Montecarlo, Thomas Yao</b><br />Thanks to: Matthias Butz, Daniel v.Dombrowski</span>");
  ret.append("</a><br><span style=\"font-family: Tahoma, Arial, Times New Roman, Times; font-size: 7pt\"></span></td></tr></table>\n");
  return ret;
}
#undef NIHIL

void iconv(const char* to_charset, const char* from_charset, std::string& target, const std::string& source) throw (std::runtime_error)
{
  iconv_t cd(::iconv_open(to_charset, from_charset));
  if (cd == reinterpret_cast<iconv_t>(-1) )
    throw std::runtime_error("iconv_open failed");
  size_t s_len(source.size());
  char* s_ptr(const_cast<char*>(source.data()));
  size_t max_o_len = s_len * 5;
  size_t o_len(max_o_len);
  char* const buf = new char[o_len];
  char* o_ptr(buf);
  ::iconv(cd, &s_ptr, &s_len, &o_ptr, &o_len);
//  if ((s_len != 0) || errno) {
//    int sys_err(errno);
 //   throw std::runtime_error(std::string(source) +
//			     ": Dv::Xml::iconv from " + from_charset + " to " + to_charset + " failed [" +
//			     (sys_err ? strerror(sys_err) : "") + "]"
//			     );
//  }
  target.resize(0);
  target.append(buf, max_o_len - o_len);
  delete [] buf;
  iconv_close(cd);
}

string urldecode(const string& in)
{
  /* sucks
  char* recvbuf = new char[in.length()];
  urlDecode(const_cast<char*>(in.c_str()), recvbuf); //risky? bad CommonC++ prototype!
  string ret(recvbuf);
  delete [] recvbuf;
  return ret;
  */

  string ret;
  string hex;
  string::const_iterator it = in.begin();
  while(it != in.end()) {
    switch(*it) {
    case '+':
      ret.push_back(' ');
      ++it;
      break;
    case '%':
      hex = "";
      ++it;
      if(it != in.end()) {
	hex.push_back(*it);
	++it;
	if(it != in.end()) {
	  hex.push_back(*it);
	  long r = strtol(hex.c_str(), NULL, 16);
	  if((0 < r) && (r < 256))
	    ret.push_back(r);
	  ++it;
	}
      }
      break;
    default:
      ret.push_back(*it);
      ++it;
    }
  }
  return ret;
}

bool hostmatch(string pattern, string str)
{
  int ret = fnmatch(pattern.c_str(), str.c_str(), FNM_NOESCAPE | FNM_CASEFOLD);
  return (!bool(ret));
}

string htmlspecialchars(const string& toedit)
{
    string toret = toedit;
    toret = replace(toret, "&", "&amp;");
    toret = replace(toret, "\"", "&quot;");
    toret = replace(toret, "'", "&#039;");
    toret = replace(toret, "<", "&lt;");
    toret = replace(toret, ">", "&gt;");
    return toret;
}

string striphtml(const string& toedit)
{
    string toret;
    bool html = false;
    for(string::const_iterator it = toedit.begin(); it != toedit.end(); ++it)
    {
        if(html)
        {
            if(*it != '>')
                continue;
            else
            {
                html = false;
                continue;
            }
        }
        if(*it == '<')
        {
            html = true;
            continue;
        }

        toret.push_back(*it);
    }
    return toret;
}
