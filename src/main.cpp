#include <stdio.h>

#include <SDL.h>
#include <SDL_syswm.h>
#include <bgfx.h>
#include <bgfxplatform.h>
#include <lua.hpp>
#include <lauxlib.h>
#include <lualib.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char** argv)
{
  lua_State *L = luaL_newstate();

  SDL_Window* window = nullptr;
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("error %s\n", SDL_GetError());
  } else {
    window = SDL_CreateWindow("どうもありがとう", SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  }
  bgfx::sdlSetWindow(window);
  bgfx::init();

  uint32_t debug = BGFX_DEBUG_TEXT;
  uint32_t reset = BGFX_RESET_VSYNC;

  bgfx::init();
  bgfx::reset(SCREEN_WIDTH, SCREEN_HEIGHT, reset);

  bgfx::setDebug(debug);

  bgfx::setViewClear(0
    , BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
    , 0x303030ff
    , 1.0f
    , 0
  );

  bool quit = false;
  while (!quit)
  {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
      if (e.type == SDL_QUIT)
      {
          quit = true;
      }
    }
    bgfx::setViewRect(0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    bgfx::touch(0);

    bgfx::dbgTextClear();
    bgfx::dbgTextPrintf(0, 1, 0x4f, "Good news, Yerevan.");

    bgfx::frame();

    SDL_Delay(16);
  }

  bgfx::shutdown();
  if (window != nullptr)
  {
    SDL_DestroyWindow(window);
  }
  SDL_Quit();
  lua_close(L);
  return 0;
}
