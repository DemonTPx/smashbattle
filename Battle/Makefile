CC=g++
CFLAGS=-c -Wall -Wno-reorder -Wno-switch -std=c++11 -I./
LDFLAGS=-lz -lstdc++ 
LDPOSTFLAGS= `sdl-config --libs` -lSDL -lSDL_mixer -lSDL_net
SRCS=LevelSelect.cpp\
	Player.cpp\
	ChickNPC.cpp\
	Commands.cpp\
	Options.cpp\
	HealthPowerUp.cpp\
	RandomPowerUp.cpp\
	Menu.cpp\
	GameInput.cpp\
	InstantKillBulletPowerUp.cpp\
	MinePowerUp.cpp\
	NPC.cpp\
	Airstrike.cpp\
	PauseMenu.cpp\
	OptionsScreen.cpp\
	DoubleDamagePowerUp.cpp\
	Projectile.cpp\
	log.cpp\
	Main.cpp\
	AmmoPowerUp.cpp\
	PlayerAnimation.cpp\
	Timer.cpp\
	LaserBeam.cpp\
	Mine.cpp\
	MissionSelect.cpp\
	NetworkMultiplayer.cpp\
	GatlingNPC.cpp\
	Gameplay.cpp\
	AirstrikePowerUp.cpp\
	CommandProcessor.cpp\
	AudioOptions.cpp\
	LocalMultiplayer.cpp\
	states/ServerStateInactive.cpp\
	states/ServerStateGameStarted.cpp\
	states/ServerStateAcceptClients.cpp\
	states/ServerState.cpp\
	states/ServerStateInitialize.cpp\
	Server.cpp\
	util/PlayerUtil.cpp\
	util/ServerUtil.cpp\
	util/LevelUtil.cpp\
	LocalMultiplayerRoundEnd.cpp\
	Level.cpp\
	ClientNetworkMultiplayer.cpp\
	AudioController.cpp\
	Client.cpp\
	Graphics.cpp\
	GameplayObject.cpp\
	Mission.cpp\
	Bomb.cpp\
	Text.cpp\
	CharacterSelect.cpp\
	ServerClient.cpp\
	ShieldPowerUp.cpp\
	CannonNPC.cpp\
	LaserBeamPowerUp.cpp\
	BombPowerUp.cpp\
	ControlsOptions.cpp\
	Util.cpp
OBJS=$(SRCS:.cpp=.o)
EXECUTABLE=battle

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o battle $(LDPOSTFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -rf *.o battle

install:
	mkdir -p /usr/share/games/smashbattle
	cp $(EXECUTABLE) /usr/share/games/smashbattle
	mkdir -p /usr/share/games/smashbattle/gfx
	mkdir -p /usr/share/games/smashbattle/sfx
	mkdir -p /usr/share/games/smashbattle/music
	mkdir -p /usr/share/games/smashbattle/stage
	cp -R gfx/* /usr/share/games/smashbattle/gfx/
	cp -R sfx/* /usr/share/games/smashbattle/sfx/
	cp -R music/* /usr/share/games/smashbattle/music/
	cp -R stage/* /usr/share/games/smashbattle/stage/
	cp smashbattle /usr/local/bin/
