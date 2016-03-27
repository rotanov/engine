#pragma once

#include <cstdint>
#include <cstdio>

#include "entity.hpp"
#include "error.hpp"

template <typename T>
class system_base
{
public:
  typedef handle_base<T> handle;

  handle get_handle(const entity e)
  {
    auto i = find_entity_(e);
    PANIC_IF(i == entities_.end());
    return handle(i - entities_.begin());
  }
private:
  std::vector<entity> entities_;
  std::vector<uint32_t> unused_handle_indices_;

  inline auto find_entity_(const entity e)
  {
    return std::find(entities_.begin(), entities_.end(), e);
  }
protected:
  typedef system_base base;

  handle link(const entity e)
  {
    auto i = find_entity_(e);
    PANIC_IF(i != entities_.end());
    uint32_t new_index;
    if (!unused_handle_indices_.empty()) {
      new_index = unused_handle_indices_.back();
      unused_handle_indices_.pop_back();
      entities_[new_index] = e;
    }
    else {
      new_index = entities_.size();
      entities_.push_back(e);
    }
    return handle(new_index);
  }
  void unlink(const entity e)
  {
    auto i = find_entity_(e);
    PANIC_IF(i == entities_.end());
    uint32_t index = i - entities_.begin();
    entities_[index] = entity::invalid;
    unused_handle_indices_.push_back(index);
  }
};

class system_observer
{

};

// example system
class name_system : public system_base<name_system>
{
public:
  handle link(const entity e)
  {
    names_.push_back("");
    return base::link(e);
  }
  void unlink(const entity e)
  {
    handle h = link(e);
    std::swap(names_[h.index], names_.back());
    names_.pop_back();
    base::unlink(e);
  }
  std::string& name(const handle h)
  {
    return names_[h.index];
  }
private:
  std::vector<std::string> names_;
};
