#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <tuple>
#include <array>
#include <set>
#include <iostream>

using std::cin;
using std::cout;
using std::array;
using std::shared_ptr;
using std::unique_ptr;
using std::optional;
using std::nullopt;
using std::unordered_map;
using std::tuple;

inline constexpr std::nullopt_t null_optional{std::nullopt_t::_Tag{}};

struct hash_pair {
  template <class T1, class T2>
  size_t operator()(const std::pair<T1, T2>& p) const {
    auto hash1 = std::hash<T1>{}(p.first);
    auto hash2 = std::hash<T2>{}(p.second);

    if (hash1 != hash2) {
      return hash1 ^ hash2;
    }
    // If hash1 == hash2, their XOR is zero.
    return hash1;
  }
};

template <typename T>
inline T& tuple_get(auto& arg) {
  return std::get<T>(arg);
}