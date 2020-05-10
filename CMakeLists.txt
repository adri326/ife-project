cmake_minimum_required(VERSION 3.15)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
project(ife-project)

set(SRC_FILES
  src/main.c
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

add_executable(ife-project ${SRC_FILES} ${HEADER_FILES})

if(MINGW AND (NOT DEFINED MINGW_USE_SYSTEM_LIBRARIES))
  set(SDL2_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2-2.0.12/x86_64-w64-mingw32")
  set(SDL2_GFX_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2_gfx-1.0.4") # This library looks like a mess to me. You will have to figure out how to compile it for yourself beforehand
  set(SDL2_IMAGE_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2_image-2.0.5/x86_64-w64-mingw32")
  set(SDL2_MIXER_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2_image-2.0.5/x86_64-w64-mingw32")
  set(SDL2_TTF_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2_image-2.0.5/x86_64-w64-mingw32")
endif(MINGW)

find_package(SDL2 REQUIRED)
find_package(SDL2_gfx REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(
  ${SDL2_INCLUDE_DIRS}
  ${SDL2_GFX_INCLUDE_DIRS}
  ${SDL2_IMAGE_INCLUDE_DIRS}
  ${SDL2_MIXER_INCLUDE_DIRS}
  ${SDL2_TTF_INCLUDE_DIRS}
)
target_link_libraries(ife-project
  SDL2::SDL2
  SDL2::SDL2gfx
  SDL2::SDL2image
  SDL2::SDL2mixer
  SDL2::SDL2ttf
)
