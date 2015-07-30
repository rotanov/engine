cmake_minimum_required(VERSION 2.8)

project(lua)

set(SRC_BASE ${PROJECT_SOURCE_DIR}/../../3rd/lua/src)
include_directories(${SRC_BASE})

include(../utils.cmake)

set_sources(SOURCES lapi.c lauxlib.c lbaselib.c lbitlib.c lcode.c lcorolib.c
  lctype.c ldblib.c ldebug.c ldo.c ldump.c lfunc.c lgc.c linit.c liolib.c llex.c
  lmathlib.c lmem.c loadlib.c lobject.c lopcodes.c loslib.c lparser.c lstate.c
  lstring.c lstrlib.c ltable.c ltablib.c ltm.c lundump.c lutf8lib.c lvm.c lzio.c
)

add_library(lua ${SOURCES})

install(TARGETS lua ARCHIVE DESTINATION lib)
