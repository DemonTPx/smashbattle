CC=g++
CFLAGS=-c -Wall -std=c++11 -I./
LDFLAGS=-lz -lstdc++ 
LDPOSTFLAGS= `sdl-config --libs` -lSDL -lSDL_mixer -lSDL_net
SRCS=Airstrike.cpp\
	AirstrikePowerUp.cpp\
	AmmoPowerUp.cpp\
	AudioController.cpp\
	AudioOptions.cpp\
	Bomb.cpp\
	BombPowerUp.cpp\
	CannonNPC.cpp\
	CharacterSelect.cpp\
	ChickNPC.cpp\
	Client.cpp\
	ClientNetworkMultiplayer.cpp\
	CommandProcessor.cpp\
	Commands.cpp\
	ControlsOptions.cpp\
	DoubleDamagePowerUp.cpp\
	GameInput.cpp\
	Gameplay.cpp\
	GameplayObject.cpp\
	GatlingNPC.cpp\
	Graphics.cpp\
	HealthPowerUp.cpp\
	InstantKillBulletPowerUp.cpp\
	LaserBeam.cpp\
	LaserBeamPowerUp.cpp\
	Level.cpp\
	LevelSelect.cpp\
	LocalMultiplayer.cpp\
	LocalMultiplayerRoundEnd.cpp\
	log.cpp\
	Main.cpp\
	Menu.cpp\
	Mine.cpp\
	MinePowerUp.cpp\
	Mission.cpp\
	MissionSelect.cpp\
	NetworkMultiplayer.cpp\
	NPC.cpp\
	Options.cpp\
	OptionsScreen.cpp\
	PauseMenu.cpp\
	PlayerAnimation.cpp\
	Player.cpp\
	Projectile.cpp\
	RandomPowerUp.cpp\
	ServerClient.cpp\
	Server.cpp\
	ShieldPowerUp.cpp\
	states/ServerState.cpp\
	states/ServerStateInactive.cpp\
	states/ServerStateInitialize.cpp\
	Text.cpp\
	Timer.cpp\
	Util.cpp\
	util/LevelUtil.cpp\
	util/PlayerUtil.cpp\
	util/ServerUtil.cpp
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
