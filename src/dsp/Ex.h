#ifndef PICO_CMSIS_SANDBOX_CMSISEX_H_INCLUDED
#define PICO_CMSIS_SANDBOX_CMSISEX_H_INCLUDED

#include <string>
#include <exception>

class Ex : public std::exception {
public:
  const std::string msg;
  
  Ex(const std::string& msg)
    : msg(msg)
  {}
  
  virtual const char* what() const noexcept {
    return msg.c_str();
  }
};

class Fail : public Ex {
public:
  Fail(const std::string& msg)
    : Ex(msg)
  {}
};

#endif
