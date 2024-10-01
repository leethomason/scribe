cmake_minimum_required(VERSION 3.28)
include(FetchContent)
cmake_policy(SET CMP0135 NEW)
cmake_policy(SET CMP0091 NEW)

project(scribe)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        e57ca2e3685b160617d3d95fcd9e789c4e06ca88 # 10.1.0
)
FetchContent_Declare(
  argh
  GIT_REPOSITORY https://github.com/adishavit/argh.git
  GIT_TAG        d17062c870b5919f6d1eae7fe12879869a893b32
)
FetchContent_MakeAvailable(fmt argh)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.h")

add_executable(scribe 
  ${SOURCES} 
  ${HEADERS}
  ${fmt_SOURCE_DIR}/src/format.cc
)

target_include_directories(scribe PRIVATE
  ${fmt_SOURCE_DIR}/include 
  ${argh_SOURCE_DIR}
)

set_target_properties(scribe PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "..")


