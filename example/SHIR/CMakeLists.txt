project("${PROJECT_NAME}_SHIR" LANGUAGES CXX)

configure_file(data/BR.csv BR.csv COPYONLY)

add_executable(${PROJECT_NAME}
  example.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  reset_example_util
  reset
  calendar_static-data
)

# to make IDE to show this file as part of the project
target_sources(${PROJECT_NAME} PRIVATE
  data/BR.csv
)
