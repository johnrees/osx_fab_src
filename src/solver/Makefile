CFLAGS = -O3
#CFLAGS = -g

LDFLAGS = -lpng -lboost_thread -lboost_system

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
  # Look in the X11 folder for system default libpng
  LDFLAGS += -L/usr/X11/lib
  CFLAGS += -I/usr/X11/include
  
  # Check for MacPorts installation, if found look there for libraries
  OPT := $(shell test -e /opt/local/lib; echo $$?)
  ifeq ($(OPT), 0)
    LDFLAGS += -L/opt/local/lib
  endif
endif

CPP = converter.cpp parser.cpp solver.cpp \
      fabtools.cpp numeric_tree.cpp rect.cpp tree.cpp \
      logic_tree.cpp opcodes.cpp roots.cpp lattice.cpp \
      solver_tp.cpp
HPP = converter.hpp logic_tree.hpp parser.hpp switches.hpp \
      fab_interval.hpp numeric_nodes.hpp rect.hpp tree.hpp \
      fabtools.hpp numeric_tree.hpp  roots.hpp \
      logic_node.hpp opcodes.hpp solver.hpp lattice.hpp \
      solver_tp.hpp
OBJS = $(CPP:.cpp=.o)

all: math_png

math_png: main.cpp $(OBJS)
	g++ $(CFLAGS) $(OBJS) main.cpp -o math_png $(LDFLAGS)

%.o: %.cpp $(HPP)
	g++ $(CFLAGS) -c $<

clean:
	@echo "	Removing solver files"
	@rm -f $(OBJS) math_png
