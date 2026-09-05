project("${PROJECT_NAME}_test" LANGUAGES CXX)

add_executable(${PROJECT_NAME}
  reset_math.cpp
  scaled_value.cpp
  rate.cpp
  fixings.cpp
  index.cpp
  average.cpp
  accrued.cpp
  term.cpp
  compounded.cpp
  setup.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  reset
  calendar_static-data
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
