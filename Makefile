CXX = -g -Wall #-std=c++11
SRCDIR = src
OBJDIR = obj
BINDIR = bin
INCDIR = include

SRCS = Socket.cpp ServerSocket.cpp ClientSocket.cpp MazeRunner.cpp
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)

INCFLAGS = -Iinclude


LDFLAGS = -lpthread
CXXFLAGS = $(INCFLAGS)

FILE=$(INCDIR)/RunnerVersion.txt
VERSION = `grep 'MINOR_VERSION' $(FILE)`

BIN_TRGTS = $(BINDIR)/MazeRunner_1.1_beta

all: $(BIN_TRGTS)
	@echo $(VERSION)


$(BIN_TRGTS): $(OBJS)
	@mkdir -p $(@D)
	g++ $(CXX) test/runMaze.cpp  $^ -o $@ $(CXXFLAGS) $(LDFLAGS)


$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	g++ $(CXX) -c -Iinclude $< -o $@ $(CXXFLAGS)

clean:
	rm -f $(OBJDIR)/*.o $(BIN_TRGTS)
