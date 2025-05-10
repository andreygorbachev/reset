project(reset)

add_library(${PROJECT_NAME} INTERFACE
  resets_math.h
  resets.h
)

target_include_directories(${PROJECT_NAME} INTERFACE .)
