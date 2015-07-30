cmake_minimum_required(VERSION 2.8)

project(engine)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/..)

include(ExternalProject)

ExternalProject_Add(lua
  PREFIX ${CMAKE_BINARY_DIR}/lua
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/lua
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
)

ExternalProject_Add(bgfx
  PREFIX ${CMAKE_BINARY_DIR}/bgfx
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/bgfx
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
)

ExternalProject_Add(sdl
  PREFIX ${CMAKE_BINARY_DIR}/sdl
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/../3rd/sdl
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
)

link_directories(${CMAKE_INSTALL_PREFIX}/lib)
include_directories(${CMAKE_INSTALL_PREFIX}/include/SDL2
  ../3rd/bgfx/include
  ../3rd/bx/include
  ../3rd/lua/src
  )

add_executable(engine ../src/main.cpp)

add_dependencies(engine lua)
add_dependencies(engine sdl)
add_dependencies(engine bgfx)

target_link_libraries(engine ../lib/lua ../lib/bgfx SDL2 SDL2main Winmm Version Imm32)

install(TARGETS engine RUNTIME DESTINATION bin)
