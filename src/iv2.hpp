#pragma once

#include <cstdint>

#include "ma7th.hpp"

struct iv2
{
  uint32_t x;
  uint32_t y;

  iv2() {}
  INLINE__ explicit iv2(uint32_t x, uint32_t y)
    : x(x)
    , y(y)
  {}

};
