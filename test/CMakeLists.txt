project(reset-test)
#project(test)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.13.0
)
FetchContent_MakeAvailable(googletest)

add_executable(${PROJECT_NAME}
  round.cpp
  resets.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  reset
  Calendar::calendar
  CouponSchedule::coupon-schedule
  GTest::gtest_main
)
# should coupon-schedule come in via reset?

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}
)
