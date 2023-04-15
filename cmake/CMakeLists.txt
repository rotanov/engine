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

if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
  add_compile_options("/Zc:__cplusplus")
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

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

# ExternalProject_Add(bgfx
#   PREFIX ${CMAKE_BINARY_DIR}/bgfx
#   DOWNLOAD_COMMAND ""
#   SOURCE_DIR ${CMAKE_SOURCE_DIR}/bgfx
#   CMAKE_ARGS
#     -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
# )

ExternalProject_Add(bgfx.cmake
  PREFIX ${CMAKE_BINARY_DIR}/bgfx.cmake
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/../3rd/bgfx.cmake
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
include_directories(
  ${CMAKE_INSTALL_PREFIX}/include/SDL2
  ../3rd/bgfx.cmake/bgfx/include
  ../3rd/bgfx.cmake/bx/include
  ../3rd/bgfx.cmake/bimg/include
  ../3rd/bgfx.cmake/bx/include/compat/msvc
  ../3rd/lua/src
  ../3rd/mflect/mflect
)

# variable_watch(SOURCES)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "../src/*.hpp" "../src/*.cpp")

add_executable(engine ${SOURCES})

if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
  target_compile_features(engine INTERFACE cxx_std_20)
  target_compile_options(engine INTERFACE "/Zc:__cplusplus")
endif()

target_compile_definitions(engine PUBLIC "BX_CONFIG_DEBUG=1")
target_compile_definitions(engine PUBLIC "__STDC_LIMIT_MACROS")
target_compile_definitions(engine PUBLIC "__STDC_FORMAT_MACROS")
target_compile_definitions(engine PUBLIC "__STDC_CONSTANT_MACROS")

add_dependencies(engine lua)
add_dependencies(engine sdl)
add_dependencies(engine bgfx.cmake)

target_link_libraries(
  engine
  ../lib/lua
  ../lib/bgfx
  bx
  bimg
  bimg_decode
  SDL2
  SDL2main
  Winmm
  Version
  Imm32
  psapi
)

install(TARGETS engine RUNTIME DESTINATION bin)
