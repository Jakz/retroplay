/* How to play game music files with Music_Player (requires SDL library)

Run program with path to a game music file.

Left/Right  Change track
Space       Pause/unpause
E           Normal/slight stereo echo/more stereo echo
A			Enable/disable accurate emulation
-/=         Adjust tempo
1-9         Toggle channel on/off
0           Reset tempo and turn channels back on */

#include "ui/view_manager.h"

ui::ViewManager gvm;

int const scope_width = 512;

#include "plugins/libgme/player/Music_Player.h"
#include "plugins/libgme/player/Audio_Scope.h"

extern "C"
{
  #include "plugins/libupse/upse.h"
}

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>

void handle_error(const char*);

static bool paused;
static Music_Player* player;

static void start_track(int track, const char* path)
{
  paused = false;
  handle_error(player->start_track(0));

  // update window title with track info

  long seconds = player->track_info().length / 1000;
  const char* game = player->track_info().game;
  if (!*game)
  {
    // extract filename
    game = strrchr(path, '\\'); // DOS
    if (!game)
      game = strrchr(path, '/'); // UNIX
    if (!game)
      game = path;
    else
      game++; // skip path separator
  }

  char title[512];


  sprintf(title, "%s: %d/%d %s (%ld:%02ld)",
    game, track, player->track_count(), player->track_info().song,
    seconds / 60, seconds % 60);

  gvm.text(title, 0, 0);
}

int mainz()
{

  const char* path = "test.nsf";
  handle_error(player->load_file(path));
  start_track(1, path);

  // Main loop
  int track = 1;
  double tempo = 1.0;
  bool running = true;
  double stereo_depth = 0.0;
  bool accurate = false;
  int muting_mask = 0;
  while (running)
  {
    SDL_Delay(1000 / 100);

    // Automatically go to next track when current one ends
    if (player->track_ended())
    {
      if (track < player->track_count())
        start_track(++track, path);
      else
        ;
        //player->pause(paused = true);
    }

    // Handle keyboard input
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
      switch (e.type)
      {
      case SDL_QUIT:
        running = false;
        break;

      case SDL_KEYDOWN:
        int key = e.key.keysym.sym;
        switch (key)
        {
        case SDLK_q:
        case SDLK_ESCAPE: // quit
          running = false;
          break;

        case SDLK_LEFT: // prev track
          if (!paused && !--track)
            track = 1;
          start_track(track, path);
          break;

        case SDLK_RIGHT: // next track
          if (track < player->track_count())
            start_track(++track, path);
          break;

        case SDLK_MINUS: // reduce tempo
          tempo -= 0.1;
          if (tempo < 0.1)
            tempo = 0.1;
          player->set_tempo(tempo);
          break;

        case SDLK_EQUALS: // increase tempo
          tempo += 0.1;
          if (tempo > 2.0)
            tempo = 2.0;
          player->set_tempo(tempo);
          break;

        case SDLK_SPACE: // toggle pause
          paused = !paused;
          //player->pause(paused);
          break;

        case SDLK_a: // toggle accurate emulation
          accurate = !accurate;
          player->enable_accuracy(accurate);
          break;

        case SDLK_e: // toggle echo
          stereo_depth += 0.2;
          if (stereo_depth > 0.5)
            stereo_depth = 0;
          player->set_stereo_depth(stereo_depth);
          break;

        case SDLK_0: // reset tempo and muting
          tempo = 1.0;
          muting_mask = 0;
          player->set_tempo(tempo);
          player->mute_voices(muting_mask);
          break;

        default:
          if (SDLK_1 <= key && key <= SDLK_9) // toggle muting
          {
            muting_mask ^= 1 << (key - SDLK_1);
            player->mute_voices(muting_mask);
          }
        }
      }
    }
  }

  // Cleanup
  delete player;

  return 0;
}

void handle_error(const char* error)
{
  if (error)
  {
    // put error in window title
    char str[256];
    sprintf(str, "Error: %s", error);
    fprintf(stderr, "%s\n", str);
    SDL_SetWindowTitle(gvm.window(), str);

    // wait for keyboard or mouse activity
    SDL_Event e;
    do
    {
      while (!SDL_PollEvent(&e)) {}
    } while (e.type != SDL_QUIT && e.type != SDL_KEYDOWN && e.type != SDL_MOUSEBUTTONDOWN);

    exit(EXIT_FAILURE);
  }
}

class MusicPlugin
{
private:

public:
};


#include "AudioEngine.h"
#include "FileManager.h"

AudioEngine audio;

int main(int argc, char* argv[])
{  
  if (!gvm.init())
    return -1;

  if (!gvm.loadData())
  {
    printf("Error while loading and initializing data.\n");
    gvm.deinit();
    return -1;
  }

  

  FileManager fm;
  auto data = fm.load("music/sml.gbs");

  Music_Player* player = new Music_Player();
  player->init(44100);

  audio.init(44100, Music_Player::fill_buffer, player);

  player->load_data(data.data, data.length);
  player->start_track(0);

  audio.resume();

  gvm.loop();
  gvm.deinit();

  //loader.load("1level.l");
  //getchar();

  audio.cleanup();

  return 0;
}

void* mopen(const char* path, const char* mode) { return fopen(path, mode); }
int mclose(void* file) { return fclose((FILE*)file); }
long mtell(void* file) { return ftell((FILE*)file); }
size_t mread(void* ptr, size_t size, size_t count, void* file) { return fread(ptr, size, count, (FILE*)file); }
int mseek(void* file, long offset, int whence) { return fseek((FILE*)file, offset, whence); }

/*const char* path = "D:/dev/retroplay/projects/ms2017/RetroPlay/ff7/319 Aeris' Theme.minipsf";

upse_iofuncs_t functs =
{
  mopen, mread, mseek, mclose, mtell
};

upse_module_init();

upse_module_t* module = upse_module_open(path, &functs);

s16* samples = new s16[module->metadata->rate];
int position = 0;

s16* buffer = nullptr;

while (position < module->metadata->rate)
{
  int rendered = module->evloop_render(&module->instance, &buffer);
  memcpy(samples + position, buffer, sizeof(s16) * rendered);
  position += rendered;
}


int rendered = module->evloop_render(&module->instance, &buffer);

upse_module_close(module);*/