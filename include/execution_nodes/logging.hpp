#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <strstream>
#include <utility>
#include <vector>

namespace execution_nodes {

struct Warning {};

extern Warning warning;

template <typename T> Warning &operator<<(Warning &s, const T &x) {
  std::cout << "[Warning] " <<  x;
  return s;
}

/*

struct Log {

  template <class... A> static void error(A &&...a) {
    errorImpl(
        {[&a](std::strstream &ststr) { ststr << std::forward<A>(a); }...});
  }

  template <class... A> static void warning(A &&...a) {

      warningImpl(
        {[&a](std::strstream &ststr) { ststr << std::forward<A>(a); }...});

  }

  template <class... A> static void throwError(A &&...a) {
    throwErrorImpl(
        {[&a](std::strstream &ststr) { ststr << std::forward<A>(a); }...});
  }

private:
  static void errorImpl(std::vector<std::function<void(std::strstream &)>> v);
  static void
  throwErrorImpl(std::vector<std::function<void(std::strstream &)>> v);
  static void warningImpl(std::vector<std::function<void(std::strstream &)>> v);
  struct Impl;
  static std::unique_ptr<Impl> impl_;
};
*/
} // namespace execution_nodes