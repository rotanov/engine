#include "handle.hpp"

handle_base::handle_base(const uint32_t index, const uint32_t generation)
  : index(index)
  , generation(generation)
{
}

handle_base::handle_base(const uint32_t index)
  : index(index)
  , generation(UINT32_MAX)
{
}

bool handle_base::operator ==(const handle_base rhs) const
{
  return rhs.index == this->index
    && rhs.generation == this->generation;
}

bool handle_base::operator !=(const handle_base rhs) const
{
  return rhs.index != this->index
    || rhs.generation != this->generation;
}

const handle_base handle_base::invalid = handle_base(UINT32_MAX, UINT32_MAX);
