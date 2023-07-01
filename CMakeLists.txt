cmake_minimum_required(VERSION 3.15)

project("reset" VERSION 0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

find_package(Calendar)
find_package(CouponSchedule)
# maybe we should not have CouponSchedule here?

enable_testing()

add_subdirectory(include)
add_subdirectory(test)

export(TARGETS reset NAMESPACE Reset:: FILE ResetConfig.cmake)

set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
export(PACKAGE Reset)
