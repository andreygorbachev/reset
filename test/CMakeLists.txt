project(reset-test)

add_executable(${PROJECT_NAME}
  resets_math.cpp
  resets.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  reset
  calendar
  day-count
  Boost::multiprecision
  GTest::gtest_main
)
# should day-count come in via reset?

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
