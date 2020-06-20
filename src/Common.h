#pragma once

#include <cstdint>

using byte = uint8_t;

enum class MusicFormatType
{
  NSF, GBS,
};

struct data_t
{
  byte* data;
  size_t length;
};

struct status_t
{
private:
  int _code;
  std::string _message;

public:
  status_t() : _code(0), _message("") { }
  status_t(const char* message) : _code(message ? -1 : 0), _message(message ? message : "") { }
  status_t(std::string&& message) : _code(-1), _message(message) { }
  status_t(int code, std::string&& message) : _code(code), _message(message) { }

  int code() const { return _code; }
  const std::string& message() const { return _message; }

  operator bool() const { return _code == 0; }
};

struct AudioSpec
{
  int rate;
};

#define ERROR(fmt, ...) printf(fmt ".\n", __VA_ARGS__)