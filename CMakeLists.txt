cmake_minimum_required(VERSION 2.6.2)
 
project(smashbattle)
 
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

include_directories("./Battle/")

add_definitions(-std=c++11)

find_package(SDL)
find_package(SDL_mixer)
find_package(SDL_net)
find_package(ZLIB)
 
# generate with: find ./Battle/ -name '*.cpp'|xargs echo

file(GLOB_RECURSE Foo_SOURCES "Battle/*.cpp")
file(GLOB_RECURSE Foo_HEADERS "Battle/*.h")

set (Foo_INCLUDE_DIRS "")
foreach (_headerFile ${Foo_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND Foo_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES Foo_INCLUDE_DIRS)

include_directories(${Foo_INCLUDE_DIRS})

add_executable (smashbattle ${Foo_SOURCES})
 
target_link_libraries(smashbattle ${wxWidgets_LIBRARIES} ${SDL_LIBRARY} ${SDL_MIXER_LIBRARY} ${SDL_NET_LIBRARY} ${ZLIB_LIBRARIES})

install (TARGETS smashbattle DESTINATION /usr/share/games/smashbattle/)
install (DIRECTORY ${CMAKE_BINARY_DIR}/Battle/gfx/  DESTINATION /usr/share/games/smashbattle/gfx FILES_MATCHING PATTERN "*")
install (DIRECTORY ${CMAKE_BINARY_DIR}/Battle/sfx/  DESTINATION /usr/share/games/smashbattle/sfx FILES_MATCHING PATTERN "*")
install (DIRECTORY ${CMAKE_BINARY_DIR}/Battle/music/  DESTINATION /usr/share/games/smashbattle/music FILES_MATCHING PATTERN "*")
install (DIRECTORY ${CMAKE_BINARY_DIR}/Battle/stage/  DESTINATION /usr/share/games/smashbattle/stage FILES_MATCHING PATTERN "*")
install (TARGETS smashbattle DESTINATION /usr/local/bin/)
