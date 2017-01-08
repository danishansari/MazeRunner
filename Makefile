CXX = -g -Wall
SRCDIR = src
OBJDIR = obj
BINDIR = bin

SRCS = Socket.cpp ServerSocket.cpp ClientSocket.cpp MazeRunner.cpp
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)

INCFLAGS = -Iinclude

BIN_TRGTS = $(BINDIR)/MazeRunner_1.0

LDFLAGS = -lpthread
CXXFLAGS = $(INCFLAGS)

all: $(BIN_TRGTS)


$(BINDIR)/MazeRunner_1.0: $(OBJS)
	@mkdir -p $(@D)
	g++ $(CXX) test/runMaze.cpp  $^ -o $@ $(CXXFLAGS) $(LDFLAGS)


$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	g++ $(CXX) -c -Iinclude $< -o $@ $(CXXFLAGS)

clean:
	rm -f $(OBJDIR)/*.o $(BIN_TRGTS)
