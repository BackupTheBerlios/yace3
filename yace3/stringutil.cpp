/*
  Copyright (C) 2003 Giuliano Gagliardi, Thomas Yao, Giuliano Montecarlo

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
#define SEBISUCKS
#define YACERULAY
#define DONTEDIT
#define KICKMICROSOFT
string getHeader()
{
  string ret;
  // IF YOU EDIT SOME OF THIS STUFF, YOU BRAKE LAW! WE'LL FIND YOU AND KICK YOUR ASS.
  // The YaCE-Frame don't scroll if you edit this, btw.
  ret.append("H" DONTEDIT "TT" NIHIL "P/1" YACERULAY ".1 200" NIHIL " O" KICKMICROSOFT "K\r\n");
  ret.append("Se" NIHIL "rv" DONTEDIT "er: Ya" NIHIL "CE" YACERULAY " 3" KICKMICROSOFT "\r\n");
  ret.append("C" KICKMICROSOFT "ache" NIHIL "-cont" DONTEDIT "rol: no-c" NIHIL "ac" KICKMICROSOFT "he\r\n");
  ret.append("Co" KICKMICROSOFT "nt" NIHIL "en" DONTEDIT "t-ty" NIHIL "pe: " NIHIL "tex" YACERULAY "t/h" KICKMICROSOFT "tml\r\n");
  ret.append("" NIHIL "\r" YACERULAY "\n");
  ret.append("\r" NIHIL "\n");


  ret.append("<H" NIHIL "TML" YACERULAY ">\n");
  ret.append("<HE" NIHIL "AD>\n");
  ret.append("<me" NIHIL "t" DONTEDIT "a " YACERULAY "ht" NIHIL "tp-eq" DONTEDIT "uiv=\"C" NIHIL "on" KICKMICROSOFT "te" KICKMICROSOFT "nt" YACERULAY "-Type\" con" KICKMICROSOFT "t" DONTEDIT "ent" NIHIL "=\"te" KICKMICROSOFT "x" DONTEDIT "t/" YACERULAY "ht" KICKMICROSOFT "ml; c" DONTEDIT "ha" KICKMICROSOFT "rs" YACERULAY "et" KICKMICROSOFT "=UT" NIHIL "F-" KICKMICROSOFT "8\">\n");
  ret.append("<TI" NIHIL "TLE>Ya" NIHIL "CE</T" NIHIL "I" KICKMICROSOFT "TLE>\n");
  ret.append("<SC" NIHIL "R" DONTEDIT "IP" KICKMICROSOFT "T" NIHIL " LA" KICKMICROSOFT "NG" YACERULAY "UAG" DONTEDIT "E=\"" NIHIL "Jav" KICKMICROSOFT "a" DONTEDIT "Sc" KICKMICROSOFT "ri" YACERULAY "p" KICKMICROSOFT "t\" T" DONTEDIT "Y" KICKMICROSOFT "P" KICKMICROSOFT "E=\"tex" KICKMICROSOFT "t/" NIHIL "ja" KICKMICROSOFT "v" YACERULAY "ascri" NIHIL "p" KICKMICROSOFT "t\">\n");
  ret.append("<!--\n");
  ret.append("" NIHIL "s" KICKMICROSOFT "cr" DONTEDIT "ol" KICKMICROSOFT "li" YACERULAY "ng" NIHIL "=tr" NIHIL "ue;\n");
  ret.append("fun" NIHIL "cti" SEBISUCKS "on mo" NIHIL "ve" KICKMICROSOFT "sn" DONTEDIT "ew(){\n");
  ret.append("if (" NIHIL "sc" KICKMICROSOFT "r" YACERULAY "oll" KICKMICROSOFT "ing !" NIHIL "= fa" DONTEDIT "lse) {wind" KICKMICROSOFT "ow" YACERULAY ".scroll(1, 5" NIHIL "0" DONTEDIT "0000);}\n");
  ret.append("wi" YACERULAY "nd" YACERULAY "ow." KICKMICROSOFT "set" NIHIL "Tim" KICKMICROSOFT "eou" KICKMICROSOFT "t(\"m" YACERULAY "oves" DONTEDIT "new" NIHIL "()\", 100);\n");
  ret.append("}" NIHIL "\n");
  ret.append("" NIHIL "move" DONTEDIT "sn" DONTEDIT "ew();\n");
  ret.append("//-" NIHIL "-" KICKMICROSOFT ">" DONTEDIT "\n");
  ret.append("</" NIHIL "SC" KICKMICROSOFT "R" DONTEDIT "IPT>\n");
  ret.append("</H" NIHIL "E" KICKMICROSOFT "A" DONTEDIT "D>\n");
  ret.append("<B" DONTEDIT "O" KICKMICROSOFT "DY " KICKMICROSOFT "BGC" KICKMICROSOFT "OLO" NIHIL "R=\"#f" YACERULAY "fff" KICKMICROSOFT "ff\" AL" DONTEDIT "INK=\"#000" NIHIL "000\" L" KICKMICROSOFT "IN" DONTEDIT "K=\"#000" YACERULAY "00" KICKMICROSOFT "0\" VL" DONTEDIT "INK=\"" SEBISUCKS "#00" KICKMICROSOFT "00" DONTEDIT "0\" on" YACERULAY "Foc" KICKMICROSOFT "us=\"s" SEBISUCKS "croll" DONTEDIT "ing = fa" DONTEDIT "lse\" o" SEBISUCKS "nBl" KICKMICROSOFT "ur=\"sc" KICKMICROSOFT "ro" NIHIL "lling = true\" on" DONTEDIT "Mouse" SEBISUCKS "Over=\"" SEBISUCKS "scr" DONTEDIT "olling = fa" DONTEDIT "lse\" " SEBISUCKS "onMouse" YACERULAY "Out=\"scr" YACERULAY "olling = true\">\n");
  ret.append("<tab" SEBISUCKS "l" KICKMICROSOFT "e bo" DONTEDIT "rd" KICKMICROSOFT "er=\"1\" ce" DONTEDIT "llp" SEBISUCKS "addin" NIHIL "g=\"2\" ce" KICKMICROSOFT "l" DONTEDIT "lspac" DONTEDIT "ing" KICKMICROSOFT "=\"" SEBISUCKS "0\" " YACERULAY "s" KICKMICROSOFT "ty" DONTEDIT "le" KICKMICROSOFT "=\"pad" KICKMICROSOFT "din" DONTEDIT "g-left: 5" YACERULAY "px; bor" DONTEDIT "der-col" DONTEDIT "lapse:" NIHIL " coll" DONTEDIT "aps" KICKMICROSOFT "e\" wi" DONTEDIT "dth=\"625\">" DONTEDIT "<tr><t" DONTEDIT "d st" DONTEDIT "yle=\"width: 1" DONTEDIT "00%\"><a hr" DONTEDIT "ef=\"/SO" YACERULAY "U" SEBISUCKS "RCE/sou" DONTEDIT "rce.tar.gz\" " NIHIL "sty" YACERULAY "le=\"text" YACERULAY "-decora" YACERULAY "tion: no" DONTEDIT "ne; color: " NIHIL "#00" DONTEDIT "00" YACERULAY "00;\" tar" YACERULAY "get=\"_bla" YACERULAY "nk\">\n");
  ret.append("<s" DONTEDIT "pan" KICKMICROSOFT " st" NIHIL "y" KICKMICROSOFT "le=\"" SEBISUCKS "fo" DONTEDIT "nt-" SEBISUCKS "family: Ta" DONTEDIT "homa, " NIHIL "Ar" KICKMICROSOFT "i" DONTEDIT "al, Time" SEBISUCKS "s" NIHIL " N" KICKMICROSOFT "ew R" DONTEDIT "om" KICKMICROSOFT "a" KICKMICROSOFT "n; fo" DONTEDIT "nt-" KICKMICROSOFT "siz" KICKMICROSOFT "e: 8pt" KICKMICROSOFT "\"><b><sp" DONTEDIT "an styl" DONTEDIT "e=\"color: #00" DONTEDIT "99FF\">" NIHIL "YaC" DONTEDIT "E 3" NIHIL "</spa" DONTEDIT "n" DONTEDIT "> " NIHIL VERSION NIHIL " - " DONTEDIT "Co" YACERULAY "pyrig" DONTEDIT "ht &" SEBISUCKS "copy; 2003" YACERULAY " Giuli" DONTEDIT "ano G" SEBISUCKS "aglia" DONTEDIT "rdi, T" YACERULAY "obias Bah" DONTEDIT "ls, Giuliano " DONTEDIT "" SEBISUCKS "Monteca" DONTEDIT "rlo, Tho" DONTEDIT "mas Yao" DONTEDIT "</b>" NIHIL "<br />Than" SEBISUCKS "ks " DONTEDIT "to: Mat" DONTEDIT "t" YACERULAY "hias Bu" DONTEDIT "tz, " NIHIL "Dani" DONTEDIT "el v.Do" YACERULAY "mbro" DONTEDIT "wski</span>");
  ret.append("</" KICKMICROSOFT "a" SEBISUCKS "><br" KICKMICROSOFT "><s" SEBISUCKS "pan sty" KICKMICROSOFT "le=" YACERULAY "\"font" KICKMICROSOFT "-fam" KICKMICROSOFT "il" SEBISUCKS "y: Tah" SEBISUCKS "oma, Ari" NIHIL "al, Ti" KICKMICROSOFT "mes " YACERULAY "Ne" KICKMICROSOFT "w R" KICKMICROSOFT "oman, " NIHIL "Time" KICKMICROSOFT "s; fon" SEBISUCKS "t-si" KICKMICROSOFT "ze: 7p" KICKMICROSOFT "t\"></sp" KICKMICROSOFT "an>" SEBISUCKS "</t" YACERULAY "d></" YACERULAY "tr></ta" YACERULAY "ble>\n");
  return ret;
}
#undef NIHIL
#undef SEBISUCKS
#undef YACERULAY
#undef DONTEDIT
#undef KICKMICROSOFT

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
