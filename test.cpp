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
  s.print();
}

void Stress() {
  Set<int> s;
  const int N = 1e6;
  for (size_t i = 0; i < N; i++) {
    s.insert(N - i);
  }
  assert(s.size() == N);
  for (size_t i = 0; i < N; i++) {
    s.erase(N - i);
  }
  assert(s.empty());
}

int main() {
  Test();
  Stress();
}
