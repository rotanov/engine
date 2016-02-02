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

class pes_application
{
protected:
  application<pes_application>* application;

  pes_application() {}

private:
  void test_case_lots_of_garbage(entity root)
  {
    auto& a = *application;
    auto sw = a.renderer.screen_width;
    auto sh = a.renderer.screen_height;
    const int size = 32.0f;

    auto root_tsh = a.transform_system.link(root, transform(v2(sw / 2, sh / 2), v2(0.0f), v2(0.5f, 0.5f)));
    a.solid_quads.link(root, quad(v2(64.0f, 64.0f), 0xffffff, v2(0.0f, 0.0f)), root_tsh);
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
              t
            , transform(
                  random_range(v2(-sw / 2, -sh / 2), v2(sw / 2, sh / 2)) / (i + 1)
                  , v2(random_range(0.0f, 3.1415f))
                  , v2(1.0f, 1.0f) / (float)(sqrt(i + 1.0f)))
            , n);
          a.solid_quads.link(
            t
            , quad(
            random_range(v2(size / 2.0f, size / 2.0f), v2(size, size))
            , 0x00FFFFFF & 0xfdf << (i * 3) | 0x44000000
            , v2(0.5f, 0.5f))
            , t_tsh);
          new_children.push_back(t_tsh);

          //t->color = (uint8_t)(i / 72.0f * 255) << 24 | 0x0000ff | 0xFF << i*3;
        }
      }
      children = new_children;
      new_children.clear();
    }
    children.clear();
    printf("total nodes: %d\n", total_children + 1);
  }

  void test_case_solar_system(entity root)
  {
    //entity next = new_entity(root);
    //entity third = new_entity(next);

    //init_entity(root, v2(0.0f, 0.0f), v2(32.0f, 32.0f));
    //init_entity(next, v2(0.5f, 0.5f), v2(32.0f, 32.0f));
    //init_entity(third, v2(0.5f, 0.5f), v2(32.0f, 32.0f));

    //position(next) = v2(32.0f, 0.0f);
    //position(third) = v2(32.0f, 0.0f);

    //color(root) = 0xffffffff;
    //color(next) = 0x880000ff;
    //color(third) = 0xff00ff00;

    //set_angle(root, 0.0f);
    //set_angle(next, 0.0f);
    //set_angle(third, 0.0f);

    //set_scale(root, v2(0.5f, 0.5f));
    //set_scale(next, v2(0.5f, 0.5f));
    //set_scale(third, v2(0.5f, 0.5f));
  }

public:
  entity root = entity::invalid;
  void init()
  {
    root = application->entity_system.make();
    application->transform_system.link(root);
    //test_case_lots_of_garbage(root);
    //auto rth = application->transform_system.link(root, transform(v2(100.0f, 100.0f), v2(1.2f), v2(1.0f, 1.0f)));
    //application->solid_quads.link(root, quad(v2(100.0f, 100.0f), 0xffffffff));

    //entity ech = application->entity_system.make();
    //auto chth = application->transform_system.link(ech, transform(v2(100.0f, 100.0f)), rth);
    //application->solid_quads.link(ech, quad(v2(32.0f, 32.0f), 0xffff0000));
    //
    ////test_case_lots_of_garbage(root);
    ////test_case_solar_system(root);
    //auto root_name = application->name_system.link(root);
    //application->name_system.name(root_name) = "pespespespespespespespes";
    //printf("%s\n", application->name_system.name(application->name_system.get_handle(root)).c_str());
  }

  void main_loop(float dt)
  {
    auto root_th = application->transform_system.get_handle(root);
    int l1 = random_int(0, 100);
    static std::vector<entity> es;
    for (int i = 0; i < l1; i++) {
      auto e = application->entity_system.make();
      es.push_back(e);
      application->transform_system.link(e, transform(v2(100.0f, 100.0f)), root_th);
    }
    int l2 = random_int(0, 100);
    for (int i = 0; i < std::min(l2, (int)es.size()); i++) {
      if (!application->entity_system.alive(es[i])) {
        es.erase(std::find(es.begin(), es.end(), es[i]));
        i--;
        continue;
      }
      application->entity_system.kill(es[i]);
      application->transform_system.unlink(es[i]);
    }
    printf("%d\n", application->entity_system.count());
    printf("%d\n", l2 > l1);
    //auto h = application->transform_system.get_handle(root);
    //auto& t = application->transform_system.get_local_transform(h);
    //t.rotation = v2(0.1f).rotate(t.rotation);
  }
};

unsigned g_seed = 0;

int main(int argc, char** argv)
{
  application<pes_application> app;
  app.init();
  app.main_loop();
  return 0;
}
