project("${PROJECT_NAME}_SELIC" LANGUAGES CXX)

configure_file("data/SELIC Effective.csv" "SELIC Effective.csv" COPYONLY) # maybe no spaces in the file names?

add_executable(${PROJECT_NAME}
  example.cpp
  parser.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  reset
  calendar
  fin-calendar_day-count
  Boost::config
  Boost::multiprecision
)
