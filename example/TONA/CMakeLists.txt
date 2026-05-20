project("${PROJECT_NAME}_TONA" LANGUAGES CXX)

configure_file(data/FM01.csv FM01.csv COPYONLY)

add_executable(${PROJECT_NAME}
  example.cpp
  parser.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  reset
  calendar_static-data
)

# to make IDE to show this file as part of the project
target_sources(${PROJECT_NAME} PRIVATE
  data/FM01.csv
)
