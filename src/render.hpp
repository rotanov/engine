#pragma once

#include "SDL.h"
#include "bx/math.h"
#include "bx/file.h"
#include "bgfx/bgfx.h"
#include "bgfx/embedded_shader.h"
#include "bimg/bimg.h"
#include <bx/commandline.h>
#include <bx/endian.h>
#include <bx/math.h>
#include <bx/readerwriter.h>
#include <bx/string.h>
#include <bimg/decode.h>

#include "v2.hpp"
#include "iv2.hpp"
#include "system.hpp"
#include "transform.hpp"
#include "shaders/fs.bin.h"
#include "shaders/vs.bin.h"


static const bgfx::EmbeddedShader s_embeddedShaders[] =
{
  BGFX_EMBEDDED_SHADER(vs),
  BGFX_EMBEDDED_SHADER(fs),

  BGFX_EMBEDDED_SHADER_END()
};

struct texture_record
{
  bgfx::TextureHandle texture_bgfx_handle;
};

// TODO: rename to reflect this being a solid 2d quad
//struct vertex
//{
//  v2 pos;
//  uint32_t abgr;
//
//  static void init()
//  {
//    ms_layout
//      .begin()
//      .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
//      .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
//      .end();
//  };
//
//  static bgfx::VertexLayout ms_layout;
//};

struct vertex_t
{
  v2 pos;
  uint32_t abgr;
  v2 uv;

  static void init()
  {
    ms_layout
      .begin()
      .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
      .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
      .end();
  };

  static bgfx::VertexLayout ms_layout;
};

#define DBG(A) PANIC(A)

static const bgfx::Memory* loadMem(bx::FileReaderI* _reader, const char* _filePath)
{
  if (bx::open(_reader, _filePath))
  {
    uint32_t size = (uint32_t)bx::getSize(_reader);
    const bgfx::Memory* mem = bgfx::alloc(size + 1);
    bx::read(_reader, mem->data, size, bx::ErrorAssert{});
    bx::close(_reader);
    mem->data[mem->size - 1] = '\0';
    return mem;
  }

  DBG("Failed to load %s.", _filePath);
  return NULL;
}

static void* loadMem(bx::FileReaderI* _reader, bx::AllocatorI* _allocator, const char* _filePath, uint32_t* _size)
{
  if (bx::open(_reader, _filePath))
  {
    uint32_t size = (uint32_t)bx::getSize(_reader);
    void* data = bx::alloc(_allocator, size);
    bx::read(_reader, data, size, bx::ErrorAssert{});
    bx::close(_reader);

    if (NULL != _size)
    {
      *_size = size;
    }
    return data;
  }

  DBG("Failed to load %s.", _filePath);
  return NULL;
}

static bx::DefaultAllocator s_allocator;

bimg::ImageContainer* imageLoad(const char* _filePath, bgfx::TextureFormat::Enum _dstFormat);

struct batch
{
  int vertex_count;
  int start_vertex;
  int start_index;
  int index_count;
  bgfx::TextureHandle texture_bgfx_handle;
  bgfx::TransientVertexBuffer tvb;
  bgfx::TransientIndexBuffer tib;
};

class renderer
{
private:
  const bgfx::Memory* vs_ = nullptr;
  const bgfx::Memory* fs_ = nullptr;
  bgfx::ShaderHandle vsh;
  bgfx::ShaderHandle fsh;
  bgfx::ProgramHandle program;

  bgfx::TransientVertexBuffer tvb;
  bgfx::TransientIndexBuffer tib;
  std::vector<batch> batches;
  bgfx::UniformHandle texture_uniform_sampler;
public:
  uint16_t screen_width = 1600;
  uint16_t screen_height = 1200;

  batch& get_batch(texture_record tr, int vertex_count, int index_count)
  {
    bgfx::TransientVertexBuffer tvb;
    bgfx::TransientIndexBuffer tib;
    bgfx::allocTransientBuffers(&tvb, vertex_t::ms_layout, vertex_count, &tib, index_count);
    auto top_batch = batch{
      .vertex_count = vertex_count,
      .start_vertex = 0,
      .start_index = 0,
      .index_count = index_count,
      .texture_bgfx_handle = tr.texture_bgfx_handle,
      .tvb = tvb,
      .tib = tib,
    };
    batches.push_back(top_batch);
    return batches.back();
    bool new_batch = false;
    int offset = 0;
    if (batches.size() == 0) {
      new_batch = true;
    } else if (batches.back().texture_bgfx_handle.idx == tr.texture_bgfx_handle.idx) {
      batches.back().vertex_count += vertex_count;
      return batches.back();
    } else {
      new_batch = true;
      offset = batches.back().start_vertex + batches.back().vertex_count;
    }

    if (new_batch) {
      auto top_batch = batch {
        .vertex_count = vertex_count,
        .start_vertex = offset,
        .texture_bgfx_handle = tr.texture_bgfx_handle
      };
      batches.push_back(top_batch);
    }
    return batches.back();
  }

  void init()
  {
    auto renderer_type = bgfx::getRendererType();
    bgfx::ShaderHandle vsh = bgfx::createEmbeddedShader(s_embeddedShaders, renderer_type, "vs");
    bgfx::ShaderHandle fsh = bgfx::createEmbeddedShader(s_embeddedShaders, renderer_type, "fs");
    program = bgfx::createProgram(vsh, fsh, true);
    texture_uniform_sampler = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    vertex_t::init();
  }

  vertex_t* get_vertices(const uint32_t vertex_count)
  {
    bgfx::allocTransientVertexBuffer(&tvb, vertex_count, vertex_t::ms_layout);
    return reinterpret_cast<vertex_t*>(tvb.data);
  }

  uint16_t* get_indices(const uint32_t index_count) {
    bgfx::allocTransientIndexBuffer(&tib, index_count, false);
    return (uint16_t*)tib.data;
  }

  void shutdown()
  {
    bgfx::destroy(program);
    bgfx::shutdown();
  }

  void begin_frame()
  {
    bx::Vec3 at{ 0.0f, 0.0f, 0.0f };
    bx::Vec3 eye{ 0.0f, 0.0f, -50.0f };
    float view[16];
    bx::mtxLookAt(view, eye, at);
    float proj[16];
    bx::mtxOrtho(proj, 0.0f, screen_width, screen_height, 0.0f, 0.0f, 1000.f, 0.0f, true);
    bgfx::setViewTransform(0, NULL, proj);
    bgfx::setViewRect(0, 0, 0, screen_width, screen_height);
    bgfx::touch(0);
  }

  void render()
  {
    for (auto& batch : batches) {
      bgfx::setState(0
        | BGFX_STATE_WRITE_RGB
        | BGFX_STATE_WRITE_A
        | BGFX_STATE_DEPTH_TEST_LESS
        | BGFX_STATE_CULL_CW
        | BGFX_STATE_MSAA
        | BGFX_STATE_BLEND_ALPHA
      );
      bgfx::setTexture(0, texture_uniform_sampler, batch.texture_bgfx_handle);
      bgfx::setVertexBuffer(0, &batch.tvb, 0, 4);
      bgfx::setIndexBuffer(&batch.tib, 0, 6);
      //bgfx::setVertexBuffer(0, &tvb, batch.start_vertex, batch.vertex_count);
      //bgfx::setIndexBuffer(&tib, batch.start_vertex / 4 * 6, batch.vertex_count / 4 * 6);
      bgfx::submit(0, program);
    }
    batches.clear();
    bgfx::dbgTextClear();
    //bgfx::frame();
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

struct texture_system : public system_specific<texture_system>
{
  std::vector<texture_record> textures;

  void init()
  {
    int t_size = 512;
    texture_record t;
    uint32_t data_size = t_size * t_size * 4;
    auto tdata = new uint8_t[data_size];
    for (int i = 0; i < t_size * t_size; i++)
    {
      float x = (float)((i % t_size) - t_size * 0.5f) / (float)t_size;
      float y = (float)((i / t_size) - t_size * 0.5f) / (float)t_size;
      float a;
      a = (0.25f - (x * x + y * y)) * 255.f;
      a = a * a * a * a * a;
      if (a < 0.0f) a = 0.0f;
      if (a > 255.0f) a = 255.0f;
      tdata[i * 4 + 0] = 255;
      tdata[i * 4 + 1] = 255;
      tdata[i * 4 + 2] = 255;
      tdata[i * 4 + 3] = a;
    }
    t.texture_bgfx_handle = bgfx::createTexture2D(
      t_size
      , t_size
      , false
      , 1
      , bgfx::TextureFormat::RGBA8
      , 0
      , bgfx::makeRef(tdata, data_size)
    );
    textures.push_back(t);
    base::link(entity::invalid);
  }

  handle link(const entity e, const char* texture_path)
  {
    texture_record t;
    bimg::ImageContainer* image = imageLoad(texture_path, bgfx::TextureFormat::RGBA8);
    auto mem = bgfx::copy(image->m_data, image->m_size);
    t.texture_bgfx_handle = bgfx::createTexture2D(
      image->m_width
      , image->m_height
      , false
      , 1
      , bgfx::TextureFormat::RGBA8
      , 0 //BGFX_TEXTURE_SRGB
      , mem
    );

    textures.push_back(t);
    return base::link(e);
  }
  texture_record& get_texture(const handle h)
  {
    if (h == handle::invalid) {
      return textures[0];
    }
    return textures[h.index];
  }
};

struct solid_quads : public system_specific<solid_quads>
{
  std::vector<quad> quads;
  std::vector<transform_system::handle> transform_handles;
  std::vector<texture_system::handle> texture_handles;
  handle link(const entity e, const quad q, const transform_system::handle transform_handle, const texture_system::handle texture_handle = texture_system::handle::invalid)
  {
    quads.push_back(q);
    transform_handles.push_back(transform_handle);
    texture_handles.push_back(texture_handle);
    return base::link(e);
  }
  void draw(transform_system& transform_system, texture_system& texture_system, renderer& renderer)
  {
    //vertex_t* v = renderer.get_vertices(quads.size() * 4);
    //uint16_t* index = renderer.get_indices(quads.size() * 6);
    for (uint32_t i = 0; i < quads.size(); i++) {
      auto& wt = transform_system.get_world_transform(transform_handles[i]);
      auto& tr = texture_system.get_texture(texture_handles[i]);
      auto& batch = renderer.get_batch(tr, 4, 6);
      auto& q = quads[i];
      auto t = q.pivot * q.size;
      v2 v_min = -t;
      v2 v_max = q.size - t;

      v2 rot = wt.rotation * wt.scale;

      auto v = (vertex_t*)batch.tvb.data;
      auto index = (uint16_t*)batch.tib.data;

      v[0].pos = rot.rotate(v_min) + wt.position;
      v[1].pos = rot.rotate(v2(v_min.x, v_max.y)) + wt.position;
      v[2].pos = rot.rotate(v_max) + wt.position;
      v[3].pos = rot.rotate(v2(v_max.x, v_min.y)) + wt.position;

      v[0].abgr = q.color;
      v[1].abgr = q.color;
      v[2].abgr = q.color;
      v[3].abgr = q.color;

      v[0].uv = v2(0.0f, 0.0f);
      v[1].uv = v2(0.0f, 1.0f);
      v[2].uv = v2(1.0f, 1.0f);
      v[3].uv = v2(1.0f, 0.0f);

      index[0] = 0;
      index[1] = 1;
      index[2] = 2;
      index[3] = 0;
      index[4] = 2;
      index[5] = 3;

      v += 4;
      index += 6;
    }
  }
  quad& get_quad(const handle h)
  {
    return quads[h.index];
  }
};
