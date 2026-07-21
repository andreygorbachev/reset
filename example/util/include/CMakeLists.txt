# project "example_util"

add_library(${PROJECT_NAME} INTERFACE
  parser.h
  checker.h # should these be called parsers.h and checkers.h?
)

target_include_directories(${PROJECT_NAME} INTERFACE .)
