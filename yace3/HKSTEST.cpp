#include "hkstable.h"
#include <iostream>

using namespace std;

int main()
{
  hkstable t;
  
  t.set("1", 1);
  t.set("2", 2);
  t.set("3", 3);


  cout << t.get("0") << endl;
  cout << t.get("1") << endl;
  cout << t.get("2") << endl;
  cout << t.get("3") << endl;
 
  t.decrSeconds();
  
    cout << t.get("0") << endl;
  cout << t.get("1") << endl;
  cout << t.get("2") << endl;
  cout << t.get("3") << endl;
 
  t.decrSeconds();
  
  cout << t.get("0") << endl;
  cout << t.get("1") << endl;
  cout << t.get("2") << endl;
  cout << t.get("3") << endl;
 
  t.decrSeconds();
  
  cout << t.get("0") << endl;
  cout << t.get("1") << endl;
  cout << t.get("2") << endl;
  cout << t.get("3") << endl;
 
  t.decrSeconds();
  
  
  
}
