#include <sstream>
#include "stringutil.h"
#include "ircfunctions.h"
#include "yace.h"

void sendUserIRC(const string &user, const string &what)
{
  ostringstream toirc;
  
  user *u = yace->users().getUser(user);
  room *r = yace->rooms().getRoom(u->getRoom());
  
  string tosendto = r->sgetProp("ircroom");
  if(tosendto == "none") return;
  
  toirc << ":" << user << " PRIVMSG #" << tosendto << " :" << what;
  yace->irc().send(toirc.str());  
  return;
}

void newIRCUser(const string &who)
{
  ostringstream toirc;
  
  user *u = yace->users().getUser(who);
  room *r = yace->rooms().getRoom(u->getRoom());
  
  string ircroom = r->sgetProp("ircroom");
  if(ircroom == "none") return;
  
  toirc << "NICK " << who << " 1 1 " << who <<  << u->getIP() << " foo 1 :Yace-User";
  yace->irc().send(toirc.str());  
  toirc.clear();

  toirc << ":" << who << " SETHOST yace.filbboard.de";
  yace->irc().send(toirc.str());
  toirc.clear();

  toirc << ":" << who << " JOIN " << ircroom;
  yace->irc().send(toirc.str());
  return;
}
