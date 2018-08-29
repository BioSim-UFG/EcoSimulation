SHELL = /bin/sh
CC=gcc				#C compiler
CPPC=g++			#C++ compiler
CFLAGS=-lz -lm -std=c++11		#standard g++ flags
NVC=nvcc			#CUDA compiler
NVFLAGS=-use_fast_math -O2		#CUDA compiler flags
PSC=fpc				#pascal compiler
PFLAGS=-Fl/usr/local/cuda/lib64 -Mdelphi	#pascal compiler flags

ECHOMSG= ''


# src dir
SRCDIR = src#current dir
# Binary dir
BDIR = bin

# object files ( .o)
ODIR = $(BDIR)
_OBJ = decompressData.o PaleoClimate.o Interpolation.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

# headers
_DEPS = color.h PaleoClimate.h decompressData.h
DEPS = $(patsubst %,$(SRCDIR)/%,$(_DEPS))

#target .c files
$(BDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	@$(CPPC) $< -c -o $@ $(CFLAGS) $(EXTFLG)
	@echo Compiling $(subst $(SRCDIR)/,'',$<)
#target .cpp files
$(BDIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	@$(CPPC) $< -c -o $@ $(CFLAGS) $(EXTFLG)
	@echo Compiling $(subst $(SRCDIR)/,'',$<)
#target .cc files
$(BDIR)/%.o: $(SRCDIR)/%.cc $(DEPS)
	@$(CPPC) $< -c -o $@ $(CFLAGS) $(EXTFLG)
	@echo Compiling $(subst $(SRCDIR)/,'',$<)
#target .cu files
$(BDIR)/%.o: $(SRCDIR)/%.cu $(DEPS)
	@$(NVC) $< -c -o $@ $(NVFLAGS) $(EXTFLG)
	@echo Compiling $(subst $(SRCDIR)/,'',$<)


# normal compiling mode
default: ECHOMSG := 'Normal compiling mode\n'
default: $(OBJ)
	@$(CPPC) $^ -o Prog.exe $(CFLAGS)$(EXTFLG)
	@printf $(ECHOMSG)
	@echo 'All compiled'

#fast, optimized compiling mode
fast: opt3
opt3: EXTFLG = -O3 -march=native -Wno-unused-result
opt3: ECHOMSG = "fast, optimized compiling mode level 3\n"
opt3: default

opt2: EXTFLG = -O2 -march=native -Wno-unused-result
opt2: ECHOMSG = "fast, optimized compiling mode level 2\n"
opt2: default

opt1: EXTFLG = -O1 -march=native -Wno-unused-result
opt1: ECHOMSG = "fast, optimized compiling mode level 1\n"
opt1: default

debug: EXTFLG = -g -fno-inline-functions -DGLIBCXX_DEBUG
debug: ECHOMSG = "Debugging compiling mode \n"
debug: default

all:
	@touch $(SRCDIR)/*
	@echo 'touching all src files'

clear: 
	@find -name '*.o' -delete
	@echo 'deleted .o files'
