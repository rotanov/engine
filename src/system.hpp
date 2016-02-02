#pragma once

#include <cstdint>
#include <cstdio>
#include <unordered_map>

#include "entity.hpp"
#include "error.hpp"

template <typename T>
class system_base
{
public:
  typedef handle_base<T> handle;

  handle get_handle(const entity e)
  {
    auto i = entity_to_handle_.find(e);
    PANIC_IF(i == entity_to_handle_.end());
    return i->second;
  }
private:
  std::unordered_map<entity, handle> entity_to_handle_;
  std::unordered_map<handle, entity> handle_to_entity_;
protected:
  typedef system_base base;

  handle link(const entity e, const uint32_t new_index)
  {
    auto i = entity_to_handle_.find(e);
    PANIC_IF(i != entity_to_handle_.end());
    auto new_handle = handle(new_index);
    entity_to_handle_.emplace(e, new_handle);
    handle_to_entity_.emplace(new_handle, e);
    return new_handle;
  }
  void unlink(const entity e)
  {
    auto i_kv = entity_to_handle_.find(e);
    auto h = i_kv->second;
    PANIC_IF(i_kv == entity_to_handle_.end());
    entity_to_handle_.erase(e);
    handle_to_entity_.erase(h);
  }
  entity get_entity(const handle h)
  {
    auto i = handle_to_entity_.find(h);
    PANIC_IF(i == handle_to_entity_.end());
    return i->second;
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
    return base::link(e, next_index++);
  }
  void unlink(const entity e)
  {
    handle h = link(e);
    std::swap(names_[h.id], names_.back());
    names_.pop_back();
    base::unlink(e);
  }
  std::string& name(const handle h)
  {
    return names_[h.id];
  }
private:
  uint32_t next_index = 0;
  std::vector<std::string> names_;
};
