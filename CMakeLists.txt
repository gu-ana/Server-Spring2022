cmake_minimum_required(VERSION 3.10.0)
project(swan)

# Make sure developers do not run cmake in the main project directory, to keep
# build artifacts from becoming clutter
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()

# Turn on debug builds if we are building in a devel shell
if (CMAKE_BUILD_TYPE STREQUAL "" AND "$ENV{DEVEL_SHELL}" STREQUAL "1")
    message(STATUS "Setting debug build type by default in devel shell")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Output binaries to a sub directory "bin"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable cmake testing
include(CTest)
enable_testing()

# Enable GoogleTest
include(GoogleTest)
add_subdirectory(/usr/src/googletest googletest)

# Enable Boost
# Use static libraries so binaries can be deployed without a full boost install
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.50 REQUIRED COMPONENTS system filesystem log_setup log)
message(STATUS "Boost version: ${Boost_VERSION}")

include_directories(include)

# require pthreads 
find_package(Threads REQUIRED)

# add libraries
add_library(server_lib src/server.cc)
add_library(session_lib src/session.cc)
add_library(config_parser_lib src/config_parser.cc)
add_library(request_handler_delegate_lib src/request_handler_delegate.cc)
add_library(request_handler_lib src/request_handler/request_handler.cc)
add_library(echo_handler_lib src/request_handler/echo_handler.cc src/request_handler/request_handler.cc)
add_library(error_handler_lib src/request_handler/error_handler.cc src/request_handler/request_handler.cc)
add_library(static_handler_lib src/request_handler/static_handler.cc src/request_handler/request_handler.cc)
add_library(api_handler_lib src/request_handler/api_handler.cc src/request_handler/request_handler.cc)
add_library(sleep_handler_lib src/request_handler/sleep_handler.cc src/request_handler/request_handler.cc)
add_library(health_handler_lib src/request_handler/health_handler.cc src/request_handler/request_handler.cc)
add_library(bad_request_handler_lib src/request_handler/bad_request_handler.cc src/request_handler/request_handler.cc)
add_library(mime_types_lib src/mime_types.cc)
add_library(logger_lib src/logger.cc)
add_library(handler_factory_lib src/request_handler_factory/request_handler_factory.cc)
add_library(static_factory_lib src/request_handler_factory/static_handler_factory.cc src/request_handler/request_handler.cc src/request_handler/static_handler.cc src/mime_types.cc)
add_library(echo_factory_lib src/request_handler_factory/echo_handler_factory.cc src/request_handler/request_handler.cc src/request_handler/echo_handler.cc)
add_library(error_factory_lib src/request_handler_factory/error_handler_factory.cc src/request_handler/request_handler.cc src/request_handler/error_handler.cc)
add_library(api_factory_lib src/request_handler_factory/api_handler_factory.cc src/request_handler/request_handler.cc src/request_handler/api_handler.cc)
add_library(sleep_factory_lib src/request_handler_factory/sleep_handler_factory.cc src/request_handler/request_handler.cc src/request_handler/sleep_handler.cc)
add_library(health_factory_lib src/request_handler_factory/health_handler_factory.cc src/request_handler/request_handler.cc src/request_handler/health_handler.cc)
add_library(bad_request_factory_lib src/request_handler_factory/bad_request_handler_factory.cc src/request_handler/request_handler.cc src/request_handler/bad_request_handler.cc)
add_library(palette_factory_lib src/request_handler_factory/palette_handler_factory.cc src/request_handler/request_handler.cc src/request_handler/palette_handler.cc)
add_library(file_system_lib src/real_file_system.cc src/fake_file_system.cc)

# set grouped lists of libraries
set(test_libs gtest_main gmock_main) 
set(logger_libs logger_lib Boost::log_setup Boost::log)
set(handler_libs request_handler_delegate_lib 
    echo_handler_lib static_handler_lib api_handler_lib sleep_handler_lib bad_request_handler_lib
    error_handler_lib health_handler_lib mime_types_lib request_handler_lib file_system_lib)
set(handler_factory_libs static_factory_lib echo_factory_lib api_factory_lib sleep_factory_lib bad_request_factory_lib
    error_factory_lib health_factory_lib palette_factory_lib handler_factory_lib file_system_lib)

#Updated executable name, srcs, and deps
add_executable(server src/server_main.cc)
target_link_libraries(server server_lib session_lib config_parser_lib ${handler_libs} ${logger_libs} ${handler_factory_libs} Boost::filesystem Boost::system Threads::Threads)

add_executable(config_parser src/config_parser_main.cc)
target_link_libraries(config_parser config_parser_lib ${logger_libs})

# Update test executable name, srcs, and deps
add_executable(config_parser_test tests/config_parser_test.cc)
target_link_libraries(config_parser_test config_parser_lib ${logger_libs} ${test_libs} )

add_executable(session_test tests/session_test.cc)
target_link_libraries(session_test session_lib ${handler_libs} config_parser_lib  
    ${logger_libs} Boost::filesystem ${test_libs})

add_executable(request_handler_test tests/request_handler_test.cc)
target_link_libraries(request_handler_test ${handler_libs} config_parser_lib ${logger_libs} Boost::filesystem ${test_libs})

add_executable(request_handler_delegate_test tests/request_handler_delegate_test.cc)
target_link_libraries(request_handler_delegate_test ${handler_libs} ${logger_libs} 
${handler_factory_libs} ${test_libs} Boost::filesystem)

add_executable(server_test tests/server_test.cc)
target_link_libraries(server_test server_lib session_lib config_parser_lib ${handler_libs} ${logger_libs} ${handler_factory_libs} Boost::filesystem Boost::system Threads::Threads ${test_libs})

# Update with test binary
gtest_discover_tests(config_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(session_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(request_handler_delegate_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(server_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)


# Add integration tests
add_test(NAME IntegrationTests COMMAND integration_tests.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

#Update with target/test targets
include(cmake/CodeCoverageReportConfig.cmake)
generate_coverage_report(TARGETS server server_lib session_lib config_parser_lib ${handler_libs}
                         TESTS config_parser_test session_test server_test 
                         request_handler_test request_handler_delegate_test)
