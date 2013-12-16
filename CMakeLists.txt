cmake_minimum_required(VERSION 2.6.2)
 
project(smashbattle)
 
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

include_directories("Battle")

add_definitions(-std=c++11)

find_package(SDL)
find_package(SDL_mixer)
find_package(SDL_net)
find_package(ZLIB)
 
set(SRCS Battle/Airstrike.cpp Battle/AirstrikePowerUp.cpp Battle/AmmoPowerUp.cpp Battle/AudioController.cpp Battle/AudioOptions.cpp Battle/Bomb.cpp Battle/BombPowerUp.cpp Battle/CannonNPC.cpp Battle/CharacterSelect.cpp Battle/ChickNPC.cpp Battle/Client.cpp Battle/ClientNetworkMultiplayer.cpp Battle/CommandProcessor.cpp Battle/commands/CommandGeneratePowerup.cpp Battle/Commands.cpp Battle/ControlsOptions.cpp Battle/DoubleDamagePowerUp.cpp Battle/GameInput.cpp Battle/Gameplay.cpp Battle/GameplayObject.cpp Battle/GatlingNPC.cpp Battle/Graphics.cpp Battle/HealthPowerUp.cpp Battle/InstantKillBulletPowerUp.cpp Battle/LaserBeam.cpp Battle/LaserBeamPowerUp.cpp Battle/Level.cpp Battle/LevelSelect.cpp Battle/LocalMultiplayer.cpp Battle/LocalMultiplayerRoundEnd.cpp Battle/log.cpp Battle/Main.cpp Battle/Menu.cpp Battle/Mine.cpp Battle/MinePowerUp.cpp Battle/Mission.cpp Battle/MissionSelect.cpp Battle/NetworkMultiplayer.cpp Battle/NPC.cpp Battle/Options.cpp Battle/OptionsScreen.cpp Battle/PauseMenu.cpp Battle/PlayerAnimation.cpp Battle/Player.cpp Battle/Projectile.cpp Battle/RandomPowerUp.cpp Battle/ServerClient.cpp Battle/Server.cpp Battle/ShieldPowerUp.cpp Battle/states/ServerStateAcceptClients.cpp Battle/states/ServerState.cpp Battle/states/ServerStateGameStarted.cpp Battle/states/ServerStateInactive.cpp Battle/states/ServerStateInitialize.cpp Battle/Text.cpp Battle/Timer.cpp Battle/Util.cpp Battle/util/LevelUtil.cpp Battle/util/PlayerUtil.cpp Battle/util/ServerUtil.cpp)
 
add_executable(Battle/smashbattle WIN32 ${SRCS})
 
target_link_libraries(Battle/smashbattle ${wxWidgets_LIBRARIES} ${SDL_LIBRARY} ${SDL_MIXER_LIBRARY} ${SDL_NET_LIBRARY} ${ZLIB_LIBRARIES})
