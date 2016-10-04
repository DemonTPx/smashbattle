cmake_minimum_required(VERSION 2.6.2)
project(smashbattle)

find_package(SDL2 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(CURL REQUIRED)

find_file(SDL2_INCLUDE_DIR NAME SDL_mixer.h HINTS SDL2_mixer)
find_library(SDL2_mixer NAME SDL2_mixer)
find_file(SDL2_INCLUDE_DIR NAME SDL_net.h HINTS SDL2_net)
find_library(SDL2_net NAME SDL2_net)

if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
    message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
    message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

include_directories("Battle/")

add_definitions(
        -std=c++0x
        -fpermissive
)

file(GLOB_RECURSE Battle_SOURCES "Battle/*.cpp")
file(GLOB_RECURSE Battle_HEADERS "Battle/*.h")
file(GLOB_RECURSE Battle_HEADERS "Battle/*.hpp")

add_executable (smashbattle ${Battle_SOURCES})

target_link_libraries(smashbattle ${SDL2_LIBRARIES} ${ZLIB_LIBRARIES} ${CURL_LIBRARIES} -lpthread -lSDL2_mixer -lSDL2_net)

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

install (TARGETS smashbattle DESTINATION /usr/share/games/smashbattle/)
install (DIRECTORY ${CMAKE_SOURCE_DIR}/Battle/gfx/ DESTINATION /usr/share/games/smashbattle/gfx FILES_MATCHING PATTERN "*")
install (DIRECTORY ${CMAKE_SOURCE_DIR}/Battle/sfx/ DESTINATION /usr/share/games/smashbattle/sfx FILES_MATCHING PATTERN "*")
install (DIRECTORY ${CMAKE_SOURCE_DIR}/Battle/music/ DESTINATION /usr/share/games/smashbattle/music FILES_MATCHING PATTERN "*")
install (DIRECTORY ${CMAKE_SOURCE_DIR}/Battle/stage/ DESTINATION /usr/share/games/smashbattle/stage FILES_MATCHING PATTERN "*")
install (TARGETS smashbattle DESTINATION /usr/local/bin/)
