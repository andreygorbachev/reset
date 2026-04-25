project("${PROJECT_NAME}_F-TIIE" LANGUAGES CXX)

configure_file("data/Overnight TIIE Funding Rate.csv" "Overnight TIIE Funding Rate.csv" COPYONLY) # maybe no spaces in the file names?

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
  "data/Overnight TIIE Funding Rate.csv"
)
