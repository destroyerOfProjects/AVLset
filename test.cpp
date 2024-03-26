#include "avl.h"

void Test() {
  Set<int> s;

  s.insert(3);
  s.insert(1);
  s.insert(0);
  auto it = s.lower_bound(0);
  std::cout << "it " << *it << "\n";
  it = s.lower_bound(1);
  std::cout << "it " << *it << "\n";
  it = s.lower_bound(2);
  std::cout << "it " << *it << "\n";
  it = s.lower_bound(4);
  assert(it == s.end());

  assert(s.size() == 3);
  s.insert(2);
  s.insert(2);
  assert(s.size() == 4);
  s.insert(13);
  s.erase(7);
  s.Print();
}

int main() {
  Test();
}

/*
12
+ 3
+ 1
+ 0
? 0
> 2
> 4
<
+ 2
+ 2
<
+ 13
> 7
*/