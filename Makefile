CC=g++
LIBS=-lsfml-graphics
SRC=src/cpu.cpp src/memory.cpp src/screen.cpp src/input.cpp
LIBS=-lsfml-window -lsfml-graphics
.SUFFIXES: .c .o
all:chip8 dasm

dasm:disassembler.cpp
	$(CC) $(CXXFLAGS) $^ -o $@

chip8:$(SRC:.cpp=.o)
	$(CC) $(LIBS) $^ -o $@ $(LIBS)

$(SRC):$(SRC:.cpp=.h)
