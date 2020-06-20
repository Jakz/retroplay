#pragma once

#include <SDL.h>
#include <cstring>

class AudioEngine
{
private:
  SDL_AudioDeviceID device;
  SDL_AudioSpec requested, obtained;
  bool paused, inited;

public:
  using callback_t = SDL_AudioCallback;


public:
  AudioEngine();
  ~AudioEngine();

  bool init(int rate, callback_t callback, void* data);

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

bool AudioEngine::init(int rate, callback_t callback, void* data)
{
  memset(&requested, 0, sizeof(SDL_AudioSpec));
  requested.freq = rate;
  requested.format = AUDIO_S16SYS;
  requested.channels = 2;
  requested.samples = 8192;
  requested.callback = callback;
  requested.userdata = data;

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
  printf("[LOG] Closed audio device %s.\n", SDL_GetAudioDeviceName(device, false));
  stop();
  SDL_CloseAudioDevice(device);
  inited = false;
}
