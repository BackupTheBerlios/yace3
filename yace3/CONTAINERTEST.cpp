#include "roomcont.h"
#include "usercont.h"
#include "room.h"
#include "user.h"
#include <string>
#include <iostream>

using namespace std;

YaCE* yace;

void printRoomC(roomcont* rc)
{
  hash_map<string, room*> m = rc->getAllRooms();
  hash_map<string, room*>::iterator it = m.begin();
  cout << "ROOMS:" << endl;
  while(it != m.end()) {
    cout << it->second->getName() << " with: ";
    
    set<user*> u = it->second->getAllUsers();
    set<user*>::iterator it2 = u.begin();
    while(it2 != u.end()) {
      cout << (*it2)->getName() << ", ";
      ++it2;
    }

    cout << endl;
    ++it;
  }
}

void printUserC(usercont* uc)
{
  hash_map<string, user*> m = uc->getAllUsers();
  hash_map<string, user*>::iterator it = m.begin();

  cout << "USERS:" << endl;
  while(it != m.end()) {
    cout << it->second->getName() << " in " << it->second->getRoom()->getName() << endl;
    ++it;
  }
}

void shu()
{
  cout << "-----------" << endl;
}

roomcont* g_rc;
usercont* g_uc;

void foo()
{
  shu();
  printRoomC(g_rc);
  printUserC(g_uc);
  shu();
}


int main()
{
  roomcont rc;
  usercont uc;
  g_rc = &rc;
  g_uc = &uc;
  
  user u1(NULL, "GoGi", "GoGi");
  user u2(NULL, "Filb", "Filb");
  uc.insertUser(&u1);
  uc.insertUser(&u2);

  rc.joinRoom(&u2, "lounge");
  rc.joinRoom(&u1, "lounge");
  
  foo();

  rc.joinRoom(&u1, "foobar");

  foo();

  rc.joinRoom(&u2, "foobar");

  foo();

  rc.joinRoom(&u1, "ok");
  
  foo();
  
  rc.leaves(&u1);
  rc.leaves(&u2);

  foo(); //Segmentation fault is ok
  
}
