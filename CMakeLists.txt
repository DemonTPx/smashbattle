#
#
#
# With debugging symbols:
#
# cmake -DCMAKE_BUILD_TYPE=Debug . 
#
#

cmake_minimum_required(VERSION 2.6.2)
 
project(smashbattle)
 
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

include_directories("./Battle/")

add_definitions(-std=c++11)

find_package(SDL)
find_package(SDL_mixer)
find_package(SDL_net)
find_package(ZLIB)
find_package(CURL)
 
# generate with: find ./Battle/ -name '*.cpp'|xargs echo

file(GLOB_RECURSE Battle_SOURCES "Battle/*.cpp")
file(GLOB_RECURSE Battle_HEADERS "Battle/*.h")

set (Battle_INCLUDE_DIRS "")
foreach (_headerFile ${Battle_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND Battle_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES Battle_INCLUDE_DIRS)

include_directories(${Battle_INCLUDE_DIRS})

add_executable (smashbattle ${Battle_SOURCES})

file(GLOB_RECURSE tests_SOURCES "tests/*.cpp")
file(GLOB_RECURSE tests_HEADERS "tests/*.h")
file(GLOB_RECURSE tests_HEADERS "tests/*.hpp")

set (Tests_INCLUDE_DIRS "")
foreach (_headerFile ${Tests_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND Tests_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES Tests_INCLUDE_DIRS)

include_directories(${Tests_INCLUDE_DIRS})

add_executable (smashbattle_tests ${tests_SOURCES} 
	#"Battle/states/ServerStateGameStarted.cpp"
	#"Battle/network/Server.cpp"
)
 
target_link_libraries(smashbattle ${wxWidgets_LIBRARIES} ${SDL_LIBRARY} ${SDL_MIXER_LIBRARY} ${SDL_NET_LIBRARY} ${ZLIB_LIBRARIES} ${CURL_LIBRARIES})

install (TARGETS smashbattle DESTINATION /usr/share/games/smashbattle/)
install (DIRECTORY ${CMAKE_BINARY_DIR}/Battle/gfx/  DESTINATION /usr/share/games/smashbattle/gfx FILES_MATCHING PATTERN "*")
install (DIRECTORY ${CMAKE_BINARY_DIR}/Battle/sfx/  DESTINATION /usr/share/games/smashbattle/sfx FILES_MATCHING PATTERN "*")
install (DIRECTORY ${CMAKE_BINARY_DIR}/Battle/music/  DESTINATION /usr/share/games/smashbattle/music FILES_MATCHING PATTERN "*")
install (DIRECTORY ${CMAKE_BINARY_DIR}/Battle/stage/  DESTINATION /usr/share/games/smashbattle/stage FILES_MATCHING PATTERN "*")
install (TARGETS smashbattle DESTINATION /usr/local/bin/)
