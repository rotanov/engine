cmake_minimum_required(VERSION 2.8)

cmake_policy(SET CMP0020 NEW)

project(editor)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core)
find_package(Qt5Widgets)

QT5_WRAP_UI(UI_HEADERS
    src/main_window.ui
)

add_executable(editor WIN32 src/main.cpp src/main_window.cpp ${UI_HEADERS})

if(WIN32 AND Qt5Core_FOUND)
  # this snippet for locating windeployqt is taken from wireshark source
  get_target_property(_qmake_location Qt5::qmake IMPORTED_LOCATION)
  get_filename_component(_qt_bin_path "${_qmake_location}" DIRECTORY)
  find_program(QT_WINDEPLOYQT_EXECUTABLE windeployqt
    HINTS "${_qmake_location}"
    DOC "Path to the windeployqt utility."
  )
  if (NOT "${QT_WINDEPLOYQT_EXECUTABLE}" STREQUAL "QT_WINDEPLOYQT_EXECUTABLE-NOTFOUND")
    set(QT_BIN_PATH "${_qt_bin_path}" CACHE INTERNAL
      "Path to qmake, windeployqt, and other Qt utilities."
    )
    add_custom_target(copy_qt_dlls ALL)
    set_target_properties(copy_qt_dlls PROPERTIES FOLDER "Copy Tasks")
    add_custom_command(TARGET copy_qt_dlls
      POST_BUILD
      COMMAND set "PATH=%PATH%;${QT_BIN_PATH}"
      COMMAND "${QT_WINDEPLOYQT_EXECUTABLE}"
        $<$<CONFIG:Debug>:--debug>
        $<$<NOT:$<CONFIG:Debug>>:--release>
        --no-compiler-runtime
        "$<TARGET_FILE:editor>"
    )
    add_dependencies(copy_qt_dlls editor)
  endif()
endif(WIN32 AND Qt5Core_FOUND)

target_link_libraries(editor Qt5::Widgets)
