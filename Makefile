MODE = OPT
CFLAGS = -g -O2 -Wall

ifeq "$(MODE)" "DEBUG"
	CFLAGS = -g
endif

################################################################################
#Edit these values to change the name of the executible and the source files
EXE = test 
SRC = main.cpp 
#INCLUDES = -I../../include -I./stl_bv
#LIBS = -L../../lib -lverif.linux -lutil.linux
#LIBS =
################################################################################

CXX = g++
CXXFLAGS = $(CFLAGS) $(INCLUDES)

OBJS = $(addsuffix .o, $(basename $(SRC)))

all : $(EXE)

$(EXE) : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

clean :
	rm -f $(OBJS) $(EXE)

