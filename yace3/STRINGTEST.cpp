#include <string>
#include <iostream>
#include "stringutil.h"
#include "commandargs.h"
#include <cc++/misc.h>
#include <cc++/digest.h>

using namespace ost;
using namespace std;

#include "yace.h"
YaCE* yace;

int main()
{

  /*  
  cout << tolower("qweQWEqweQWE") << endl;
  cout << toupper("qweQWEqweQWE") << endl;
  cout << replace("AaaaaaA", "a", "b") << endl; //hissukk0rsuck

  cout << string("hilfehallo").find("lol") << endl;
  

  cout << urlextract("?okay=lol&rofl=yahoay&zuzu=as", "rofl") << endl;
  

  commandargs a("hi hallo test ich bins");

  cout << a.all() << endl;
  for(int i = 0; i < 7; i++) {
    cout << "arg " << i << ": " << a.arg(i) << endl;
    cout << "rst " << i << ": " << a.rest(i) << endl;
  }
  */

  MD5Digest dig;
  dig << "pasdasd";
  cout << dig << endl;


}
