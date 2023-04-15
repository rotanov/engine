#include <cstdio>
#include <cstdint>
#include <thread>

#include "SDL.h"
#include "SDL_syswm.h"
#include "lua.hpp"
#include "lauxlib.h"
#include "lualib.h"

#include "application.hpp"
#include "entity.hpp"
#include "sparse_set.hpp"

class case2
{
protected:
  application<case2>* application;

  case2() {}

private:

public:
  entity root = entity::invalid;

  void init()
  {
    auto& a = *application;
    auto sw = a.renderer.screen_width;
    auto sh = a.renderer.screen_height;
    const int size = 32.0f;

    root = a.entity_system.make();
    auto root_t = a.transform_system.link(root);
    auto root_tsh = a.transform_system.link(root, transform(v2(sw / 2, sh / 2), v2(0.0f), v2(0.5f, 0.5f)));
    a.solid_quads.link(root, quad(v2(64.0f, 64.0f), 0xff00ff, v2(0.0f, 0.0f)), root_tsh);
    std::vector<transform_system::handle> children;
    children.push_back(root_tsh);
    std::vector<transform_system::handle> new_children;
    int total_children = 0;
    // 15 2 1.7ms
    // 4 16 = ~69k 2.1ms
    // 1 65000 1.7ms
    // 16 2
    for (int i = 0; i < 4; i++) {
      for (auto& n : children) {
        for (int j = 0; j < 16; j++) {
          total_children++;
          auto t = a.entity_system.make();
          auto t_tsh = a.transform_system.link(
            t,
            transform(
              random_range(v2(-sw / 2, -sh / 2), v2(sw / 2, sh / 2)) / (i + 1),
              v2(random_range(0.0f, 3.1415f)),
              v2(1.0f, 1.0f) / (float)(sqrt(i + 1.0f))
            ),
            n
          );
          auto q = a.solid_quads.link(
            t,
            quad(
              random_range(v2(size / 2.0f, size / 2.0f), v2(size, size)),
              0x00FFFFFF & 0xfdf << (i * 3) | 0x44000000,
              v2(0.5f, 0.5f)
            ),
            t_tsh
          );
          new_children.push_back(t_tsh);
        }
      }
      children = new_children;
      new_children.clear();
    }
    children.clear();
    printf("total nodes: %d\n", total_children + 1);
  }

  void main_loop(float dt)
  {

  }
  void consume_key_down(SDL_Keycode key)
  {
    auto root_th = application->transform_system.get_handle(root);
    auto& t = application->transform_system.get_local_transform(root_th);
    if (key == SDLK_UP) {
      t.position += v2(0, 10);
    }
  }
  void consume_key_up(SDL_Keycode key)
  {
    if (key == SDLK_UP) {

    }
  }
};

class case1
{
protected:
  application<case1>* application;

  case1() {}

private:
  name_system name_system;

  const char* str_map =
    "xxxxxxxxxxxxxxxxxxxx"
    "xvooooooooooooooooox"
    "xxxxxxxxxxoxoxxxooxx"
    "xooooovoooooooooooox"
    "xoooooxooooooooooxox"
    "xoooooooxooxxxxxxxxx"
    "xooovoxoooooooooooox"
    "xoooxoooooooooooooox"
    "xooxxxooooooooooooox"
    "xxxxxxxxxxxxxxxooxxx"
    "xoooooooooooooooooox"
    "xoovooooooooooooxoox"
    "xoooxoxoxoxoxoxoxoox"
    "xvooooooooooooooooxx"
    "xxxxxxxxxxxxxxxxxxxx"
;

  const int map_width = 20;
  const int map_height = 15;
  const int tile_width = 32;
  const int tile_height = 32;
  const int char_width = 28;
  const int char_height = 26;

public:
  entity root_e = entity::invalid;
  entity char_e = entity::invalid;
  transform_system::handle char_th = entity::invalid;

  void init()
  {
    auto& a = application;

    root_e = a->entity_system.make();
    auto root_th = a->transform_system.link(root_e);
    auto window_size = a->get_window_size();
    auto& root_t = application->transform_system.get_local_transform(root_th);
    root_t.position = v2(tile_width, tile_width);
    root_t.scale *= 2.5;
    auto c_path = std::string(SDL_GetBasePath());
    auto tile_tex = a->texture_system.link(root_e, (c_path + "tile.png").c_str());
    auto morcow_tex = a->texture_system.link(root_e, (c_path + "morcow.png").c_str());
    auto calicker_tex = a->texture_system.link(root_e, (c_path + "calicker.png").c_str());
    for (int i = 0; i < strlen(str_map); i++) {
      int x = i % map_width;
      int y = i / map_width;
      if (str_map[i] == 'x') {
        auto e = a->entity_system.make();
        auto th = a->transform_system.link(e, transform(v2(x * tile_width, y * tile_height)), root_th);
        a->solid_quads.link(e, quad(v2(tile_width, tile_height), 0xffffffff), th, tile_tex);
      } else if (str_map[i] == 'v') {
        auto e = a->entity_system.make();
        auto th = a->transform_system.link(e, transform(v2(x * tile_width, y * tile_height), v2(1.0f, 0.0f)), root_th);
        auto qh = a->solid_quads.link(e, quad(v2(10, 20), 0xffffffff), th, morcow_tex);
        morcowki[i] = qh;
      }
    }

    char_e = a->entity_system.make();
    char_th = a->transform_system.link(char_e, transform(v2(64, 64), v2(1.0f, 0.0f)), root_th);
    a->solid_quads.link(char_e, quad(v2(char_width, char_height), 0xffffffff, v2(0.0f, 0.0f)), char_th, calicker_tex);
  }

  v2 char_v = v2(0, 0);
  v2 jump_v = v2(0, 0);

  std::unordered_map<int, solid_quads::handle> morcowki;

  void main_loop(float dt)
  {
    auto char_v_m = char_v;
    if (is_in_contact_with[0] && char_v_m.y < 0.0f) {
      char_v_m = v2(char_v_m.x, 0.0f);
      jump_v = v2(0, 0);
    }
    if (is_in_contact_with[2] && char_v_m.y > 0.0f) {
      char_v_m = v2(char_v_m.x, 0.0f);
    }
    if (is_in_contact_with[1] && char_v.x > 0.0f) {
      char_v_m = v2(0.0f, char_v_m.y);
    }
    if (is_in_contact_with[3] && char_v.x < 0.0f) {
      char_v_m = v2(0.0f, char_v_m.y);
    }
    char_v_m *= 2.0f;
    auto& t = application->transform_system.get_local_transform(char_th);
    jump_v *= 0.9f;
    auto a = jump_v * 10;
    auto offset = (char_v_m + a + (!is_in_contact_with[2] ? v2(0.0f, 2.0f) : v2(0.0f, 0.0f))) * 100 * dt;

    auto t0 = t.position;
    auto t1 = t.position + offset;
    float tp = 1.0f;

    auto lerp = [](v2 a, v2 b, float t) {
      return a * (1.0 - t) + (b * t);
    };

    auto get_corners = [=](v2 char_position) {
      v2 fix = v2(tile_width / 2, tile_height / 2);
      auto p = char_position + fix;
      // left top
      auto grid_p_0 = p;
      // right top
      auto grid_p_1 = v2(p.x + char_width, p.y);
      // right bottom
      auto grid_p_2 = v2(p.x + char_width, p.y + char_height);
      // left bottom
      auto grid_p_3 = v2(p.x, p.y + char_height);
      return std::vector<v2> { grid_p_0, grid_p_1, grid_p_2, grid_p_3 };
    };

    auto get_cells = [=](v2 char_position) {
      v2 fix = v2(tile_width / 2, tile_height / 2);
      auto p = char_position + fix;
      // left top
      auto grid_p_0 = iv2(p.x / tile_width, p.y / tile_height);
      // right top
      auto grid_p_1 = iv2((p.x + char_width) / tile_width, p.y / tile_height);
      // right bottom
      auto grid_p_2 = iv2((p.x + char_width) / tile_width, (p.y + char_height) / tile_height);
      // left bottom
      auto grid_p_3 = iv2(p.x / tile_width, (p.y + char_height) / tile_height);
      return std::vector<iv2> { grid_p_0, grid_p_1, grid_p_2, grid_p_3 };
    };

    auto get_box = [=](int x, int y) {
      auto bx = x * tile_width;
      auto by = y * tile_height;
      return std::vector<v2>  {
        v2(bx, by),
        v2(bx + tile_width, by),
        v2(bx + tile_width, by + tile_height),
        v2(bx, by + tile_height)
      };
    };


    auto is_any_collision = [=, &t, &tp]() {
      auto ps0 = get_cells(t.position);
      auto ps1 = get_cells(lerp(t0, t1, tp));
      for (auto pi : { 0, 1, 2, 3 }) {
        auto ix0 = ps0[pi].x;
        auto iy0 = ps0[pi].y;
        auto ix1 = ps1[pi].x;
        auto iy1 = ps1[pi].y;
        if (ix0 == ix1 && iy0 == iy1) {
          continue;
        }
        if (ix1 < 0 || ix1 >= map_width || iy1 < 0 || iy1 >= map_height) {
          continue;
        }
        if (str_map[iy1 * map_width + ix1] != 'x') {
          continue;
        }
        return true;
      }
      return false;
    };

    while (true) {
      if (!is_any_collision()) {
        break;
      }
      tp *= 0.9f;
    }

    t.position = lerp(t0, t1, tp);

    iv2 offsets[] = {
      iv2(0, -1),
      iv2(1, 0),
      iv2(0, 1),
      iv2(-1, 0),
    };

    int v2s[] = {
      1, 0, 1, 0
    };

    float near_off[] = {
      tile_height, 0, 0, tile_width,
    };

    auto ps0 = get_cells(t.position);
    auto ps0c = get_corners(t.position);
    for (int i = 0; i < 4; i++) {
      is_in_contact_with[i] = false;
      int i0 = i % 4;
      int i1 = (i + 1) % 4;
      for (auto j : { i0, i1 }) {
        // todo range check for map tiles
        auto inext = ps0[j] + offsets[i];
        if (str_map[inext.y * map_width + inext.x] != 'x') {
          continue;
        }
        if (abs(ps0c[j][v2s[i]] - inext[v2s[i]] * tile_height - near_off[i]) < 0.01f) {
          is_in_contact_with[i] = true;
        }
      }
    }

    auto get_morcowki = [=, &t, &tp]() {
      auto ps0 = get_cells(t.position);
      for (auto pi : { 0, 1, 2, 3 }) {
        auto ix0 = ps0[pi].x;
        auto iy0 = ps0[pi].y;
        int index = iy0 * map_width + ix0;
        if (str_map[index] == 'v' && morcowki.contains(index)) {
          auto qh = morcowki[index];
          auto& q = application->solid_quads.get_quad(qh);
          q.color = 0x00ffffff;
          morcowki.erase(index);
          if (morcowki.empty()) {
            printf("\n\nKot! You've got to pay 5000 AMD now. Those mor-cows were imported from yu ass ei.\n\n\n\n\n\nNo really, HAPPY BIRTHDAY NOW!!!111!11");
          }
          continue;
        }
      }
    };

    get_morcowki();

    if (is_in_contact_with[0] || is_in_contact_with[2]) {
      jump_v = v2(0.0f, 0.0f);
    }

    //printf("is_in_contact_with: %d %d %d %d\n", is_in_contact_with[0], is_in_contact_with[1], is_in_contact_with[2], is_in_contact_with[3]);
  }

  bool is_in_contact_with[4] { false, false, false, false };

  void consume_key_down(SDL_Keycode key)
  {
    auto root_th = application->transform_system.get_handle(root_e);
    auto& root_t = application->transform_system.get_local_transform(root_th);
    auto& t = application->transform_system.get_local_transform(char_th);
    if (key == SDLK_z && is_in_contact_with[2]) {
      jump_v = v2(0, -1);
    } else if (key == SDLK_UP) {
      // char_v += v2(0, -1);
    } else if (key == SDLK_DOWN) {
      // char_v += v2(0, 1);
    } else if (key == SDLK_LEFT) {
      char_v += v2(-1, 0);
    } else if (key == SDLK_RIGHT) {
      char_v += v2(1, 0);
    } else if (key == SDLK_q) {
      root_t.scale *= 1.1f;
    } else if (key == SDLK_e) {
      root_t.scale *= 0.9f;
    }
  }
  void consume_key_up(SDL_Keycode key)
  {
    if (key == SDLK_z) {
      jump_v = v2(0, 0);
    } else if (key == SDLK_UP) {
      // char_v = v2(char_v.x, 0);
    } else if (key == SDLK_DOWN) {
      // char_v = v2(char_v.x, 0);
    } else if (key == SDLK_LEFT) {
      char_v -= v2(-1.0, 0.0f);
    } else if (key == SDLK_RIGHT) {
      char_v -= v2(1.0f, 0.0f);
    }
  }
};

// why? why here?
unsigned g_seed = 0;

int main(int argc, char** argv)
{
  printf(SDL_GetBasePath());
  application<case1> a;
  a.init();
  a.start();
  return 0;
}
