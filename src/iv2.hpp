#pragma once

#include <cstdint>

#include "ma7th.hpp"

struct iv2
{
  int32_t x;
  int32_t y;

  iv2() {}

  INLINE__ explicit iv2(int32_t x, int32_t y)
    : x(x)
    , y(y)
  {}

  iv2 operator +(const iv2& rhs) const
  {
    return iv2(x + rhs.x, y + rhs.y);
  }

  int32_t& operator [](const int i)
  {
    return (&x)[i];
  }
};
