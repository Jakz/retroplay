#pragma once

#include <SDL.h>
#include <cstring>

#include "Plugin.h"

class AudioEngine
{
private:
  SDL_AudioDeviceID device;
  SDL_AudioSpec requested, obtained;
  bool paused, inited;

  static void callback(void* data, uint8_t* out, int count);

public:
  using callback_t = SDL_AudioCallback;


public:
  AudioEngine();
  ~AudioEngine();

  bool init(int rate, Player* target);

  void resume();
  void pause();

  void cleanup();
  void stop();

  const SDL_AudioSpec spec() { return obtained; }
};

AudioEngine::AudioEngine() : paused(true), inited(false)
{

}

AudioEngine::~AudioEngine()
{
  if (inited)
    cleanup();
}

void AudioEngine::callback(void* data, uint8_t* out, int count)
{
  static_cast<Player*>(data)->fillBuffer(out, count);
}

bool AudioEngine::init(int rate, Player* target)
{
  memset(&requested, 0, sizeof(SDL_AudioSpec));
  requested.freq = rate;
  requested.format = AUDIO_S16SYS;
  requested.channels = 2;
  requested.samples = 8192;
  requested.callback = callback;
  requested.userdata = target;

  device = SDL_OpenAudioDevice(nullptr, 0, &requested, &obtained, 0);

  if (device <= 0)
  {
    fprintf(stderr, "Error while opening audio: %s", SDL_GetError());
    return false;
  }
  else
    printf("[LOG] Opened audio device %s with a sample rate of %d.\n", SDL_GetAudioDeviceName(device, false), rate);

  inited = true;
  return true;
}

void AudioEngine::resume()
{
  if (paused)
    SDL_PauseAudioDevice(device, false);
}

void AudioEngine::pause()
{
  if (!paused)
    SDL_PauseAudioDevice(device, true);
}

void AudioEngine::stop()
{
  pause();
  SDL_LockAudioDevice(device);
  SDL_UnlockAudioDevice(device);
}

void AudioEngine::cleanup()
{
  printf("[LOG] Closed audio device.\n");
  stop();
  SDL_CloseAudioDevice(device);
  inited = false;
}
