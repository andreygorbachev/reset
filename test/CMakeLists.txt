project("${PROJECT_NAME}_test" LANGUAGES CXX)

#configure_file(data/EuroSTR.csv EuroSTR.csv COPYONLY)
#configure_file(data/EuroSTR_compounded_index.csv EuroSTR_compounded_index.csv COPYONLY)
#configure_file(data/h_sar1mc_delayed.csv h_sar1mc_delayed.csv COPYONLY)
#configure_file(data/h_sar1wc_delayed.csv h_sar1wc_delayed.csv COPYONLY)
#configure_file(data/h_sar2mc_delayed.csv h_sar2mc_delayed.csv COPYONLY)
#configure_file(data/h_sar3mc_delayed.csv h_sar3mc_delayed.csv COPYONLY)
#configure_file(data/h_sar6mc_delayed.csv h_sar6mc_delayed.csv COPYONLY)
#configure_file(data/h_sar9mc_delayed.csv h_sar9mc_delayed.csv COPYONLY)
#configure_file(data/h_sar12mc_delayed.csv h_sar12mc_delayed.csv COPYONLY)
#configure_file(data/SARON.csv SARON.csv COPYONLY)
#configure_file(data/SONIA.csv SONIA.csv COPYONLY)
#configure_file(data/SONIA_compounded_index.csv SONIA_compounded_index.csv COPYONLY)

add_executable(${PROJECT_NAME}
  resets_math.cpp
  resets.cpp
  index.cpp
  setup.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  reset
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
