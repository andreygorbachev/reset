project("${PROJECT_NAME}_SARON" LANGUAGES CXX)

configure_file(data/hsrron.csv hsrron.csv COPYONLY)
configure_file(data/h_sar1wc_delayed.csv h_sar1wc_delayed.csv COPYONLY)
configure_file(data/h_sar1mc_delayed.csv h_sar1mc_delayed.csv COPYONLY)
configure_file(data/h_sar2mc_delayed.csv h_sar2mc_delayed.csv COPYONLY)
configure_file(data/h_sar3mc_delayed.csv h_sar3mc_delayed.csv COPYONLY)
configure_file(data/h_sar6mc_delayed.csv h_sar6mc_delayed.csv COPYONLY)
configure_file(data/h_sar9mc_delayed.csv h_sar9mc_delayed.csv COPYONLY)
configure_file(data/h_sar12mc_delayed.csv h_sar12mc_delayed.csv COPYONLY)

add_executable(${PROJECT_NAME}
  example.cpp # should we have *.cpp in src and *.h in include?
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  reset_example_util
  reset
  calendar_static-data
)

# to make IDE to show this file as part of the project
target_sources(${PROJECT_NAME} PRIVATE
  SARON_calendar_algorithms.h # should target_include_directories take care of a path here?
  SARON_average.h
  data/hsrron.csv
  data/h_sar1wc_delayed.csv
  data/h_sar1mc_delayed.csv
  data/h_sar2mc_delayed.csv
  data/h_sar3mc_delayed.csv
  data/h_sar6mc_delayed.csv
  data/h_sar9mc_delayed.csv
  data/h_sar12mc_delayed.csv
)
