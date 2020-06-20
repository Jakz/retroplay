#include "FileManager.h"

#include <algorithm>
#include <cassert>

using MFT = MusicFormatType;

std::vector<MusicFormat> FileManager::formats = {
  { MFT::NSF, "NES Music", { "nsf" } },
  { MFT::GBS, "GameBoy Music", { "gbs" } },
};

data_t FileManager::load(const path& path)
{
  FILE* in = fopen(path.c_str(), "rb");
  fseek(in, 0, SEEK_END);
  size_t length = ftell(in);
  fseek(in, 0, SEEK_SET);

  uint8_t* data = new uint8_t[length];
  assert(data);
  fread(data, sizeof(uint8_t), length, in);
  fclose(in);

  return { data, length };
}

const MusicFormat* FileManager::formatForPath(const path& path)
{
  std::string extension = path.extension();

  auto it = std::find_if(formats.begin(), formats.end(), [&extension](const MusicFormat& format) {
    return std::find(format.extensions.begin(), format.extensions.end(), extension) != format.extensions.end();
  });

  return it != formats.end() ? &(*it) : nullptr;
}