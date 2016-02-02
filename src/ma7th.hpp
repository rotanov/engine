#pragma once

#include <cmath>
#include <cassert>
#include <stdexcept>
#include <vector>
#include <algorithm>

#ifndef INLINE__
  #if defined(_MSC_VER)
    #define INLINE__ __forceinline
  #elif __GNUC__ >= 4
    #define INLINE__ __inline __attribute__ ((always_inline))
  #else
    #define INLINE__ inline
  #endif
#endif

#if defined(min)
  #undef min
#endif

#if defined(max)
  #undef max
#endif

namespace m7c
{
  const float epsilon = 0.00001f;
  const float PI = 3.1415926f;
  const float PI_D2 = PI / 2.0f;
  const float PI2  = PI * 2.0f;
  const float OOPI = 0.3183098f;
  const int SINE_COSINE_TABLE_DIM  = 8192;
  const float deganglem =  static_cast<float>(SINE_COSINE_TABLE_DIM) / 360.0f;
  const float radanglem =  static_cast<float>(SINE_COSINE_TABLE_DIM) / PI2;
  const float PI_d180 = PI / 180.0f;
  const float d180_PI  = 180.0f / PI;
}

extern unsigned g_seed;

class v2;
class m2;

void GenSinTable();
float fSinr(float angle);
float fSind(float angle);
float fSini(int index);
float fCosr(float angle);
float fCosd(float angle);
float fCosi(int index);

#define random_int random_range<int>
#define random_float random_range<float>

template <typename T>
INLINE__ T random_range(const T& min, const T& max)
{
  assert(false);
}

template <>
INLINE__ int random_range<int>(const int& min, const int& max)
{
  assert(max - min + 1);
  g_seed = 214013 * g_seed + 2531011;
  return min + (g_seed ^ g_seed >> 15) % (max - min + 1);
}

template <>
INLINE__ float random_range<float>(const float& min, const float& max)
{
  g_seed = 214013 * g_seed + 2531011;
  //return min + g_seed * (1.0f / 4294967295.0f) * (max - min);
  return min + (g_seed >> 16) * (1.0f / 65535.0f) * (max - min);
}

template<typename T>
INLINE__ T clamp(const T& x, const T& min, const T& max)
{
  return std::min(std::max(x, min), max);
}

INLINE__ float deg_to_rad(const float degree)
{
  return static_cast<float>(degree * m7c::PI_d180);
}

INLINE__ float rad_to_deg(const float radian)
{
  return static_cast<float>(radian * m7c::d180_PI);
}

template<typename T>
INLINE__ T round(const T& x)
{
  if (x >= 0.0f) {
    return floor(x + 0.5f);
  } else {
    return ceil(x - 0.5f);
  }
}

template <typename T>
INLINE__ T round_by(const T& x, const float by)
{
  int integralTimes = static_cast<int>(floor(x / by));
  const float remainder = x - integralTimes * by;
  T result = integralTimes * by;
  if (remainder > by * 0.5f)
  {
    result += by;
  }
  return result;
}

template<typename T>
INLINE__ T sqr(const T& x)
{
  return x * x;
}

template<typename T>
INLINE__ T cube(const T& x)
{
  return x * x * x;
}

template<typename T>
INLINE__ T abs(const T& x)
{
  return x < 0 ? -x : x;
}

template<>
INLINE__ float abs(const float& x)
{
  return std::fabs(x);
}

template<typename T>
INLINE__ bool equal(const T& a, const T& b)
{
  return abs(a - b) < m7c::epsilon;
}

template <typename T>
INLINE__ int sign(const T& x, std::false_type is_signed)
{
  return T(0) < x;
}

template <typename T>
INLINE__ int sign(const T& x, std::true_type is_signed)
{
  return (T(0) < x) - (x < T(0));
}

template <typename T>
INLINE__ int sign(const T& x)
{
  return sign(x, std::is_signed<T>());
}

/**
*  Interpolate between interval [a,b] with t in [0,1].
*/
template <typename T>
INLINE__ T interpolate_linear(const T& a, const T& b, const float t)
{
  return a + (b - a) * t;
}

template <typename T>
INLINE__ T interpolate_cosine(const T& a, const T& b, const float t)
{
  return a + (b - a) * (1.0f - cos(t * m7c::PI)) * 0.5f;
}

/**
*  Here v1 is the "a" point and v2 is the "b" point.
*  v0 is "the point before a" and v3 is "the point after b".
*/
INLINE__ float interpolate_cubic(const float v0, const float v1, const float v2,
  const float v3, const float t)
{
  float P = (v3 - v2) - (v0 - v1);
  float Q = (v0 - v1) - P;
  float R = v2 - v0;
  float S = v1;
  float x2 = t * t;
  float x3 = x2 * t;
  return P * x3 + Q * x2 + R * t + S;
}

template <typename T>
INLINE__ T interpolate_catmull_rom(const T& v0, const T& v1, const T& v2, const T& v3,
  const float t)
{
  const float t2 = t * t;
  const float t3 = t2 * t;
  return v1 + (
    (v2 - v0) * t +
    (v0 * 2.0f + v2 * 4.0f - v1 * 5.0f - v3) * t2 +
    (v1 * 3.0f + v3 - v0 - v2 * 3.0f) * t3) * 0.5f;
}

// Snap floating point number to grid.
INLINE__ float snap(float p, float grid)
{
  return grid ? static_cast<float>(floor((p + grid * 0.5f) / grid) * grid) : p;
}

// /**
// *  SqareEq(...) Решает квдратное уравнение.
// *  Корни - t0 и t1
// *  a b c - соответствующие коэффициенты квадратного уравнения.
// */
// bool SqareEq(float a, float b, float c, float &t0, float &t1);

// /**
// *  HalfPlaneSign - определяет знак полуплоскости точки x относительно прямой, лежащей на отрезке [u0, u1].
// *  Если точка лежит на прямой, то результат 0.0f
// */
// float HalfPlaneSign(const Vector2 &u0, const Vector2 &u1, const Vector2 &p);

// /**
// *  DistanceToLine - определяет расстояние со знаком от точки до прямой, лежащей на отрезке.
// */
// float DistanceToLine(const Vector2 &u0, const Vector2 &u1, const Vector2 &p);

// /**
// *  DistanceToSegment - определяет расстояние со знаком от точки до отрезка.
// *  @todo: Возможно есть более быстрый способ определять знак расстояниея в случаях конца отрезка.
// *  @todo: Возможно стоит возвращать ещё и найденный случай, т.е. первая точка отрезка, вторая или сам отрезок.
// */
// float DistanceToSegment(const Vector2 &u0, const Vector2 &u1, const Vector2 &p);

// *
// *  IntersectLines - определяет точку пересечения прямых, лежащих на отрезках, если они пересекаются.

// bool IntersectLines(const Vector2 &u0, const Vector2 &u1, const Vector2 &v0, const Vector2 &v1, Vector2 &Result);

// /**
// *  IntersectSegments(): returns intersection point if segments intersect.
// *  Notice, that it is segments, not infinite lines.
// *  @todo: check out coincident lines case.
// */
// bool IntersectSegments(const Vector2 &u0, const Vector2 &u1, const Vector2 &v0, const Vector2 &v1, Vector2 &Result);

// /**
// *  AreSegmentsIntersect(): checks if segments intersect.
// *  Notice, that it is segments, not infinite lines.
// *  @todo: check out coincident lines case.
// */
// bool AreSegmentsIntersect(const Vector2 &u0, const Vector2 &u1, const Vector2 &v0, const Vector2 &v1);

// /**
// *  CalcConvexHull does exactly how it named.
// *  Taken from http://www.e-maxx.ru/algo/convex_hull_graham and slightly
// *  modified in order to fit naming conventions and interfaces
// */
// void CalcConvexHull (std::vector<Vector2> &a);
