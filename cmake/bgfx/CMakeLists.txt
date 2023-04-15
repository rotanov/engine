cmake_minimum_required(VERSION 3.10.2)

project(bgfx)

if(POLICY CMP0072)
  cmake_policy(SET CMP0072 NEW)
endif()

if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
  add_compile_options("/Zc:__cplusplus")
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

set(SRC_BASE ${PROJECT_SOURCE_DIR}/../../3rd/bgfx/src/)
set(BX_PATH ${SRC_BASE}/../../bx/)
set(3RD_PATH ${PROJECT_SOURCE_DIR}/../../3rd/)
set(BGFX_PATH ${PROJECT_SOURCE_DIR}/../../3rd/bgfx/)
set(SHADERC_PATH ${BGFX_PATH}tools/shaderc)

include_directories(
  ${SRC_BASE}
  ${SRC_BASE}/../include
  ${BX_PATH}/include
  ${BX_PATH}/include/compat/msvc
  ${3RD_PATH}/bimg/include
  ${SRC_BASE}/../3rdparty/khronos
  ${SRC_BASE}/../3rdparty/directx-headers/include/directx
  ${SRC_BASE}/../3rdparty
)

include(../utils.cmake)

set_sources(SOURCES amalgamated.cpp)

add_library(bgfx ${SOURCES})

variable_watch(SHADERC_PATH)

file(GLOB_RECURSE SHADERC_SOURCES CONFIGURE_DEPENDS ${SHADERC_PATH}*.h ${SHADERC_PATH}*.cpp)

variable_watch(SHADERC_SOURCES)

add_executable(shaderc ${SHADERC_SOURCES})

target_compile_definitions(bgfx PUBLIC "BX_CONFIG_DEBUG=1")
target_compile_definitions(bgfx PUBLIC "__STDC_LIMIT_MACROS")
target_compile_definitions(bgfx PUBLIC "__STDC_FORMAT_MACROS")
target_compile_definitions(bgfx PUBLIC "__STDC_CONSTANT_MACROS")

if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
  target_compile_features(bgfx INTERFACE cxx_std_20)
  target_compile_options(bgfx INTERFACE "/Zc:__cplusplus")
endif()

target_compile_definitions(shaderc PUBLIC "BX_CONFIG_DEBUG=1")
target_compile_definitions(shaderc PUBLIC "__STDC_LIMIT_MACROS")
target_compile_definitions(shaderc PUBLIC "__STDC_FORMAT_MACROS")
target_compile_definitions(shaderc PUBLIC "__STDC_CONSTANT_MACROS")

if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
  target_compile_features(shaderc INTERFACE cxx_std_20)
  target_compile_options(shaderc INTERFACE "/Zc:__cplusplus")
endif()

install(TARGETS bgfx ARCHIVE DESTINATION lib)
install(TARGETS shaderc ARCHIVE DESTINATION bin)
