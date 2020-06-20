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

  virtual Player* buildPlayer(const AudioSpec& spec) const = 0;
  virtual std::vector<MusicFormatType> supportedFormats() const = 0;

  virtual std::string name() const = 0;
};

class PluginManager
{
private:
  std::vector<std::unique_ptr<Plugin>> plugins;

public:
  void registerPlugin(Plugin* plugin) { plugins.push_back(std::unique_ptr<Plugin>(plugin)); }
  Plugin* findPluginForFormat(MusicFormatType format)
  {
    auto it = std::find_if(plugins.begin(), plugins.end(), [format](const auto& plugin) {
      const auto& formats = plugin->supportedFormats();
      return std::find(formats.begin(), formats.end(), format) != formats.end();
    });

    if (it != plugins.end())
      return it->get();
    else
      return nullptr;
  }

  size_t size() const { return plugins.size(); }
};