# project "reset"

add_library(${PROJECT_NAME} INTERFACE
  resets_math.h
  scaled_value.h
  rate.h
  resets.h
  index.h
  average.h
  accrued.h
)

target_include_directories(${PROJECT_NAME} INTERFACE .)

target_link_libraries(${PROJECT_NAME} INTERFACE
  calendar
  fin-calendar_day-count
  fin-calendar_business-day-convention
  Boost::config
  Boost::multiprecision
)
