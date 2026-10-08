CXX = g++                                                                                                             

CXXFLAGS = -I./include -Wall -Wextra $(shell pkg-config --cflags glfw3)
CFLAGS = -I./include
LIBS = $(shell pkg-config --libs glfw3) -lGL -lGLEW -lglm -lassimp -limgui

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:src/%.cpp=build/%.o)

build/3d : $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LIBS)

build/%.o : src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LIBS)

clean :
	rm -f build/*
