CC=g++
CFLAGS=-c -Wall
LDFLAGS=-lSDL -lSDL_ttf -lSDL_mixer -lstdc++
SRCS=AudioController.cpp Battle.cpp Main.cpp Menu.cpp Player.cpp Projectile.cpp Timer.cpp CharacterSelect.cpp
OBJS=AudioController.o Battle.o Main.o Menu.o Player.o Projectile.o Timer.o CharacterSelect.o
EXECUTABLE=battle

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o battle

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -rf *.o battle
