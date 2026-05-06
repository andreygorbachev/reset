project("${PROJECT_NAME}_F-TIIE" LANGUAGES CXX)

configure_file("data/CF101.csv" "CF101.csv" COPYONLY)

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
  "data/CF101.csv"
)
