GPP_FLAGS = -lGL -lGLU -lglut -lboost_system -lboost_filesystem -lpthread -std=c++14

default:
	@g++ SimVisualization.cpp Tools.c Cell_HexaPoly.cpp -o Prog.exe $(GPP_FLAGS)

debug:
	@g++ SimVisualization.cpp Tools.c Cell_HexaPoly.cpp -o Prog.exe $(GPP_FLAGS) -g


opt2:
	@g++ SimVisualization.cpp Tools.c Cell_HexaPoly.cpp -o Prog.exe $(GPP_FLAGS) -O2

opt3:
	@g++ SimVisualization.cpp Tools.c Cell_HexaPoly.cpp -o Prog.exe $(GPP_FLAGS) -O3


libs= $(shell pkg-config --static --libs glfw3 glew)