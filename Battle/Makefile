CC=g++
CFLAGS=-c -Wall
LDFLAGS=-lSDL -lSDL_ttf -lSDL_mixer -lstdc++
SRCS=AudioController.cpp\
	Battle.cpp\
	Main.cpp\
	Menu.cpp\
	OptionsScreen.cpp\
	Options.cpp\
	Player.cpp\
	Projectile.cpp\
	Timer.cpp\
	CharacterSelect.cpp\
	AmmoPowerUp.cpp\
	HealthPowerUp.cpp\
	BombPowerUp.cpp\
	Bomb.cpp\
	DoubleDamagePowerUp.cpp\
	InstantKillBulletPowerUp.cpp
OBJS=AudioController.o\
	Battle.o\
	Main.o\
	Menu.o\
	OptionsScreen.o\
	Options.o\
	Player.o\
	Projectile.o\
	Timer.o\
	CharacterSelect.o\
	AmmoPowerUp.o\
	HealthPowerUp.o\
	BombPowerUp.o\
	Bomb.o\
	DoubleDamagePowerUp.o\
	InstantKillBulletPowerUp.o
EXECUTABLE=battle

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o battle

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -rf *.o battle
