GPP_FLAGS = -lGL -lGLU -lglut -lboost_system -lboost_filesystem -lpthread -std=c++14

SRC_FILES = SimVisualization.cpp Tools.c Cell_HexaPoly.cpp Helper.cpp

default:
	@g++ $(SRC_FILES) -o Prog.exe $(GPP_FLAGS)

debug:
	@g++ $(SRC_FILES) -o Prog.exe $(GPP_FLAGS) -g


opt2:
	@g++ $(SRC_FILES) -o Prog.exe $(GPP_FLAGS) -O2

opt3:
	@g++ $(SRC_FILES) -o Prog.exe $(GPP_FLAGS) -O3


libs= $(shell pkg-config --static --libs glfw3 glew)