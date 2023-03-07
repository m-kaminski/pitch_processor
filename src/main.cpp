#include "base36.h"
#include <iostream>
#include <string>
#include <cassert>

using namespace std;
int main () {
  string B36_10("A");
  string B36_144("40");
  string B36_948437811("FOOBAR");
  string B36_MAX("ZZZZZZZZZZZZ");
  assert(pitchstream::base36(B36_10.begin(), B36_10.end()) == 10);
  assert(pitchstream::base36(B36_10) == 10);
  assert(pitchstream::base36(B36_144) == 144);
  assert(pitchstream::base36(B36_948437811) == 948437811);
  assert(pitchstream::base36(B36_MAX) == pitchstream::base36_max);
  assert(B36_MAX.size() == 12);
}


