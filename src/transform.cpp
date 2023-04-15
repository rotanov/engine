#include "transform.hpp"

const transform transform::identity = transform(v2(0.0f, 0.0f), v2(0.0f), v2(1.0f, 1.0f));

INLINE__ void update_single_transform(const transform& local, transform& world, const transform& parent)
{
    world.scale = parent.scale * local.scale;
    world.rotation = parent.rotation.rotate(local.rotation);
    world.position = parent.position + parent.rotation.rotate(local.position * parent.scale);
}

void transform_system::update()
{
  uint32_t n = world.size();
  world[0] = local[0];
  for (uint32_t i = 1; i < n; i++) {
    update_single_transform(local[i], world[i], world[parent[i]]);
  }
}

transform_system::transform_system()
{
  auto super_root = link(entity::invalid, transform::identity, handle::invalid);
}
