CC=g++
CFLAGS=-c -Wall
LDFLAGS=-lSDL -lSDL_mixer -lz -lstdc++
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
	Main.cpp\
	Menu.cpp\
	Mine.cpp\
	MinePowerUp.cpp\
	Mission.cpp\
	MissionSelect.cpp\
	NPC.cpp\
	Options.cpp\
	OptionsScreen.cpp\
	PauseMenu.cpp\
	Player.cpp\
	PlayerAnimation.cpp\
	Projectile.cpp\
	RandomPowerUp.cpp\
	ShieldPowerUp.cpp\
	Text.cpp\
	Timer.cpp
OBJS=$(SRCS:.cpp=.o)
EXECUTABLE=battle

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o battle

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
