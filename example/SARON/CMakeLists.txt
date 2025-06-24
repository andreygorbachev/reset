project("${PROJECT_NAME}_SARON" LANGUAGES CXX)

configure_file(data/saron_compound_calculator.csv saron_compound_calculator.csv COPYONLY)

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
