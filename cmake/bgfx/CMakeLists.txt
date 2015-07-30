cmake_minimum_required(VERSION 2.8)

project(bgfx)

set(SRC_BASE ${PROJECT_SOURCE_DIR}/../../3rd/bgfx/src)
set(BX_PATH ${SRC_BASE}/../../bx)

include_directories(${SRC_BASE} ${SRC_BASE}/../include  ${BX_PATH}/include
  ${BX_PATH}/include/compat/msvc ${SRC_BASE}/../3rdparty/khronos)

include(../utils.cmake)

set_sources(SOURCES amalgamated.cpp)

add_library(bgfx ${SOURCES})

install(TARGETS bgfx ARCHIVE DESTINATION lib)
