cmake_minimum_required(VERSION 3.10)

project(tiny-renderer VERSION 0.0.1)

# Define MY_SOURCES to be a list of all the source files for my game 
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# add_library(
#   tgaimage
#   "src/tgaimage.h"
#   "src/tgaimage.cpp"
# )

add_executable("${CMAKE_PROJECT_NAME}")

target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${MY_SOURCES} )

# Link the library to the executable
# target_link_libraries(tiny-renderer PRIVATE tgaimage)
