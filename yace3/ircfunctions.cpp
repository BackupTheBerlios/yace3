#include <sstream>
#include "stringutil.h"
#include "ircfunctions.h"
#include "room.h"
#include "user.h"
#include "yace.h"
#include "irccon.h"

string getChannel(const string& yaceroom)
{
  return yace->irc().getChannel(yaceroom);
}

void sendUserIRC(const string& user, const string& what)
{
  ostringstream toirc;
  
  ::user* u = yace->users().getUser(user);
  
  string tosendto = getChannel(u->getRoom());
  if(tosendto == "") return;
  
  toirc << ":" << user << " PRIVMSG " << tosendto << " :" << what;
  yace->irc().send(toirc.str());  
  return;
}

void newIRCUser(const string& who)
{
  ostringstream toirc;
  
  ::user* u = yace->users().getUser(who);
  
  string ircroom = getChannel(u->getRoom());
  if(ircroom == "") return;
  
  toirc << "NICK " << who << " 1 1 " << who << /* WHAT'S THIS??  <<  */ u->getIP() << " yace.filbboard.de 1 :Yace-User";
  yace->irc().send(toirc.str());  
  toirc.clear();

  toirc << ":" << who << " SETHOST yace.filbboard.de";
  yace->irc().send(toirc.str());
  toirc.clear();

  toirc << ":" << who << " JOIN " << ircroom;
  yace->irc().send(toirc.str());
  return;
}
