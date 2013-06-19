UNAME := $(shell uname -s)
CC = g++
OBJ = miro.o pathfinder.o
LDFLAGS = -lGL -lGLU -lglut

# Mac OS
ifeq ($(UNAME), Darwin)
	LDFLAGS = -framework OpenGL -framework glut
endif

miro : $(OBJ)
	$(CC) -o miro $(OBJ) $(LDFLAGS)

miro.o : miro.cpp miro.h pathfinder.h
	$(CC) -c -g miro.cpp

pathfinder.o : pathfinder.cpp pathfinder.h miro.h
	$(CC) -c -g pathfinder.cpp

clean :
	rm $(OBJ)
