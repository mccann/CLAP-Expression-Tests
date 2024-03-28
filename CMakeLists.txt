cmake_minimum_required(VERSION 3.20.2)

project(CLAP-Expression-Tests)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
  clap
  GIT_REPOSITORY https://github.com/free-audio/clap.git
  GIT_TAG 1.2.0
)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.2.0
)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.90.4
)

FetchContent_Declare(
  clap_imgui_support
  GIT_REPOSITORY https://github.com/free-audio/clap-imgui-support.git
)

FetchContent_MakeAvailable(fmt clap imgui)

set(IMGUI_SOURCE_DIR ${imgui_SOURCE_DIR})

FetchContent_MakeAvailable(clap_imgui_support)

add_subdirectory(note-generator)
add_subdirectory(note-logger)