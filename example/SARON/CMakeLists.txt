project("${PROJECT_NAME}_SARON" LANGUAGES CXX)

configure_file(data/saron_compound_calculator.csv saron_compound_calculator.csv COPYONLY)

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
  data/saron_compound_calculator.csv
)
