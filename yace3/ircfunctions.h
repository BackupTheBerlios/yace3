#include <sstream>
#include "stringutil.h"
#include "yace.h"


string getIRCChannel(const string &yaceroom);

void sendUserIRC(const string &user, const string &what);
void newIRCUser(const string &who);
