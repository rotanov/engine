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
  struct entity_data_
  {
    // we should store system-entity relation here for all systems
    // at best it would be for all systems if such a relation exists
    // a relation must be expressed as handle of this entity in system
  };
  // sparse_[e.index] == index of e in dense_
  std::vector<uint32_t> sparse_;
  // dense_[sparse_[e.index]] == sparse_[e.index]
  // i.e. dense_ and sparse_ must point to each other
  // for every valid (alive) entity
  std::vector<uint32_t> dense_;
  // generation_[e.index] == e.generation if entity handle is valid
  std::vector<uint32_t> generation_;
  // first not used entity handle index in sparse_
  // sparse_[freelist_start_] == `next_free_index_in_sparse_`
  // or UINT32_MAX if this is the last free index in sparse_
  uint32_t freelist_start_ = UINT32_MAX;
  // arbitrary (so far) data associated with entity
  // data(e) is data_[sparse_[e.index]]
  std::vector<entity_data_> data_;
public:
  entity make()
  {
    int id = freelist_start_;
    if (id == UINT32_MAX) {
      id = sparse_.size();
      generation_.push_back(0);
      sparse_.push_back(UINT32_MAX);
    } else {
      freelist_start_ = sparse_[freelist_start_];
    }
    sparse_[id] = dense_.size();
    dense_.push_back(id);
    // place to allocate data_ i.e. data_push_back(...)
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
    // place to deallocate data_ (same way as dense_: swap with last and pop back)
    generation_[e.index]++;
    PANIC_IF(generation_[e.index] == UINT32_MAX);
    sparse_[e.index] = freelist_start_;
    freelist_start_ = e.index;
    // for each system
    //   if (system.has_entity(e) == true)
    //     system.unlink(e)
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
