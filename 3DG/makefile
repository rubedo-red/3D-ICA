CXX = g++                                                                                                             

CXXFLAGS = -Iinclude -IExternal -Ibuild -Wall -Wextra -MMD -MP $(shell pkg-config --cflags glfw3)
LIBS = $(shell pkg-config --libs glfw3) -lGL -lGLEW -lglm -lassimp -limgui

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:src/%.cpp=build/%.o)

build/3d : $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LIBS)

build/%.o : src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LIBS)

-include $(OBJ:.o=.d)

clean :
	rm -f build/*
