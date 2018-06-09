PCNAME	:= $(shell uname -n)
OSNAME	:= $(shell uname)
ifeq ($(PCNAME), nova)
	CC	:= g++-5.3.0
else ifeq ($(OSNAME), Darwin)
	CC	:= g++-8
else
	CC	:= g++
endif
CFLAGS		:= -std=c++14 -Wall -Wextra -Werror -pedantic-errors -fPIC -DNDEBUG -g
SRCDIR		:= .
OBJDIR		:= .
BINDIR		:= .

EXE_NAME	:= ex3
EXE_OBJS	:= main.o TournamentManager.o GameManager.o AlgorithmRegistration.o Piece.o BoardImpl.o
EXE_FLAGS	:= -pthread -rdynamic -ldl -lstdc++fs

LIB_NAME	:= RSPPlayer_203521984.so
LIB_OBJS	:= AutoPlayerAlgorithm.o AlgorithmRegistration.o Piece.o BoardImpl.o
LIB_FLAGS	:= -shared

SOURCES	= $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
EXE_TARGET	:= $(BINDIR)/$(EXE_NAME)
LIB_TARGET	:= $(BINDIR)/$(LIB_NAME)

.PHONY: clean zip

default : all

all: rps_tournament rps_lib

rps_tournament: $(EXE_TARGET)

$(EXE_TARGET): $(EXE_OBJS)
	$(CC) $(EXE_OBJS) -o $@ $(EXE_FLAGS)

rps_lib: $(LIB_TARGET)

$(LIB_TARGET): $(LIB_OBJS)
	$(CC) $(LIB_OBJS) -o $@ $(LIB_FLAGS)

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXE_TARGET) $(LIB_TARGET) $(OBJECTS)

zip:
	zip hw3_203521984_203774849 *.cpp *.h makefile *.txt