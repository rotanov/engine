#pragma once

#include <cstdint>
#include <vector>
#include <unordered_set>

#include "handle.hpp"
#include "error.hpp"

class entity_system;
typedef handle_base<entity_system> entity;

class entity_system
{
private:
  std::unordered_set<entity> dead_entities_;
  uint32_t next_index_ = 0;
  unsigned count_ = 0;

public:
  entity make()
  {
    count_++;
    if (dead_entities_.size() > 0) {
      auto e = *dead_entities_.begin();
      dead_entities_.erase(e);
      return e;
    } else {
      return entity(next_index_++);
    }
  }

  bool alive(entity e) const
  {
    return e.id < next_index_ && dead_entities_.count(e) == 0;
  }

  void kill(entity e)
  {
    PANIC_IF(!alive(e));
    count_--;
    dead_entities_.insert(e);
  }

  unsigned count() const
  {
    return count_;
  }
};
