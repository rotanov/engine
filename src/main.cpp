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

// todo: consider using those typedefs though
// i'm unsure how much it's frown upon
typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

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

  enum class walk_state
  {
    none,
    left,
    right,
    left_then_right,
    right_then_left,
  };

  enum class jump_state
  {
    none,
    start_jump,
    jump,
    start_land,
    land,
  };

  // ---
  //     3
  //    ---
  // 2 | P | 0
  //    ---
  //     1
  // 0, 90, 180, 270 degrees directions
  // top and bottom should be swapped if global y axis changes from
  // downward to upward
  enum class direction_index
  {
    right = 0,
    down = 1,
    left  = 2,
    up = 3,
  };

  // jump_state = f(jump_state, walk_state, input, update, player_flags) ?
  // walk_state = f(jump_state, walk_state, input, update, player_flags) ?
  // player_flags is like if player can double jump or dash because they've got a powerup

  walk_state walk_state = walk_state::none;
  jump_state jump_state = jump_state::none;

  const float walk_speed = 200.0f;
  const float max_jump_height = -32.0f * 2.0f - char_height * 0.5f - 5.0f;
  const float jump_duration = 0.4f;
  const float landing_duration = 0.2f;

  const float jump_acceleration = -2.0f * max_jump_height / (jump_duration * jump_duration);
  const float initial_jump_velocity = 2.0f * max_jump_height / jump_duration;
  const float land_acceleration = -2.0f * max_jump_height / (landing_duration * landing_duration);

  std::unordered_map<int, solid_quads::handle> morcowki;


  v2 velocity { 0.0f, 0.0f };

  bool contact_up() { return contact_state[static_cast<int>(direction_index::up)]; }
  bool contact_down() { return contact_state[static_cast<int>(direction_index::down)]; }
  bool contact_left() { return contact_state[static_cast<int>(direction_index::left)]; }
  bool contact_right() { return contact_state[static_cast<int>(direction_index::right)]; }

  void main_loop(float dt)
  {
    auto& v = velocity;
    if (contact_up() && jump_state == jump_state::jump) {
      jump_state = jump_state::start_land;
    }
    if (contact_down() && jump_state == jump_state::land) {
      v.y = 0.0f;
      jump_state = jump_state::none;
    }
    if (jump_state == jump_state::none) {
      v.y = 0.0f;
    }
    v.x = 0.0f;
    if (walk_state == walk_state::left) {
      v.x = -walk_speed;
    }
    if (walk_state == walk_state::right) {
      v.x = walk_speed;
    }
    if (contact_right() && walk_state == walk_state::right) {
      v.x = 0.0f;
    }
    if (contact_left() && walk_state == walk_state::left) {
      v.x = 0.0f;
    }
    auto& t = application->transform_system.get_local_transform(char_th);
    auto a = jump_state != jump_state::land ? jump_acceleration : land_acceleration;
    if (jump_state == jump_state::start_jump) {
      v.y = initial_jump_velocity;
      jump_state = jump_state::jump;
    }
    if (contact_down() && jump_state == jump_state::none) {
      a = 0.0f;
    }
    v += v2(0.0f, a) * dt;
    if (jump_state == jump_state::jump && v.y > 0.0f) {
      jump_state = jump_state::start_land;
    }
    if (jump_state == jump_state::start_land) {
      v.y = 0;
      jump_state = jump_state::land;
    }

    auto offset = v * dt;

    auto t0 = t.position;
    auto t1 = t.position + offset;
    float tp = 1.0f;

    auto lerp = [](v2 a, v2 b, float t) {
      return a * (1.0 - t) + (b * t);
    };

    auto get_corners = [=](v2 char_position) {
      v2 fix = v2(tile_width / 2, tile_height / 2);
      auto p = char_position + fix;
      auto left_top = p;
      auto right_top = v2(p.x + char_width, p.y);
      auto right_bottom = v2(p.x + char_width, p.y + char_height);
      auto left_bottom = v2(p.x, p.y + char_height);
      return std::vector<v2> { right_top, right_bottom, left_bottom, left_top };
    };

    auto get_cells = [=](v2 char_position) {
      v2 fix = v2(tile_width / 2, tile_height / 2);
      auto p = char_position + fix;
      auto left_top = iv2(p.x / tile_width, p.y / tile_height);
      auto right_top = iv2((p.x + char_width) / tile_width, p.y / tile_height);
      auto right_bottom = iv2((p.x + char_width) / tile_width, (p.y + char_height) / tile_height);
      auto left_bottom = iv2(p.x / tile_width, (p.y + char_height) / tile_height);
      return std::vector<iv2> { right_top, right_bottom, left_bottom, left_top };
    };

    auto is_any_collision = [=, &t, &tp]() {
      auto ps0 = get_cells(t.position);
      auto ps1 = get_cells(lerp(t0, t1, tp));
      for (int pi = 0; pi < 4; pi++) {
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

    int iterationsToResolveCollision = 0;
    while (true) {
      iterationsToResolveCollision++;
      if (!is_any_collision()) {
        break;
      }
      tp *= 0.9f;
    }
    printf("iterationsToResolveCollision: %d\n", iterationsToResolveCollision);

    t.position = lerp(t0, t1, tp);

    iv2 offsets[] = {
      iv2(1, 0),
      iv2(0, 1),
      iv2(-1, 0),
      iv2(0, -1),
    };

    int v2s[] = {
      0, 1, 0, 1
    };

    float near_off[] = {
      0, 0, tile_width, tile_height,
    };

    auto ps0 = get_cells(t.position);
    auto ps0c = get_corners(t.position);
    for (int i = 0; i < 4; i++) {
      contact_state[i] = false;
      int i0 = i % 4;
      int i1 = (i + 1) % 4;
      for (auto j : { i0, i1 }) {
        // todo range check for map tiles for when our tiles will be smaller
        // than object rectangle
        auto i_next = ps0[j] + offsets[i];
        if (str_map[i_next.y * map_width + i_next.x] != 'x') {
          continue;
        }
        if (abs(ps0c[j][v2s[i]] - i_next[v2s[i]] * tile_height - near_off[i]) < 0.01f) {
          contact_state[i] = true;
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
            printf(
              "\n\nKot! You've got to pay 5000 AMD now. "
              "Those mor-cows were imported from yu ass ei."
              "\n\n\n\n\n\nNo really, HAPPY BIRTHDAY NOW!!!111!11"
            );
          }
          continue;
        }
      }
    };

    get_morcowki();

    // we're checking for contact state in input handling
    // but we're also making contact state affect jump state here.
    // so may be we should trust jump state to be set on update?
    switch (jump_state) {
    case jump_state::none:
    {
      if (!contact_down()) {
        jump_state = jump_state::start_land;
      }
    }
    case jump_state::start_jump:; break;
    case jump_state::jump:
    {
      if (contact_up()) {
        jump_state = jump_state::start_land;
      }
      break;
    }
    case jump_state::land:
    {
      if (contact_down()) {
        jump_state = jump_state::none;
      }
    }
    }

    //printf(
    //  "contact state:\n\tright: %d\n\tdown: %d\n\tleft: %d\n\tup: %d\n",
    //  contact_right(),
    //  contact_down(),
    //  contact_left(),
    //  contact_up()
    //);
  }

  void paniq()
  {
    __debugbreak();
  }

  // ordered by direction_index
  bool contact_state[4]{ false, false, false, false };

  void consume_key_down(SDL_Keycode key)
  {
    auto root_th = application->transform_system.get_handle(root_e);
    auto& root_t = application->transform_system.get_local_transform(root_th);
    auto& t = application->transform_system.get_local_transform(char_th);
    if (key == SDLK_z) {
      switch (jump_state) {
      case jump_state::none:
      {
        if (contact_down()) {
          jump_state = jump_state::start_jump;
        }
      }
      break;
      case jump_state::jump: break;
      case jump_state::land: break;
      case jump_state::start_jump: break;
      case jump_state::start_land: break;
      }
    } else if (key == SDLK_UP) {
      // char_v += v2(0, -1);
    } else if (key == SDLK_DOWN) {
      // char_v += v2(0, 1);
    } else if (key == SDLK_LEFT) {
      switch (walk_state) {
      case walk_state::none: walk_state = walk_state::left; break;
      case walk_state::right: walk_state = walk_state::right_then_left; break;
      case walk_state::left:
      case walk_state::right_then_left:
      case walk_state::left_then_right: paniq();
      }
    } else if (key == SDLK_RIGHT) {
      switch (walk_state) {
      case walk_state::none: walk_state = walk_state::right; break;
      case walk_state::left: walk_state = walk_state::left_then_right; break;
      case walk_state::right:
      case walk_state::right_then_left:
      case walk_state::left_then_right: paniq();
      }
    } else if (key == SDLK_q) {
      root_t.scale *= 1.1f;
    } else if (key == SDLK_e) {
      root_t.scale *= 0.9f;
    }
  }
  void consume_key_up(SDL_Keycode key)
  {
    if (key == SDLK_z) {
      switch (jump_state) {
      case jump_state::jump: jump_state = jump_state::start_land; break;
      case jump_state::none:
      case jump_state::land: break;
      case jump_state::start_jump: break;
      case jump_state::start_land: break;
      }
    } else if (key == SDLK_UP) {
    } else if (key == SDLK_DOWN) {
    } else if (key == SDLK_LEFT) {
      switch (walk_state) {
      case walk_state::left: walk_state = walk_state::none; break;
      case walk_state::left_then_right: walk_state = walk_state::right; break;
      case walk_state::right_then_left: walk_state = walk_state::right; break;
      case walk_state::none:
      case walk_state::right: paniq();
      }
    } else if (key == SDLK_RIGHT) {
      switch (walk_state) {
      case walk_state::right: walk_state = walk_state::none; break;
      case walk_state::left_then_right: walk_state = walk_state::left; break;
      case walk_state::right_then_left: walk_state = walk_state::left; break;
      case walk_state::none:
      case walk_state::left: paniq();
      }
    }
  }
};

// why? why here?
unsigned g_seed = 0;

int main(int argc, char** argv)
{
  application<case1> a;
  a.init();
  a.start();
  return 0;
}
