project(reset-example-SELIC)

add_executable(${PROJECT_NAME}
  example.cpp
  parser.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  reset
  calendar
  day-count
  Boost::config
  Boost::multiprecision
)
