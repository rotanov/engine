cmake_minimum_required(VERSION 3.24)

project(engine)

if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(utils.cmake)

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
  ../3rd/mflect/mflect
)

set(SRC_BASE ../src/)

set_sources(SOURCES main.cpp
  application.cpp
  application.hpp
  entity.cpp
  entity.hpp
  error.cpp
  error.hpp
  handle.cpp
  handle.hpp
  iv2.hpp
  render.cpp
  render.hpp
  sparse_set.hpp
  system.hpp
  system.hpp
  tilemap.cpp
  tilemap.hpp
  transform.cpp
  transform.hpp
  utils.hpp
  v2.hpp
)

add_executable(engine ${SOURCES})

add_dependencies(engine lua)
add_dependencies(engine sdl)
add_dependencies(engine bgfx)

target_link_libraries(engine ../lib/lua ../lib/bgfx SDL2 SDL2main Winmm Version Imm32 psapi)

install(TARGETS engine RUNTIME DESTINATION bin)
