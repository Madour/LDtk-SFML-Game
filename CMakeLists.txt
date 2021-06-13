cmake_minimum_required(VERSION 3.12)
project(LDtkSFMLGame)

set(CMAKE_CXX_STANDARD 11)

find_package(LDtkLoader)
find_package(SFML COMPONENTS graphics)

file(GLOB_RECURSE SRC src/*.cpp)

add_executable(LDtkSFMLGame ${SRC})
set_target_properties(
    LDtkSFMLGame PROPERTIES
    DEBUG_POSTFIX -d
    RUNTIME_OUTPUT_DIRECTORY ../
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ../
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ../
)
target_link_libraries(LDtkSFMLGame PRIVATE LDtkLoader sfml-graphics)
