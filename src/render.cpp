#include "render.hpp"

//bgfx::VertexLayout vertex::ms_layout;

bgfx::VertexLayout vertex_t::ms_layout;

bimg::ImageContainer* imageLoad(const char* _filePath, bgfx::TextureFormat::Enum _dstFormat)
{
  uint32_t size = 0;
  auto freader = new bx::FileReader();
  void* data = loadMem(freader, &s_allocator, _filePath, &size);

  return bimg::imageParse(&s_allocator, data, size, bimg::TextureFormat::Enum(_dstFormat), NULL);
}
