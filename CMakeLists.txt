cmake_minimum_required(VERSION 3.15)

project("reset" VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

include(FetchContent)

set(GREGORIAN_CALENDAR_MINIMAL On)

FetchContent_Declare(
  calendar
  GIT_REPOSITORY https://github.com/andreygorbachev/calendar.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(calendar)

set(FIN_CALENDAR_MINIMAL On)

FetchContent_Declare(
  fin-calendar
  GIT_REPOSITORY https://github.com/andreygorbachev/fin-calendar.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(fin-calendar)

FetchContent_Declare(
  Boost_config
  GIT_REPOSITORY https://github.com/boostorg/config.git
  GIT_TAG        boost-1.88.0
)
FetchContent_MakeAvailable(Boost_config)

FetchContent_Declare(
  Boost_multiprecision
  GIT_REPOSITORY https://github.com/boostorg/multiprecision.git
  GIT_TAG boost-1.88.0
)
FetchContent_MakeAvailable(Boost_multiprecision)

set(BUILD_GMOCK Off)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.17.0
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(include)

if(NOT RESET_MINIMAL)
  add_subdirectory(test)
  add_subdirectory(example)
endif()

#set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
#export(PACKAGE Reset)
