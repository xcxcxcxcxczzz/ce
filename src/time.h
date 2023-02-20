#pragma once
#include <chrono>

using namespace std::chrono_literals;

using chrono_clock = std::chrono::steady_clock;
using chrono_time = std::chrono::time_point<chrono_clock>;

using std::chrono::milliseconds;
using std::chrono::nanoseconds;
using std::chrono::seconds;

inline milliseconds makeStep(chrono_time& time_var) {
  auto current = chrono_clock::now();
  auto step = current - time_var;
  time_var = current;
  return std::chrono::duration_cast<milliseconds>(step);
}

