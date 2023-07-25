#pragma once

class Noncopyable {
 protected:
  Noncopyable() {}
  ~Noncopyable() {}
  Noncopyable(const Noncopyable &lhs) = delete;
  const Noncopyable &operator=(const Noncopyable &lhs) = delete;
};