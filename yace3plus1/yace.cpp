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

#include <cc++/socket.h>
#include <iostream>
#include <cstdlib>

#ifdef CCXX_NAMESPACES
using namespace ost;
using namespace std;
#endif

#include "util.h"
#include "yace.h"
#include "connection.h"
#include "adminhandler.h"

YaCE::YaCE(const string& cfgfile):
  cfg(cfgfile)
{
}

YaCE::~YaCE()
{
}

void YaCE::log(const string& add)
{
  m_log.enterMutex();
  (*logfilep) << "[" << c_logdate() << "] " << add << endl;
  m_log.leaveMutex();
}

void YaCE::run()
{
  cout << "+--------YaCE3--------" << endl;
  cout << "+" << endl;
  cout << "+---Testing Port...";
  int port = atoi(cfg.get("port").c_str());
  if(port==0) {
    cout << "ERROR" << endl;
    exit(1);
  }
  cout << "OK" << endl;
  
  string host = cfg.get("mysql_host");
  string user = cfg.get("mysql_user");
  string pass = cfg.get("mysql_pass");
  string db = cfg.get("mysql_db");
  string logfile = cfg.get("logfile");
  
  try {
  cout << "+---Connecting MySQL...";
  sqlc = new sqlcon(db, host, user, pass);
  }
  catch(...) {
    cout << "ERROR" << endl;
    exit(1);
  }
  cout << "OK" << endl;
  
  try {
    cout << "+---Opening Logfile...";
    logfilep = new ofstream(logfile.c_str(), ios::out | ios::app);
    if(!(*logfilep))
      throw 1;
  }
  catch(...) {
    cout << "ERROR" << endl;
    exit(1);
  }
  cout << "OK" << endl;
  log("Starting up...");

  
  inqu.init();
  thetimer.init();

  inqu.start();
  thetimer.start();

  adminhandler::load_actions();

  // Starting irc, if needed
  
  if(cfg.get("irc") == "1") {
    cout << "Connecting to IRC(" << cfg.get("irc_host") << ")...";
    int ircport = atoi(cfg.get("irc_port").c_str());
    
    try {
      ic = new irccon(cfg.get("irc_host"), ircport, cfg.get("irc_name"), cfg.get("irc_password"));
      ic->start();
      //icp->init();
    } catch(...) {
      cout << "ERROR" << endl;
      exit(1);
    }
    
    cout << "OK" << endl;
  }
  else {
    ic = new irccon("localhost", 0, "NIHIL", "NIHIL");
    //icp = NULL;
  }
  // end irc

  try {
    cout << "+---Initialize Sockets...";
    InetAddress inull;
    TCPSocket server(inull, port);
    connection* nextcon;
    
    log("Listening on port " + cfg.get("port"));
    cout << "OK" << endl;
    cout << "+" << endl;
    cout << "+---Started sucessfully!" << endl;
    while(server.isPendingConnection()) {
      nextcon = new connection(server);
      nextcon->start();
    }
  }
  catch(Socket* sock) {
    cout << "ERROR (Port " << port << ")"<< endl;
  }
}
