cmake_minimum_required(VERSION 3.15)

project("reset" VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

include(FetchContent)

set(CALENDAR_BUILD_TESTS_AND_EXAMPLES Off)
set(FIN-CALENDAR_BUILD_TESTS_AND_EXAMPLES Off)

FetchContent_Declare(
  calendar
  GIT_REPOSITORY https://github.com/andreygorbachev/calendar.git
  GIT_TAG        main
)

# how can we use just day-count and not the rest of fin-calendar?
FetchContent_Declare(
  fin-calendar
  GIT_REPOSITORY https://github.com/andreygorbachev/fin_calendar.git
  GIT_TAG        main
)

FetchContent_MakeAvailable(
  calendar
  fin-calendar
)

FetchContent_Declare(
  Boost_config
  GIT_REPOSITORY https://github.com/boostorg/config.git
  GIT_TAG        boost-1.88.0
)

FetchContent_Declare(
  Boost_multiprecision
  GIT_REPOSITORY https://github.com/boostorg/multiprecision.git
  GIT_TAG        boost-1.88.0
)

FetchContent_MakeAvailable(
  Boost_config
  Boost_multiprecision
)

option(RESET_BUILD_TESTS_AND_EXAMPLES "Build all of reset's own tests and examples." On)

add_subdirectory(include)

if(${RESET_BUILD_TESTS_AND_EXAMPLES})

  set(BUILD_GMOCK Off)

  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.17.0
  )

  FetchContent_MakeAvailable(googletest)

  enable_testing()

  add_subdirectory(test)
  add_subdirectory(example)

endif()

#set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
#export(PACKAGE Reset)
