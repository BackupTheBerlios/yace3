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

#include <cc++/thread.h>
#include <iostream>
#include <string>

#ifdef  CCXX_NAMESPACES
using namespace std;
using namespace ost;
#endif

#include "connection.h"
#include "loginhandler.h"
#include "inputhandler.h"
#include "sourcehandler.h"
#include "authhandler.h"
#include "adminhandler.h"

connection::connection(TCPSocket &serv):
  TCPSession(serv, 1024, 0, 0)
{
}

void connection::run()
{
  setTimeout(5000);
  setError(true);
  //rdbuf(0);
  
  string firstline;
  try {
    std::getline(*tcp(), firstline);
    request_info got = ri(firstline);
    if(!got.ok)
      throw 1;

    if(got.service == "LOGIN") {
      authhandler hnd(this, got.query_string);
      hnd.run();
    }
    else if(got.service == "REALLOGIN") {
      loginhandler hnd(this, got.query_string);
      hnd.run();
    }
    else if(got.service == "INPUT") {
      inputhandler hnd(this, got.query_string);
      hnd.run();
    }
    else if(got.service == "SOURCE") {
      sourcehandler hnd(this, got.query_string);
      hnd.run();
    }
    else if(got.service == "ADMIN") {
      adminhandler hnd(this, got.query_string);
      hnd.run();
    }
      
  }
  catch(...) {}
  endStream();
}

void connection::close()
{
  mutex.enterMutex();
  this->sync();
//  this->flush();
  endStream();
  mutex.leaveMutex();
}

bool connection::send(const string& data)
{
  try {
    mutex.enterMutex();
    *tcp() << data;
//    this->flush();
    this->sync();
//    *tcp() << endl;
    bool ret = bool(*tcp());
    mutex.leaveMutex();
    return ret;
  }
  catch(...) {
    mutex.leaveMutex();
    return false;
  }
}

void connection::final()
{
  delete this;
}

connection::request_info connection::ri(const string& in)
{
  request_info ret;
  ret.ok = false;

  if(in.find("GET /") != 0)
    return ret;

  string work(in, 5);
  unsigned long qpos = 0;
  
  if((qpos = work.find_first_of("?/ ")) == string::npos) {
    ret.ok = true;
    ret.query_string = "";
    ret.service = work;
    return ret;
  }

  ret.ok = true;
  ret.service = work.substr(0, qpos);
  ret.query_string = work.substr(qpos);
  return ret;
}

string connection::getIP()
{
  mutex.enterMutex();
  string ret = getPeer().getHostname();
  mutex.leaveMutex();
  return ret;
}

bool connection::getl(string& buf)
{
  bool ret;
  mutex.enterMutex();
  ret = bool(std::getline(*tcp(), buf));
  mutex.leaveMutex();
  return ret;
}
