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

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/../install/)

add_subdirectory(lua ./lua)
add_subdirectory(../3rd/bgfx.cmake ./bgfx)

set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
add_subdirectory(../3rd/sdl ./sdl 
  # EXCLUDE_FROM_ALL
)

# todo: remove and see what'll happen, because probably not needed anymore
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

# todo: glob assets
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/../assets/tile.png
  ${CMAKE_CURRENT_BINARY_DIR}/assets/tile.png
  COPYONLY
)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/../assets/morcow.png
  ${CMAKE_CURRENT_BINARY_DIR}/assets/morcow.png
  COPYONLY
)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/../assets/calicker.png
  ${CMAKE_CURRENT_BINARY_DIR}/assets/calicker.png
  COPYONLY
)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "../src/*.hpp" "../src/*.cpp")

add_executable(engine ${SOURCES})

if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
  target_compile_features(engine INTERFACE cxx_std_20)
  target_compile_options(engine INTERFACE "/Zc:__cplusplus")
endif()

# why?
target_compile_definitions(engine PUBLIC "BX_CONFIG_DEBUG=1")
target_compile_definitions(engine PUBLIC "__STDC_LIMIT_MACROS")
target_compile_definitions(engine PUBLIC "__STDC_FORMAT_MACROS")
target_compile_definitions(engine PUBLIC "__STDC_CONSTANT_MACROS")

target_link_libraries(
  engine
  lua
  bgfx
  bx
  bimg
  bimg_decode
  SDL2main
  SDL2-static
  Winmm
  Version
  Imm32
  psapi
)

message(${CMAKE_INSTALL_PREFIX})

install(TARGETS engine RUNTIME DESTINATION bin)
file(GLOB_RECURSE ASSETS ${CMAKE_CURRENT_BINARY_DIR}/assets/*)
install(FILES ${ASSETS} DESTINATION assets)