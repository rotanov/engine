#pragma once

#include <cstdint>
#include <functional>

template <typename T>
struct handle_specific;

struct handle_base
{
  uint32_t index;
  uint32_t generation;
  explicit handle_base(const uint32_t index, const uint32_t generation);
  explicit handle_base(const uint32_t index);
  handle_base() = delete;
  bool operator ==(const handle_base rhs) const;
  bool operator !=(const handle_base rhs) const;
  static const handle_base invalid;

  template <typename T>
  operator handle_specific<T>() const
  {
    return handle_specific<T>(index, generation);
  }
};

template <typename T>
struct handle_specific : public handle_base
{
  explicit handle_specific(const uint32_t index, const uint32_t generation)
    : handle_base(index, generation)
  {

  }
private:
  // to prohibit casting any specific handle to any
  // and just left with casting handle_base to any_handle
  template <typename S>
  operator handle_specific<S>() const
  {
    return handle_specific<S>(index, generation);
  }
};

template <typename T>
struct std::hash<handle_specific<T>>
{
  size_t operator ()(const handle_specific<T> h) const
  {
    return hash<uint32_t>()(h.index);
  }
};

template <>
struct std::hash<handle_base>
{
  size_t operator ()(const handle_base h) const
  {
    return hash<uint32_t>()(h.index);
  }
};
