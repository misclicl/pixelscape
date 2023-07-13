cmake_minimum_required(VERSION 3.10)

project(tiny-renderer VERSION 0.0.1)

# Add a library
add_library(
  tgaimage
  "src/tgaimage.h"
  "src/tgaimage.cpp"
)

add_executable(tiny-renderer "src/main.cpp")

# Link the library
target_link_libraries(tiny-renderer PRIVATE tgaimage)
