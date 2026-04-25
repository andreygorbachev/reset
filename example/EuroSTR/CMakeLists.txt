project("${PROJECT_NAME}_EuroSTR" LANGUAGES CXX)

configure_file("data/Euro short-term rate.csv" "Euro short-term rate.csv" COPYONLY) # maybe no spaces in the file names?

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
  "data/Euro short-term rate.csv"
)
