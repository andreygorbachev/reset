# project "example_util"

add_library(${PROJECT_NAME} INTERFACE
  parser.h
)

target_include_directories(${PROJECT_NAME} INTERFACE .)
