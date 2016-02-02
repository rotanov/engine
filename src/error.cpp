#include "error.hpp"

#include <sstream>
#include <windows.h>
#include <dbghelp.h>

static std::function<void()> on_panic_action_ = nullptr;
static std::string error_text_;
static bool is_in_panic_ = false;

void abort_at_panic_();
void begin_panic_();
void exec_on_panic_action_();

void panic_(
    const std::string& error_message
  , const char* filename
  , int line_number)
{
  std::stringstream error_text;
  error_text
    << filename <<  ":"
    << line_number << ":"
    << error_message;
  set_error_text(error_text.str());
  if (!is_in_panic_) {
    begin_panic_();
    exec_on_panic_action_();
    abort_at_panic_();
  }
}

void abort_at_panic_()
{
#if defined(_DEBUG)
  throw std::runtime_error("");
#elif defined(WIN32)
  ExitProcess(1);
#else
  ::abort();
#endif
}

void begin_panic_()
{
  // TODO: use atomic
  is_in_panic_ = true;
}

void set_error_text(const std::string& error_text)
{
  if (is_in_panic_) {
    error_text_ = error_text_ + ", " + error_text;
  } else {
    error_text_ = error_text;
  }
}

const std::string& get_error_text() {
  return error_text_;
}

void set_on_panic_action(const std::function<void()> action)
{
  on_panic_action_ = action;
}

void exec_on_panic_action_()
{
  if (on_panic_action_) {
    on_panic_action_();
  }
}
