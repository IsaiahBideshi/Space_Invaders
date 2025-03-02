cmake_minimum_required(VERSION 3.30)
project(Space_Invaders)

set(CMAKE_CXX_STANDARD 20)
# Path to SFML
set(SFML_DIR "C:/Users/isaia/CLionProjects/Space_Invaders/SFML-2.6.2/lib/cmake/SFML")

# Find SFML
find_package(SFML 2.6.2 COMPONENTS system window graphics network audio REQUIRED)

add_executable(Space_Invaders main.cpp)

# Link SFML libraries to the executable
target_link_libraries(Space_Invaders sfml-system sfml-window sfml-graphics sfml-audio sfml-network)