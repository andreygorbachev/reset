project("${PROJECT_NAME}_SOFR" LANGUAGES CXX)

configure_file(data/SOFR.csv SOFR.csv COPYONLY)
configure_file("data/SOFR Compounded Index.csv" "SOFR Compounded Index.csv" COPYONLY) # maybe no spaces in the file names?

add_executable(${PROJECT_NAME}
  example.cpp
  parser.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  reset
  calendar_static-data
)

# to make IDE to show this file as part of the project
target_sources(${PROJECT_NAME} PRIVATE
  data/SOFR.csv
  "data/SOFR Compounded Index.csv"
)
