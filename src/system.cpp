#include "system.hpp"

system_base::system_base()
{
  system_manager::systems().push_back(this);
}
bool system_base::has_entity(const entity e) const
{
  return false;
}
handle_base system_base::get_handle(const entity e) const
{
  return handle_base(0, 0);
}
handle_base system_base::link(const entity e)
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
  entities_.push_back(e);
  // + something.allocate(id);
  return handle_base(id, generation_[id]);
}
void system_base::unlink(const entity e)
{
  auto index = std::find(entities_.begin(), entities_.end(), e) - entities_.begin();
  PANIC_IF(index == entities_.size());
  sparse_[dense_.back()] = sparse_[index];
  dense_[sparse_[index]] = dense_.back();
  dense_.pop_back();
  entities_[sparse_[index]] = entities_.back();
  entities_.pop_back();
  // place to deallocate data_ (same way as dense_: swap with last and pop back)
  generation_[index]++;
  PANIC_IF(generation_[index] == UINT32_MAX);
  sparse_[index] = freelist_start_;
  freelist_start_ = index;
}

inline const std::vector<uint32_t>& system_base::sparse() const
{
  return sparse_;
}
