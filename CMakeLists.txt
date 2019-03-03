cmake_minimum_required(VERSION 3.11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include_directories(./include/)

##add_executable(file_test test/test_file.cpp)
##target_link_libraries(file_test gfile)

add_executable(cleanup_test test/test_cleanup.cpp include/gupta/cleanup.hpp)

add_executable(podvector_test test/test_podvector.cpp)

add_executable(format_io_test test/test_format_io.cpp)
set_property(TARGET format_io_test PROPERTY CXX_STANDARD 17)

add_executable(arg_test test/test_arg.cpp)
set_property(TARGET arg_test PROPERTY CXX_STANDARD 17)

add_executable(buffer_stream_test test/test_buffer_stream.cpp)

add_executable(ini_test include/gupta/ini.hpp test/test_ini.cpp)

add_executable(dll_test include/gupta/dll.hpp test/test_dll.cpp)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(helloDll SHARED test/helloDll.cpp)
##set_property(TARGET helloDll PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON)
set_property(TARGET dll_test PROPERTY CXX_STANDARD 17)
