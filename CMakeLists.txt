cmake_minimum_required(VERSION 3.12)
project(LDtkSFMLGame)

set(CMAKE_CXX_STANDARD 11)

find_package(LDtkLoader 1.2)

# set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML COMPONENTS graphics)

file(GLOB_RECURSE SRC src/*.cpp)

add_executable(LDtkSFMLGame ${SRC})
set_target_properties(
    LDtkSFMLGame PROPERTIES
    DEBUG_POSTFIX -d
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}
)
target_link_libraries(LDtkSFMLGame PRIVATE LDtkLoader sfml-graphics)
