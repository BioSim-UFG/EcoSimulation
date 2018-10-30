SHELL = /bin/sh
CC=gcc				#C compiler
CPPC=g++			#C++ compiler
CFLAGS=-lz -lm -std=c++11 #-lcuda -lcudart  -L/usr/local/cuda/lib64 -I/usr/local/cuda/include	#standard g++ flags
NVC=nvcc			#CUDA compiler
NVFLAGS=-use_fast_math --std=c++11	#CUDA compiler flags
PSC=fpc				#pascal compiler
PFLAGS=-Fl/usr/local/cuda/lib64 -Mdelphi	#pascal compiler flags

ECHOMSG := "Normal compiling mode\n"

EXTFLG= 
OS:=$(shell uname)


# src dir
SRCDIR = src
# Binary dir
BDIR = bin

# object files ( .o)
ODIR = $(BDIR)
_OBJ = Specie.o Cell.o Grid.o Simulation.o main.o decompressData.o Helper.o #libCuda.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

# headers
_DEPS = SimTypes.h Specie.h Cell.h Grid.h Simulation.h decompressData.h color.h cudaErrorCheck.h Helper.h
DEPS = $(patsubst %,$(SRCDIR)/%,$(_DEPS))



#  .---------- COLORS FOR ECHO---------. #
##########################################
ifeq ($(OS),Linux)
	#  Regular
	BLACK = \033[0;30m#Black
	RED = \033[0;31m#Red
	GREEN = \033[0;32m#Green
	YELLOW = \033[0;33m#Yellow
	BLUE = \033[0;34m#Blue
	PURPLE = \033[0;35m#Purple
	CYAN = \033[0;36m#Cyan
	WHITE = \033[0;37m#White
	NC = \033[0m # o Color -> reset the color

	#  Bold
	B_BLACK = \033[1;30m
	B_RED = \033[1;31m
	B_GREEN = \033[1;32m
	B_YELLOW = \033[1;33m
	B_BLUE = \033[1;34m
	B_PURPLE = \033[1;35m
	B_CYAN = \033[1;36m
	B_WHITE = \033[1;37m


	#Light / high intesity
	L_BLACK = \033[0;90m
	L_RED = \033[0;91m
	L_GREEN = \033[0;92m
	L_YELLOW = \033[0;93m
	L_BLUE = \033[0;94m
	L_PURPLE = \033[0;95m
	L_CYAN = \033[0;96m
	L_WHITE = \033[0;97m

	# Bold light/hight intesity
	BL_BLACK = \033[1;90m
	BL_RED = \033[1;91m
	BL_GREEN = \033[1;92m
	BL_YELLOW = \033[1;93m
	BL_BLUE = \033[1;94m
	BL_PURPLE = \033[1;95m
	BL_CYAN = \033[1;96m
	BL_WHITE = \033[1;97m
endif





#target .c files
$(BDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	@$(CPPC) $< -c -o $@ $(CFLAGS) $(EXTFLG)
	@echo "${GREEN}Compiling ${RED}$(subst $(SRCDIR)/,,$<) ${NC}"
#target .cpp files
$(BDIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	@$(CPPC) $< -c -o $@ $(CFLAGS) $(EXTFLG)
	@echo "${GREEN}Compiling ${YELLOW}$(subst $(SRCDIR)/,,$<) ${NC}"
#target .cc files
$(BDIR)/%.o: $(SRCDIR)/%.cc $(DEPS)
	@$(CPPC) $< -c -o $@ $(CFLAGS) $(EXTFLG)
	@echo "${GREEN}Compiling ${YELLOW}$(subst $(SRCDIR)/,,$<) ${NC}"
#target .cu files
$(BDIR)/%.o: $(SRCDIR)/%.cu $(DEPS)
	@$(NVC) $< -c -o $@ $(NVFLAGS) $(EXTFLG)
	@echo "${GREEN}Compiling ${PURPLE}$(subst $(SRCDIR)/,,$<) ${NC}"


# normal compiling mode
default: $(OBJ)
	@echo "$(BL_YELLOW)Linking files$(NC)"
	@$(CPPC) $^ -o Prog.exe $(CFLAGS)$(EXTFLG)
	@echo $(ECHOMSG)
#	@printf $(ECHOMSG)
	@echo "${BL_GREEN}Done${NC}"

#fast, optimized compiling mode
fast: opt2
opt3: CFLAGS+= -O3 -march=native -Wno-unused-result
opt3: NVFLAGS+= -O3
opt3: ECHOMSG = "fast, optimized compiling mode level 3\n"
opt3: default

opt2: CFLAGS+= -O2 -march=native -Wno-unused-result
opt2: NVFLAGS+= -O2
opt2: ECHOMSG = "fast, optimized compiling mode level 2\n"
opt2: default

opt1: CFLAGS+= -O1 -march=native -Wno-unused-result
opt1: NVFLAGS+= -O1
opt1: ECHOMSG = "fast, optimized compiling mode level 1\n"
opt1: default

debug: CFLAGS+= -ggdb3 -O0 -fno-inline-functions -DGLIBCXX_DEBUG -rdynamic
debug: NVFLAGS+= -O3 -g -G
debug: ECHOMSG = "Debugging compiling mode \n"
debug: default

profile: EXTFLG+= -pg -g
profile: ECHOMSG+= "profiling default compiling mode \n"
profile: default

profile_fast: EXTFLG+= -pg -g
profile_fast:
	@echo 'profile mode'
profile_fast: opt3

#profile%: ECHOMSG+= "profiling $@ compiling mode \n"

all:
	@touch $(SRCDIR)/*
	@echo 'touching all src files'

clean:
ifeq ($(OS),Windows)
	@del "$(ODIR)\*.o"
else
	@find -name '*.o' -delete
endif
	@echo 'deleted .o files'