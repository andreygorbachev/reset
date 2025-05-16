cmake_minimum_required(VERSION 3.15)

project("reset" VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

include(FetchContent)

set(GREGORIAN_CALENDAR_MINIMAL TRUE)

FetchContent_Declare(
  calendar
  GIT_REPOSITORY https://github.com/andreygorbachev/calendar.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(calendar)

set(FIN_CALENDAR_MINIMAL TRUE)

FetchContent_Declare(
  fin-calendar
  GIT_REPOSITORY https://github.com/andreygorbachev/fin-calendar.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(fin-calendar)

set(BOOST_INCLUDE_LIBRARIES multiprecision)
set(BOOST_MP_STANDALONE TRUE) # this is still not working

FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG        boost-1.88.0
)
FetchContent_MakeAvailable(Boost)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.16.0
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(include)

if(NOT RESET_MINIMAL)
  add_subdirectory(test)
  add_subdirectory(example)
endif()

#set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
#export(PACKAGE Reset)
