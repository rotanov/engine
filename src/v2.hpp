#pragma once

#include <cassert>
#include <cmath>
#include <limits>

#include "ma7th.hpp"

class v2
{
public:
  float x;
  float y;

  v2()
    : x(0.0f)
    , y(0.0f)
  {}

  INLINE__ explicit v2(float x, float y)
    : x(x)
    , y(y)
  {}

  INLINE__ explicit v2(float angle)
    : x(cosf(angle))
    , y(sinf(angle))
  {}

  INLINE__ explicit v2(const std::vector<float> &values)
  {
    assert(values.size() == 2);
    x = values[0];
    y = values[1];
  }

  v2 operator +(const v2& rhs) const
  {
    return v2(x + rhs.x, y + rhs.y);
  }

  v2 operator -(const v2& rhs) const
  {
    return v2(x - rhs.x, y - rhs.y);
  }

  v2 operator *(const float rhs) const
  {
    return v2(x * rhs, y * rhs);
  }

  v2 operator /(const float rhs) const
  {
    assert(rhs != 0.0f);
    const float t = 1.0f / rhs;
    return v2(x * t, y * t);
  }

  friend v2 operator *(float lhs, const v2& rhs)
  {
    return v2(lhs * rhs.x, lhs * rhs.y);
  }

  v2& operator +=(const v2& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }

  v2& operator -=(const v2& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
  }

  v2& operator *=(const float rhs)
  {
    x *= rhs;
    y *= rhs;
    return *this;
  }

  v2& operator /=(const float rhs)
  {
    return *this = *this / rhs;
  }

  v2 operator -() const
  {
    return v2(-x, -y);
  }

  v2 operator *(const v2& rhs) const
  {
    return v2(x * rhs.x, y * rhs.y);
  }

  v2& operator *=(const v2& rhs)
  {
    this->x *= rhs.x;
    this->y *= rhs.y;
    return *this;
  }

  bool operator ==(const v2& rhs) const
  {
    return equal(x, rhs.x) && equal(y, rhs.y);
  }

  bool operator !=(const v2& rhs) const
  {
    return !(*this == rhs);
  }

  float dot(const v2& rhs) const
  {
    return x * rhs.x + y * rhs.y;
  }

  float length() const
  {
    return sqrt(x * x + y * y);
  }

  v2 orthogonal() const
  {
    return v2(-y, x);
  }

  v2 normalized() const
  {
    return *this / length();
  }

  float normalize()
  {
    const float l = length();
    assert(l != 0.0f);
    *this /= l;
    return l;
  }

  // rotate a 2d vector v by angle a
  // assuming this is a unit 2d vector: v2(x = cos(a), y = sin(a))
  v2 rotate(const v2& v) const
  {
    v2 r;
    r.x = v.x * x - v.y * y;
    r.y = v.y * x + v.x * y;
    return r;
  }

  float& operator [](const int i)
  {
    return (&x)[i];
  }
};

template<>
INLINE__ v2 random_range<v2>(const v2& min, const v2& max)
{
  return v2(
    random_float(min.x, max.x),
    random_float(min.y, max.y)
  );
}

template<>
INLINE__ v2 clamp(const v2& x, const v2& min, const v2& max)
{
  v2 r;
  r.x = clamp(x.x, min.x, max.x);
  r.y = clamp(x.y, min.y, max.y);
  return r;
}

template <>
INLINE__ v2 round_by(const v2& x, const float by)
{
  v2 r;
  r.x = round_by(x.x, by);
  r.y = round_by(x.y, by);
  return r;
}

template<>
INLINE__ v2 abs(const v2& x)
{
  return v2(std::fabs(x.x), std::fabs(x.y));
}

namespace v2c
{
  static const v2 nan = v2(static_cast<float>(std::numeric_limits<float>::quiet_NaN()));
  static const v2 zero = v2(0.0f, 0.0f);
  static const v2 identity = v2(1.0f, 1.0f);
  static const v2 left = v2(-1.0f, 0.0f);
  static const v2 right = (-left);
  static const v2 up = v2(0.0f, 1.0f);
  static const v2 down = (-up);
  static const v2 half = v2(0.5f, 0.5f);
  static const v2 bottom_left = v2(-1.0f, -1.0f);
  static const v2 bottom_right = v2(1.0f, -1.0f);
  static const v2 top_left = v2(1.0f, -1.0f);
  static const v2 top_right = v2(1.0f, 1.0f);
  static const v2 directions[4] = { left, down, right, up };
  static const v2 diagonals[4] = { bottom_left, bottom_right, top_right, top_left };
}
