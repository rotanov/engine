#pragma once

template <typename T>
struct handle_base
{
  uint32_t id;

  handle_base(const uint32_t id) : id(id) {}

  bool operator ==(const handle_base rhs) const
  {
    return rhs.id == this->id;
  }

  bool operator !=(const handle_base rhs) const
  {
      return rhs.id != this->id;
  }

  static const handle_base invalid;
};

template <typename T>
const handle_base<T> handle_base<T>::invalid = handle_base<T>(UINT32_MAX);

template <typename T>
struct std::hash<handle_base<T>>
{
  size_t operator ()(const handle_base<T> e) const
  {
    return hash<uint32_t>()(e.id);
  }
};
