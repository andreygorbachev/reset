project("${PROJECT_NAME}_SELIC" LANGUAGES CXX)

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
