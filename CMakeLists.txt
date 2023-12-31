cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)

# Ask CMake to generate the compilation database
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_BUILD_TYPE Debug)

# Release flags
# set(CMAKE_CXX_FLAGS "-Wall -Wextra")
# set(CMAKE_CXX_FLAGS_DEBUG "-g")
# set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(pixelscape VERSION 0.0.1)

# Define MY_SOURCES to be a list of all the source files for my game 
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/examples/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp"
)

set(BUILD_STATIC_LIBS ON  CACHE INTERNAL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/raylib)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/doctest)

add_executable("${CMAKE_PROJECT_NAME}")
target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${MY_SOURCES})
# Link the libraries to the executable
target_link_libraries(${CMAKE_PROJECT_NAME} raylib doctest::doctest)

if (APPLE)
    target_link_libraries(${CMAKE_PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${CMAKE_PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${CMAKE_PROJECT_NAME} "-framework OpenGL")
endif()
