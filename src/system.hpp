#pragma once

#include <cstdint>
#include <cstdio>
#include <algorithm>

#include "entity.hpp"
#include "error.hpp"

class system_base
{
private:
  std::vector<uint32_t> sparse_;
  std::vector<uint32_t> dense_;
  std::vector<uint32_t> generation_;
  std::vector<entity> entities_;
  uint32_t freelist_start_ = UINT32_MAX;
public:
  system_base();
  bool has_entity(const entity e) const;
  handle_base get_handle(const entity e) const;
  void unlink(const entity e);
protected:
  const std::vector<uint32_t>& sparse() const;
  typedef system_base base;
  handle_base link(const entity e);
};

template <typename T>
class system_specific : public system_base
{
public:
  typedef handle_specific<T> handle;
};

class system_manager
{
public:
  static std::vector<system_base*>& systems()
  {
    static std::vector<system_base*> systems_;
    return systems_;
  }
};

// example system
class name_system : public system_specific<name_system>
{
protected:
  void on_allocate(const handle h)
  {
    if (h.index >= names_.size()) {
      names_.resize(h.index + 1);
    }
  }
  void on_deallocate()
  {
    // std::swap(names_[h.index], names_.back());
    names_.pop_back();
  }
public:
  handle link(const entity e, const std::string& name)
  {
    auto h = base::link(e);
    on_allocate(h);
    names_[h.index] = name;
    return h;
  }
  void unlink(const entity e)
  {
    base::unlink(e);
  }
  const std::string& name(const handle h) const
  {
    return names_[sparse()[h.index]];
  }
  std::string& name(const handle h)
  {
    return names_[sparse()[h.index]];
  }
private:
  std::vector<std::string> names_;
};
