# project "reset"

add_library(${PROJECT_NAME} INTERFACE
  resets_math.h
  resets.h
  index.h
  rate.h
)

target_include_directories(${PROJECT_NAME} INTERFACE .)

target_link_libraries(${PROJECT_NAME} INTERFACE
  calendar
  fin-calendar_day-count
  Boost::config
  Boost::multiprecision
)
