project("${PROJECT_NAME}_test" LANGUAGES CXX)

add_executable(${PROJECT_NAME}
  resets_math.cpp
  resets.cpp
  index.cpp
  setup.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  reset
  calendar
  fin-calendar_day-count
  Boost::config
  Boost::multiprecision
  GTest::gtest_main
)
# should day-count come in via reset?

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
