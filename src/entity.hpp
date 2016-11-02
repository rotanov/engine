#pragma once

#include <cstdint>
#include <vector>

#include "handle.hpp"
#include "sparse_set.hpp"
#include "error.hpp"

class entity_system;
typedef handle_specific<entity_system> entity;

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
  entity make();
  bool alive(const entity e) const;
  void kill(const entity e);
  size_t count() const;
  size_t freeslot_count() const;
};
