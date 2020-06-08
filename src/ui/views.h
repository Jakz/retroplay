#pragma once

#include "view_manager.h"

namespace ui
{
  class MainView : public View
  {
  private:
    ViewManager* gvm;
  
  public:
    MainView(ViewManager* gvm) : gvm(gvm) { }
    
    void render() override
    {
      SDL_Renderer* renderer = gvm->renderer();
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderClear(renderer);
    }

    void handleKeyboardEvent(const SDL_Event& event) override { }
    void handleMouseEvent(const SDL_Event& event) override { }
  };
}

