project("${PROJECT_NAME}_F-TIIE" LANGUAGES CXX)

configure_file(data/CF101.csv CF101.csv COPYONLY)

add_executable(${PROJECT_NAME}
  example.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  reset_example_util
  reset
  calendar_static-data
)

# to make IDE to show this file as part of the project
target_sources(${PROJECT_NAME} PRIVATE
  F-TIIE_index.h # should target_include_directories take care of a path here?
  F-TIIE_compounded.h
  data/CF101.csv
)
