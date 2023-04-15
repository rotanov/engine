#pragma once

#include "SDL.h"
#include "bgfx/bgfx.h"
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
  uint32_t window_width = 1600;
  uint32_t window_height = 1200;
  SDL_Window* sdl_window = nullptr;
  lua_State* L = nullptr;
public:
  entity_system entity_system;
  renderer renderer;
  transform_system transform_system;
  solid_quads solid_quads;
  texture_system texture_system;

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
  }

  void init()
  {
    L = luaL_newstate();
    create_window();

    bgfx::Init init;
    init.type = bgfx::RendererType::Enum::OpenGL;
    init.vendorId = BGFX_PCI_ID_NONE;

    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(sdl_window, &wmi)) {
      return;
    }
    bgfx::PlatformData pd;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    pd.ndt = wmi.info.x11.display;
    pd.nwh = (void*)(uintptr_t)wmi.info.x11.window;
#elif BX_PLATFORM_OSX
    pd.ndt = NULL;
    pd.nwh = wmi.info.cocoa.window;
#elif BX_PLATFORM_WINDOWS
    pd.ndt = NULL;
    pd.nwh = wmi.info.win.window;
#elif BX_PLATFORM_STEAMLINK
    pd.ndt = wmi.info.vivante.display;
    pd.nwh = wmi.info.vivante.window;
#endif // BX_PLATFORM_
    pd.context = NULL;
    pd.backBuffer = NULL;
    pd.backBufferDS = NULL;

    init.platformData = pd;

    init.resolution.width = window_width;
    init.resolution.height = window_height;
    init.resolution.reset = BGFX_RESET_VSYNC | BGFX_RESET_SRGB_BACKBUFFER;

    bgfx::init(init);

    uint32_t debug = BGFX_DEBUG_TEXT;
    uint32_t reset = 0;

    bgfx::setDebug(debug);

    bgfx::setViewClear(
      0,
      BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
      0x040404ff,
      1.0f,
      0
    );

    renderer.init();
    texture_system.init();
    T::application = this;
    T::init();
  }

  void start()
  {
    main_loop();
  }

  void main_loop()
  {
    bool quit = false;
    while (!quit) {
      SDL_Event e;
      while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
          quit = true;
        } else if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
          if (e.key.keysym.sym == SDLK_ESCAPE) {
            quit = true;
          }
          T::consume_key_down(e.key.keysym.sym);
        } else if (e.type == SDL_KEYUP) {
          T::consume_key_up(e.key.keysym.sym);
        }
      }

      double smoothing_factor = 0.5;
      const double freq = double(bx::getHPFrequency());
      const double to_ms = 1000.0 / freq;
      static int64_t user_loop_time = 0;
      static int64_t transform_system_time = 0;
      static int64_t render_time = 0;
      static int64_t loop_time = 0;

      int64_t time = bx::getHPCounter();
      T::main_loop(0.016f);
      user_loop_time = ((bx::getHPCounter() - time) * smoothing_factor + user_loop_time * (1.0 - smoothing_factor));

      time = bx::getHPCounter();
      transform_system.update();
      transform_system_time = ((bx::getHPCounter() - time) * smoothing_factor + transform_system_time * (1.0 - smoothing_factor));

      time = bx::getHPCounter();
      solid_quads.draw(transform_system, texture_system, renderer);
      renderer.begin_frame();
      renderer.render();
      //bgfx::dbgTextPrintf(1, 1, 0x0f, "frame time: %f[ms]", 16.0f);
      bgfx::dbgTextPrintf(1, 1, 0x0f, "user loop time: %7.3f[ms]", double(user_loop_time) * to_ms);
      bgfx::dbgTextPrintf(1, 2, 0x0f, "transform system time: %7.3f[ms]", double(transform_system_time) * to_ms);
      bgfx::dbgTextPrintf(1, 3, 0x0f, "render time: %7.3f[ms]", double(render_time) * to_ms);
      bgfx::dbgTextPrintf(1, 4, 0x0f, "total entities: %d", entity_system.count());
      renderer.end_frame();
      render_time = ((bx::getHPCounter() - time) * smoothing_factor + render_time * (1.0 - smoothing_factor));

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
