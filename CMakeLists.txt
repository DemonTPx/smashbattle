cmake_minimum_required(VERSION 2.6.2)
 
project(smashbattle)
 
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

include_directories("Battle")

add_definitions(-std=c++11)

find_package(SDL)
find_package(SDL_mixer)
find_package(SDL_net)
find_package(ZLIB)
 
# generate with: find ./Battle/ -name '*.cpp'|xargs echo
set(SRCS ./Battle/AudioOptions.cpp ./Battle/Gameplay.cpp ./Battle/commands/CommandGeneratePowerup.cpp ./Battle/Timer.cpp ./Battle/util/StringUtils.cpp ./Battle/util/ServerUtil.cpp ./Battle/util/LevelUtil.cpp ./Battle/util/Log.cpp ./Battle/util/PlayerUtil.cpp ./Battle/Airstrike.cpp ./Battle/LocalMultiplayer.cpp ./Battle/ControlsOptions.cpp ./Battle/NPC.cpp ./Battle/RandomPowerUp.cpp ./Battle/Bomb.cpp ./Battle/ShieldPowerUp.cpp ./Battle/AirstrikePowerUp.cpp ./Battle/AmmoPowerUp.cpp ./Battle/network/ClientNetworkMultiplayer.cpp ./Battle/network/Client.cpp ./Battle/network/CommandProcessor.cpp ./Battle/network/Server.cpp ./Battle/network/ServerClient.cpp ./Battle/network/Commands.cpp ./Battle/PlayerAnimation.cpp ./Battle/GatlingNPC.cpp ./Battle/Mine.cpp ./Battle/ChickNPC.cpp ./Battle/CharacterSelect.cpp ./Battle/LaserBeamPowerUp.cpp ./Battle/CannonNPC.cpp ./Battle/DoubleDamagePowerUp.cpp ./Battle/MinePowerUp.cpp ./Battle/GameplayObject.cpp ./Battle/Menu.cpp ./Battle/PauseMenu.cpp ./Battle/Main.cpp ./Battle/Options.cpp ./Battle/BombPowerUp.cpp ./Battle/AudioController.cpp ./Battle/LevelSelect.cpp ./Battle/Projectile.cpp ./Battle/Player.cpp ./Battle/NetworkMultiplayer.cpp ./Battle/LaserBeam.cpp ./Battle/Graphics.cpp ./Battle/Text.cpp ./Battle/InstantKillBulletPowerUp.cpp ./Battle/Mission.cpp ./Battle/GameInput.cpp ./Battle/states/ServerStateInitialize.cpp ./Battle/states/ServerStateGameStarted.cpp ./Battle/states/ServerStateInactive.cpp ./Battle/states/ServerStateAcceptClients.cpp ./Battle/states/ServerState.cpp ./Battle/HealthPowerUp.cpp ./Battle/LocalMultiplayerRoundEnd.cpp ./Battle/Level.cpp ./Battle/OptionsScreen.cpp ./Battle/MissionSelect.cpp) 

add_executable(Battle/smashbattle WIN32 ${SRCS})
 
target_link_libraries(Battle/smashbattle ${wxWidgets_LIBRARIES} ${SDL_LIBRARY} ${SDL_MIXER_LIBRARY} ${SDL_NET_LIBRARY} ${ZLIB_LIBRARIES})

install (TARGETS Battle/smashbattle DESTINATION /usr/share/games/smashbattle/)
install (DIRECTORY ${CMAKE_BINARY_DIR}/Battle/gfx/  DESTINATION /usr/share/games/smashbattle/gfx FILES_MATCHING PATTERN "*")
install (DIRECTORY ${CMAKE_BINARY_DIR}/Battle/sfx/  DESTINATION /usr/share/games/smashbattle/sfx FILES_MATCHING PATTERN "*")
install (DIRECTORY ${CMAKE_BINARY_DIR}/Battle/music/  DESTINATION /usr/share/games/smashbattle/music FILES_MATCHING PATTERN "*")
install (DIRECTORY ${CMAKE_BINARY_DIR}/Battle/stage/  DESTINATION /usr/share/games/smashbattle/stage FILES_MATCHING PATTERN "*")
install (TARGETS Battle/smashbattle DESTINATION /usr/local/bin/)
