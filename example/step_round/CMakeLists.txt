project("${PROJECT_NAME}_step-round" LANGUAGES CXX)

add_executable(${PROJECT_NAME}
  example.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  reset
)
