#include <sstream>
#include "stringutil.h"
#include "irccon.h"
#include "functions.h"
string getChannel(const string& yaceroom);

void sendUserIRC(const string& user, const string& what);
void sendIRCUserPrivate(const string& user, const string& what, const string& to);

void newIRCUser(user* u);
void quitIRCUser(user* u);

/* FIXME
void i2y_say(const string& who, const string& what, const string& where, const string& color ="da1337");
void i2y_whisper(const string& who, const string& what, const string& user, const string& color ="da1337");
*/
void i2y_say(const string& who, const string& what, const string& where);
void i2y_whisper(const string& who, const string& what, const string& user);
