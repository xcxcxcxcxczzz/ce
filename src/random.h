#pragma once
#include <random>

class RandomGen {
 public:
  RandomGen() {

  }
  float Float() {
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(0, 1.0);
    return dis(gen);
  }
};