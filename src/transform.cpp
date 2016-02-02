#include "transform.hpp"

const transform transform::identity = transform(v2(0.0f, 0.0f), v2(0.0f), v2(1.0f, 1.0f));

void transform_system::update()
{
  uint32_t n = world.size();
  for (uint32_t i = 1; i < n; i++)
  {
    transform& l = local[i];
    transform& w = world[i];
    transform& p = world[parent[i]];
    w.scale = p.scale * l.scale;
    w.rotation = p.rotation.rotate(l.rotation);
    w.position = p.position + p.rotation.rotate(l.position * p.scale);
  }
}

transform_system::transform_system()
{
  parent.push_back(UINT32_MAX);
  local.push_back(transform::identity);
  world.push_back(transform::identity);
}