cmake_minimum_required(VERSION 3.12)
project(LDtkSFMLGame)

include(FetchContent)
FetchContent_Declare(
    LDtkLoader
    GIT_REPOSITORY https://github.com/Madour/LDtkLoader
    GIT_TAG 1.5.3.1
)
FetchContent_MakeAvailable(LDtkLoader)

# set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML COMPONENTS graphics)

add_executable(LDtkSFMLGame src/main.cpp src/TileMap.cpp)
set_target_properties(LDtkSFMLGame PROPERTIES DEBUG_POSTFIX -d RUNTIME_OUTPUT_DIRECTORY bin)
target_link_libraries(LDtkSFMLGame PRIVATE LDtkLoader::LDtkLoader sfml-graphics)

add_custom_command(TARGET LDtkSFMLGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets/ $<TARGET_FILE_DIR:LDtkSFMLGame>/assets/
)
