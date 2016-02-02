#pragma once

#include <cstdarg>
#include <memory>
#include <cstdint>

std::string format(const std::string format_string, ...)
{
  int final_n = 0;
  int n = ((int)format_string.size()) * 2;
  std::string str;
  std::unique_ptr<char[]> formatted;
  va_list ap;
  bool not_fit = false;
  do {
    formatted.reset(new char[n]);
    strcpy(&formatted[0], format_string.c_str());
    va_start(ap, format_string);
    final_n = vsnprintf(&formatted[0], n, format_string.c_str(), ap);
    va_end(ap);
    not_fit = final_n < 0 || final_n >= n;
    if (not_fit) {
      n += abs(final_n - n + 1);
    }
  } while (not_fit);
  return std::string(formatted.get());
}
