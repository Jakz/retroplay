#pragma once

#include <string>
#include <vector>
#include <cstdio>

#include "Common.h"

struct path
{
private:
  std::string data;

public:
  path(const char* data) : data(data) { }
  path(std::string&& data) : data(data) { }

  std::string extension() const
  {
    auto i = data.find_last_of(".");
    return i == decltype(data)::npos ? "" : data.substr(i + 1);
  }

  const char* c_str() const { return data.c_str(); }
};

struct MusicFormat
{
  MusicFormatType type;
  std::string name;
  std::vector<std::string> extensions;
};

class FileManager
{
private:
  static std::vector<MusicFormat> formats;

public:
  data_t load(const path& path);
  const MusicFormat* formatForPath(const path& path);
};

