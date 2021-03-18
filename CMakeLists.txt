cmake_minimum_required(VERSION 3.11...3.19)
project(aid
        VERSION 0.0.1
        LANGUAGES CXX
        )

set(CMAKE_CXX_STANDARD 17)

add_library(aid INTERFACE include/aid/aid.hpp include/aid/core/result.hpp include/aid/core/core.hpp include/aid/mpsc/sender.hpp include/aid/mpsc/mpsc_error.hpp include/aid/mpsc/channel_type.hpp include/aid/mpsc/mpsc.hpp include/aid/mpsc/receiver.hpp include/aid/mpsc/channel.hpp include/aid/mpsc/mpsc_queue.hpp)
target_include_directories(aid
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        )

add_subdirectory(tests)
include(cmake/doxygen.cmake)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${PROJECT_BINARY_DIR}/aidConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
        )

install(TARGETS aid
        EXPORT aidTargets
        LIBRARY DESTINATION lib COMPONENT Runtime
        ARCHIVE DESTINATION lib COMPONENT Development
        RUNTIME DESTINATION bin COMPONENT Runtime
        PUBLIC_HEADER DESTINATION include COMPONENT Development
        BUNDLE DESTINATION bin COMPONENT Runtime
        )

include(CMakePackageConfigHelpers)
configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/aidConfig.cmake.in"
        "${PROJECT_BINARY_DIR}/aidConfig.cmake"
        INSTALL_DESTINATION lib/cmake/aid
)

install(EXPORT aidTargets DESTINATION lib/cmake/aid)
install(FILES "${PROJECT_BINARY_DIR}/aidConfigVersion.cmake"
        "${PROJECT_BINARY_DIR}/aidConfig.cmake"
        DESTINATION lib/cmake/aid)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)
