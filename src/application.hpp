#pragma once

#include "SDL.h"
#include "bgfx/bgfx.h"
#include "bgfx/bgfxplatform.h"
#include "bx/timer.h"

#include "utils.hpp"
#include "error.hpp"
#include "system.hpp"
#include "transform.hpp"
#include "render.hpp"
#include "entity.hpp"

template <typename T>
class application : public T
{
private:
  uint32_t window_width = 800;
  uint32_t window_height = 600;
  SDL_Window* sdl_window = nullptr;
  lua_State* L = nullptr;
public:
  entity_system entity_system;
  renderer renderer;
  transform_system transform_system;
  name_system name_system;
  //solid_quads solid_quads;

  application() {}

  void set_window_size(uint32_t width, uint32_t height)
  {
    window_width = width;
    window_height = height;
    if (sdl_window != nullptr) {
      SDL_SetWindowSize(sdl_window, width, height);
    }
  }

  iv2 get_window_size()
  {
    return iv2(window_width, window_height);
  }

  void create_window()
  {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      PANIC(format("Unable to initialize SDL: %s\n", SDL_GetError()));
    } else {
      sdl_window = SDL_CreateWindow(
          "ｷﾉｺの木の子",
          SDL_WINDOWPOS_CENTERED,
          SDL_WINDOWPOS_CENTERED,
          window_width,
          window_height,
          SDL_WINDOW_SHOWN
      );
    }
    bgfx::sdlSetWindow(sdl_window);
  }

  void init()
  {
    L = luaL_newstate();
    create_window();

    bgfx::init(bgfx::RendererType::OpenGL);
    uint32_t debug = BGFX_DEBUG_TEXT;
    uint32_t reset = 0;
    bgfx::reset(window_width, window_height, reset);
    bgfx::setDebug(debug);
    bgfx::setViewClear(
        0,
        BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
        0x303030ff,
        1.0f,
        0
    );

    vertex::init();
    renderer.init();
    T::application = this;
    T::init();
  }

  void main_loop()
  {
    bool quit = false;
    while (!quit) {
      SDL_Event e;
      while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
          quit = true;
        }
      }
      const double freq = double(bx::getHPFrequency());
      const double toMs = 1000.0 / freq;
      static int64_t totalMeasureTime = 0;
      int64_t measureTimeLast = bx::getHPCounter();
      T::main_loop(0.016f);
      double smoothingFactor = 0.01;
      totalMeasureTime = ((bx::getHPCounter() - measureTimeLast) * smoothingFactor
        + totalMeasureTime * (1.0 - smoothingFactor));
      transform_system.update();
      //solid_quads.draw(transform_system, renderer);
      renderer.begin_frame();
      renderer.render();
      bgfx::dbgTextPrintf(1, 1, 0x0f, "frame time: %f[ms]", 16.0f);
      bgfx::dbgTextPrintf(1, 2, 0x0f, "transform & draw time: %7.3f[ms]", double(totalMeasureTime)*toMs);
      //bgfx::dbgTextPrintf(1, 3, 0x0f, "total entities: %d", entity_system.count());
      renderer.end_frame();
      SDL_Delay(16);
    }

    shutdown();
  }

private:
  void shutdown()
  {
    if (sdl_window != nullptr) {
      SDL_DestroyWindow(sdl_window);
    }
    SDL_Quit();
    lua_close(L);
  }
};
