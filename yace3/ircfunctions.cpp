#include <sstream>
#include "stringutil.h"
#include "ircfunctions.h"
#include "yace.h"

string getIRCChannel(const string &yaceroom)
{
  return yace->irc().getIRCRoom(yace->rooms().getRoom(u->getRoom()));
}



void sendUserIRC(const string &user, const string &what)
{
  ostringstream toirc;
  
  user *u = yace->users().getUser(user);
  
  string tosendto = getIRCChannel(u->getRoom());
  if(tosendto == "") return;
  
  toirc << ":" << user << " PRIVMSG #" << tosendto << " :" << what;
  yace->irc().send(toirc.str());  
  return;
}

void newIRCUser(const string &who)
{
  ostringstream toirc;
  
  user *u = yace->users().getUser(who);
  
  string ircroom = getIRCChannel(u->getRoom());
  if(ircroom == "") return;
  
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
