#pragma once

#include <cstdint>
#include <vector>

#include "handle.hpp"
#include "sparse_set.hpp"
#include "error.hpp"

class entity_system;
typedef handle_base<entity_system> entity;

class entity_system
{
private:
  std::vector<uint32_t> sparse_;
  std::vector<uint32_t> dense_;
  std::vector<uint32_t> generation_;
  uint32_t freelist_start_ = UINT32_MAX;

public:
  entity make()
  {
    int id = freelist_start_;
    if (id == UINT32_MAX) {
      generation_.push_back(0);
      sparse_.push_back(UINT32_MAX);
      id = sparse_.size() - 1;
    } else {
      freelist_start_ = sparse_[freelist_start_];
    }
    sparse_[id] = dense_.size();
    dense_.push_back(id);
    return entity(id, generation_[id]);
  }
  bool alive(const entity e) const
  {
    return e.index < sparse_.size()
      && e.generation == generation_[e.index]
      && sparse_[e.index] < dense_.size()
      && dense_[sparse_[e.index]] == e.index;
  }
  void kill(const entity e)
  {
    PANIC_IF(!alive(e));
    sparse_[dense_.back()] = sparse_[e.index];
    dense_[sparse_[e.index]] = dense_.back();
    dense_.pop_back();
    generation_[e.index]++;
    PANIC_IF(generation_[e.index] == UINT32_MAX);
    sparse_[e.index] = freelist_start_;
    freelist_start_ = e.index;
    // for each system
    // if system(e) == true
    // remove system(c)
  }
  size_t count() const
  {
    return dense_.size();
  }
  size_t freeslot_count() const
  {
    int i = 0;
    int c = freelist_start_;
    while (c != UINT32_MAX) {
      i++;
      c = sparse_[c];
    }
    return i;
  }
};
