#pragma once

template <typename T>
struct handle_base
{
  uint32_t index;
  uint32_t generation;
  explicit handle_base(const uint32_t index, const uint32_t generation)
    : index(index)
    , generation(generation)
  {}
  explicit handle_base(const uint32_t index)
    : index(index)
    , generation(UINT32_MAX)
  {}
  handle_base() = delete;
  bool operator ==(const handle_base rhs) const
  {
    return rhs.index == this->index
      && rhs.generation == this->generation;
  }

  bool operator !=(const handle_base rhs) const
  {
      return rhs.id != this->id;
  }

  static const handle_base invalid;
};

template <typename T>
const handle_base<T> handle_base<T>::invalid = handle_base<T>(UINT32_MAX, UINT32_MAX);
