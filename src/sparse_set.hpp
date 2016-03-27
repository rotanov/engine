#pragma once

#include <vector>
#include <cstdint>
#include "error.hpp"

class sparse_set
{
private:
  std::vector<uint32_t> sparse_;
  std::vector<uint32_t> dense_;
public:
  bool insert(const uint32_t e)
  {
    if (contains(e)) {
      return false;
    }
    if (sparse_.size() <= e) {
      sparse_.resize(e + 1);
    }
    sparse_[e] = dense_.size();
    dense_.push_back(e);
    return true;
  }
  void erase(const uint32_t e)
  {
    PANIC_IF(!contains(e));
    sparse_[dense_.back()] = sparse_[e];
    dense_[sparse_[e]] = dense_.back();
    dense_.pop_back();
  }
  bool contains(const uint32_t e) const
  {
    return e < sparse_.size()
        && sparse_[e] < dense_.size()
        && dense_[sparse_[e]] == e;
  }
  std::vector<uint32_t>::const_iterator begin() const
  {
    return dense_.begin();
  }
  std::vector<uint32_t>::const_iterator end() const
  {
    return dense_.end();
  }
  size_t size() const
  {
    return dense_.size();
  }
  void clear()
  {
    dense_.clear();
    sparse_.clear();
  }
  bool empty() const
  {
    return dense_.size() == 0;
  }
};
