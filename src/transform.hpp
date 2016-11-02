#pragma once

#include <cstdint>
#include <vector>

#include "v2.hpp"
#include "entity.hpp"
#include "system.hpp"

struct transform
{
  v2 position;
  v2 rotation;
  v2 scale;

  transform(
      const v2& position
    , const v2& rotation = v2(0.0f, 1.0f)
    , const v2& scale = v2(1.0f, 1.0f)
  )
    : position(position)
    , rotation(rotation)
    , scale(scale)
  {

  }
  static const transform identity;
};

class transform_system : public system_specific<transform_system>
{
private:
  std::vector<transform> local;
  std::vector<transform> world;
  std::vector<uint32_t> parent;

public:
  transform_system();
  void update();
  handle link(
      const entity e
    , transform t = transform::identity
    , handle parent_transform = handle(0, 0))
  {
    parent.push_back(parent_transform.index);
    world.push_back(transform::identity);
    local.push_back(t);
    return base::link(e);
  }
  void unlink(const entity e)
  {
    base::unlink(e);
  }
  const transform& get_world_transform(const handle h) const
  {
    return world[h.index];
  }
  void set_local_transform(const handle h, const transform& t)
  {
    local[h.index] = t;
  }
  transform& get_local_transform(const handle h)
  {
    return local[h.index];
  }
};