#include "entity.hpp"
#include "system.hpp"

entity entity_system::make()
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
bool entity_system::alive(const entity e) const
{
  return e.index < sparse_.size()
    && e.generation == generation_[e.index]
    && sparse_[e.index] < dense_.size()
    && dense_[sparse_[e.index]] == e.index;
}
void entity_system::kill(const entity e)
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
  // removing entity from each system
  for (auto system : system_manager::systems()) {
    if (system->has_entity(e)) {
      system->unlink(e);
    }
  }
}
uint32_t entity_system::count() const
{
  return dense_.size();
}
uint32_t entity_system::freeslot_count() const
{
  int i = 0;
  int c = freelist_start_;
  while (c != UINT32_MAX) {
    i++;
    c = sparse_[c];
  }
  return i;
}
