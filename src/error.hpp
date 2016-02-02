#pragma once

#include <string>
#include <functional>

void set_on_panic_action(const std::function<void()> action);
void set_error_text(const std::string& error_text);
const std::string& get_error_text();

void panic_(
    const std::string& error_message
  , const char* filename
  , int line_number);

#define PANIC(MESSAGE) do { panic_(MESSAGE, __FILE__, __LINE__); } \
  while (false); \

#define PANIC_IF(CONDITION) do { \
  if (CONDITION) { \
    PANIC(#CONDITION); \
  } \
} while (false);
