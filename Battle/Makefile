CC=g++
CFLAGS=-c -Wall
LDFLAGS=-lSDL -lSDL_ttf -lSDL_mixer -lstdc++
SRCS=AmmoPowerUp.cpp\
	AudioController.cpp\
	AudioOptions.cpp\
	Battle.cpp\
	Bomb.cpp\
	BombPowerUp.cpp\
	CharacterSelect.cpp\
	ControlsOptions.cpp\
	DoubleDamagePowerUp.cpp\
	Gameplay.cpp\
	GameplayObject.cpp\
	Graphics.cpp\
	HealthPowerUp.cpp\
	InstantKillBulletPowerUp.cpp\
	Level.cpp\
	LevelSelect.cpp\
	LocalMultiplayer.cpp\
	Main.cpp\
	Menu.cpp\
	Options.cpp\
	OptionsScreen.cpp\
	PauseMenu.cpp\
	Player.cpp\
	Projectile.cpp\
	Timer.cpp
OBJS=AmmoPowerUp.o\
	AudioController.o\
	AudioOptions.o\
	Battle.o\
	Bomb.o\
	BombPowerUp.o\
	CharacterSelect.o\
	ControlsOptions.o\
	DoubleDamagePowerUp.o\
	Gameplay.o\
	GameplayObject.o\
	Graphics.o\
	HealthPowerUp.o\
	InstantKillBulletPowerUp.o\
	Level.o\
	LevelSelect.o\
	LocalMultiplayer.o\
	Main.o\
	Menu.o\
	Options.o\
	OptionsScreen.o\
	PauseMenu.o\
	Player.o\
	Projectile.o\
	Timer.o
EXECUTABLE=battle

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o battle

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -rf *.o battle
