project("${PROJECT_NAME}_ZARONIA" LANGUAGES CXX)

configure_file(data/SARB-benchmark-data.csv SARB-benchmark-data.csv COPYONLY)
configure_file(data/ZARONIA-Period-Averages-and-Index.csv ZARONIA-Period-Averages-and-Index.csv COPYONLY)

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
  data/SARB-benchmark-data.csv
  data/ZARONIA-Period-Averages-and-Index.csv
)
