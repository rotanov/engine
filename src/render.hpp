#pragma once

#include "SDL.h"
#include "bgfx/bgfx.h"
#include "bgfx/bgfxplatform.h"
#include "bx/fpumath.h"

#include "v2.hpp"
#include "iv2.hpp"
#include "system.hpp"
#include "transform.hpp"
#include "shaders/fs.bin.h"
#include "shaders/vs.bin.h"

// TODO: rename to reflect this being a solid 2d quad
struct vertex
{
  v2 pos;
  uint32_t abgr;

  static void init()
  {
    ms_decl
      .begin()
      .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
      .end();
  };

  static bgfx::VertexDecl ms_decl;
};

class renderer
{
private:
  const bgfx::Memory* vs_ = nullptr;
  const bgfx::Memory* fs_ = nullptr;
  bgfx::ShaderHandle vsh;
  bgfx::ShaderHandle fsh;
  bgfx::ProgramHandle program;
  uint8_t* solid_quads_buffer_memory = nullptr;
  uint32_t solid_quads_vertex_count = 0;
  uint32_t solid_quads_vertex_allocated = 32000;
  bgfx::DynamicVertexBufferHandle vbh;
  bgfx::TransientVertexBuffer tvb;
public:
  uint16_t screen_width = 800;
  uint16_t screen_height = 600;

  void init()
  {
    switch (bgfx::getRendererType())
    {
    case bgfx::RendererType::Direct3D9:
      vs_ = bgfx::makeRef(vs_dx9, sizeof(vs_dx9));
      fs_ = bgfx::makeRef(fs_dx9, sizeof(fs_dx9));
      break;
    case bgfx::RendererType::Direct3D11:
    case bgfx::RendererType::Direct3D12:
      vs_ = bgfx::makeRef(vs_dx11, sizeof(vs_dx11));
      fs_ = bgfx::makeRef(fs_dx11, sizeof(fs_dx11));
      break;
    default:
      vs_ = bgfx::makeRef(vs_glsl, sizeof(vs_glsl));
      fs_ = bgfx::makeRef(fs_glsl, sizeof(fs_glsl));
      break;
    }

    vsh = bgfx::createShader(vs_);
    fsh = bgfx::createShader(fs_);
    program = bgfx::createProgram(vsh, fsh, true);

    int size = solid_quads_vertex_allocated * vertex::ms_decl.getStride();
    solid_quads_buffer_memory = new uint8_t[size];
    vbh = bgfx::createDynamicVertexBuffer(solid_quads_vertex_count, vertex::ms_decl);

    bgfx::allocTransientVertexBuffer(&tvb, 32000, vertex::ms_decl);
  }

  vertex* get_vertices(const uint32_t vertex_count)
  {
    solid_quads_vertex_count = vertex_count;
    //return reinterpret_cast<vertex*>(solid_quads_buffer_memory);
    bgfx::allocTransientVertexBuffer(&tvb, vertex_count, vertex::ms_decl);
    return reinterpret_cast<vertex*>(tvb.data);
  }

  void shutdown()
  {
    bgfx::destroyProgram(program);
    bgfx::shutdown();
  }

  void begin_frame()
  {
    bgfx::setViewRect(0, 0, 0, screen_width, screen_height);
    bgfx::touch(0);
    float at[3] = { 0.0f, 0.0f, 0.0f };
    float eye[3] = { 0.0f, 0.0f, -50.0f };
    float view[16];
    bx::mtxLookAt(view, eye, at);
    float proj[16];
    bx::mtxOrtho(proj, 0.0f, screen_width, screen_height, 0.0f, 0.0f, 1000.f);
    bgfx::setViewTransform(0, NULL, proj);
    bgfx::setViewRect(0, 0, 0, screen_width, screen_height);
  }

  void render()
  {
    uint32_t size = solid_quads_vertex_allocated * vertex::ms_decl.getStride();
    bgfx::updateDynamicVertexBuffer(vbh, 0, bgfx::makeRef(solid_quads_buffer_memory, size));
    bgfx::setVertexBuffer(vbh, solid_quads_vertex_count);
    bgfx::setState(
      0
      | BGFX_STATE_CULL_CW
      | BGFX_STATE_RGB_WRITE
      | BGFX_STATE_ALPHA_WRITE
      | BGFX_STATE_DEPTH_TEST_LESS
      | BGFX_STATE_MSAA
      | BGFX_STATE_BLEND_ALPHA
      );
    bgfx::submit(0, program);

    bgfx::setVertexBuffer(&tvb, 0, solid_quads_vertex_count);
    bgfx::setState(
      0
      | BGFX_STATE_CULL_CW
      | BGFX_STATE_RGB_WRITE
      | BGFX_STATE_ALPHA_WRITE
      | BGFX_STATE_DEPTH_TEST_LESS
      | BGFX_STATE_MSAA
      | BGFX_STATE_BLEND_ALPHA
      );
    bgfx::submit(0, program);

    bgfx::dbgTextClear();
  }

  void end_frame()
  {
    bgfx::frame();
  }
};

struct quad
{
  v2 size;
  uint32_t color;
  v2 pivot;

  quad(const v2& size, const uint32_t color, const v2& pivot = v2(0.5f, 0.5f))
    : size(size)
    , color(color)
    , pivot(pivot)
  {

  }
};

//struct solid_quads : public system_base<solid_quads>
//{
//  std::vector<quad> quads;
//  std::vector<transform_system::handle> transform_handles;
//  handle link(const entity e, const quad q, const transform_system::handle h)
//  {
//    quads.push_back(q);
//    transform_handles.push_back(h);
//    return base::link(e, quads.size() - 1);
//  }
//  void draw(transform_system& transform_system, renderer& renderer)
//  {
//    vertex* v = renderer.get_vertices(quads.size() * 6);
//
//    for (uint32_t i = 0; i < quads.size(); i++) {
//      auto& wt = transform_system.get_world_transform(transform_handles[i]);
//      auto& q = quads[i];
//      auto t = q.pivot * q.size;
//      v2 v_min = -t;
//      v2 v_max = q.size - t;
//
//      v2 rot = wt.rotation * wt.scale;
//
//      v[0].abgr = q.color;
//
//      v[0].pos = rot.rotate(v_min) + wt.position;
//      v[1].pos = rot.rotate(v2(v_min.x, v_max.y)) + wt.position;
//      v[2].pos = rot.rotate(v_max) + wt.position;
//      v[5].pos = rot.rotate(v2(v_max.x, v_min.y)) + wt.position;
//
//      v[3].pos = v[0].pos;
//      v[4].pos = v[2].pos;
//
//      v[1].abgr = v[0].abgr;
//      v[2].abgr = v[0].abgr;
//      v[3].abgr = v[0].abgr;
//      v[4].abgr = v[0].abgr;
//      v[5].abgr = v[0].abgr;
//      v += 6;
//    }
//  }
//  quad& get_quad(const handle h)
//  {
//    return quads[h.id];
//  }
//};
