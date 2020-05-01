cmake_minimum_required(VERSION 3.15)

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
