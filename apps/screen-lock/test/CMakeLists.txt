include_directories("../")

add_executable(test-dmStateMachine test-dmStateMachine.cc ../dmStateMachine.cc)
target_link_libraries(test-dmStateMachine gtest)
