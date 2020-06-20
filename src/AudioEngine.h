#pragma once

#include <SDL.h>
#include <cstring>

class AudioEngine
{
private:
  SDL_AudioSpec requested, obtained;
  bool paused, inited;

public:
  using callback_t = SDL_AudioCallback;


public:
  AudioEngine();
  ~AudioEngine();

  bool init(int rate, callback_t callback);

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

bool AudioEngine::init(int rate, callback_t callback)
{
  memset(&requested, 0, sizeof(SDL_AudioSpec));
  requested.freq = rate;
  requested.format = AUDIO_S16SYS;
  requested.channels = 2;
  requested.samples = 8192;
  requested.callback = callback;
  requested.userdata = nullptr;

  if (SDL_OpenAudio(&requested, &obtained) < 0)
  {
    //fprintf(stderr, "Error while opening audio: %s", SDL_GetError());
    return false;
  }

  inited = true;
  return true;
}

void AudioEngine::resume()
{
  if (paused)
    SDL_PauseAudio(false);
}

void AudioEngine::pause()
{
  if (!paused)
    SDL_PauseAudio(true);
}

void AudioEngine::stop()
{
  pause();
  SDL_LockAudio();
  SDL_UnlockAudio();
}

void AudioEngine::cleanup()
{
  stop();
  SDL_CloseAudio();
  inited = false;
}
