cmake_minimum_required(VERSION 3.20)
project(netio LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    main.cpp
    netio/*.cpp
    netio/*.c
)

add_executable(netio ${SOURCES})
set_target_properties(netio PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/out"
)

target_include_directories(netio PRIVATE ${LIB_DIR}/include)

target_link_libraries(netio PRIVATE
    ws2_32
)
