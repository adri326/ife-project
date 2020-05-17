cmake_minimum_required(VERSION 3.15)
project(ife-project)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_C_STANDARD 11)

set(SRC_FILES
  src/ai.c
  src/display.c
  src/rules.c
)

set(
  HEADER_FILES
  src/ai.h
  src/display.h
  src/rules.h
)

add_executable(ife-project src/main.c ${SRC_FILES} ${HEADER_FILES})

if(MINGW AND (NOT DEFINED MINGW_USE_SYSTEM_LIBRARIES))
  set(SDL2_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2-2.0.12/x86_64-w64-mingw32")
  # No SDL2_GFX, might need rotozoom later, it will be compiled with the project then
  set(SDL2_IMAGE_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2_image-2.0.5/x86_64-w64-mingw32")
  set(SDL2_MIXER_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2_image-2.0.5/x86_64-w64-mingw32")
  set(SDL2_TTF_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2_image-2.0.5/x86_64-w64-mingw32")
endif()

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(
  ${SDL2_INCLUDE_DIRS}
  ${SDL2_IMAGE_INCLUDE_DIRS}
  ${SDL2_MIXER_INCLUDE_DIRS}
  ${SDL2_TTF_INCLUDE_DIRS}
)
target_link_libraries(ife-project
  SDL2::SDL2
  SDL2::SDL2image
  SDL2::SDL2mixer
  SDL2::SDL2ttf
)


project(ife-project-test)
enable_testing()
add_test(NAME ife-project-test COMMAND ife-project-test)

add_executable(
  ife-project-test
  test/main.c
  test/test.h
  test/rules.c
  test/rules.h
  ${SRC_FILES}
)

set(NO_TEST_DEPTH "false" CACHE STRING "true or false")

if(NO_TEST_DEPTH STREQUAL "true")
  message("TEST_DEPTH disabled.")
  add_compile_definitions(NO_TEST_DEPTH)
else()
  message("TEST_DEPTH enabled. Disable with -DNO_TEST_DEPTH=true if you experience issues with global variables in the test files")
endif()

if(NOT MSVC)
  target_compile_options(ife-project-test BEFORE PUBLIC "SHELL:-Wno-unused-variable")
endif()
