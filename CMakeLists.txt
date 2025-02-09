cmake_minimum_required(VERSION 3.14)
project(snake)

include_directories(${CMAKE_SOURCE_DIR}/include .)

file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} ${SOURCES})

set(PREFIX_DIR "${CMAKE_BINARY_DIR}/pdcurses-prefix")
include(ExternalProject)
ExternalProject_Add(
    pdcurses
    PREFIX "${PREFIX_DIR}"
    GIT_REPOSITORY https://github.com/wmcbrine/PDCurses.git
    CONFIGURE_COMMAND ""
    BUILD_COMMAND make -C "${PREFIX_DIR}/src/pdcurses/wincon"
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy "${PREFIX_DIR}/src/pdcurses/wincon/pdcurses.a" "${PREFIX_DIR}/src/pdcurses-build"
)
    
add_dependencies(${PROJECT_NAME} pdcurses)
    
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -s -O3)
target_link_libraries(${PROJECT_NAME} PRIVATE "${PREFIX_DIR}/src/pdcurses-build/pdcurses.a")
target_include_directories(${PROJECT_NAME} PRIVATE ${PREFIX_DIR}/src/pdcurses)