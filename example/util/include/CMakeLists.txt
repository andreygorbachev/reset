# project "example_util"

add_library(${PROJECT_NAME} INTERFACE
  parsers.h
  checkers.h
)

target_include_directories(${PROJECT_NAME} INTERFACE .)
