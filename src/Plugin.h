#pragma once

#include "Common.h"

#include <vector>

using track_index = uint32_t;

class Player
{
public:
  virtual status_t loadData(data_t data) = 0;

  virtual status_t play(track_index i = 0) = 0;
  virtual void fillBuffer(uint8_t* out, size_t count) = 0;

  virtual void pause() { };
  virtual void resume() { };
  virtual void stop() { };

  virtual void cleanup() { };
};

class Plugin
{
private:

public:

  virtual Player* buildPlayer(const AudioSpec& spec) = 0;
  virtual std::vector<MusicFormatType> supportedFormats() = 0;
};