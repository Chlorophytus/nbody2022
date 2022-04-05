cmake_minimum_required(VERSION 3.18)
project(nbody2022 VERSION 0.1.0.3)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(GRAPHICS GRAPHICS_API_OPENGL_43)
set(OpenGL_GL_PREFERENCE "GLVND")
add_subdirectory(vendor/raylib/)

# Configure the project header(s)
configure_file(include/${PROJECT_NAME}_cfg.in 
  ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}_cfg.hpp)

# Find libraries
find_package(Threads REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_executable(${PROJECT_NAME}
  src/${PROJECT_NAME}_bullet.cpp
  src/${PROJECT_NAME}_sim.cpp
  src/${PROJECT_NAME}_main.cpp
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED TRUE)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

# Include and make aware GLEW, Lua, SDL2 headers...
target_include_directories(${PROJECT_NAME} PRIVATE include)
# Time to link
target_link_libraries(${PROJECT_NAME} raylib glfw)