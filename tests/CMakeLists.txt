include(../cmake/catch.cmake)


add_executable(aid_test src/main_test.cpp src/result.cpp)
target_link_libraries(aid_test PRIVATE Catch2::Catch2 aid)

catch_discover_tests(aid_test)
