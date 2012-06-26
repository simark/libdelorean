# this is for testing librbrntrvll
#
# author:	Philippe Proulx <philippe.proulx@polymtl.ca>

# files
SRC = test.cpp
TARGET = test
LIB_PATH = ../../../src/.libs
INC_PATH = ../../../include/rbrntrvll/
LIB = -L$(LIB_PATH) -lrbrntrvll

# automatic
OBJ = $(SRC:.cpp=.o)

# commands
CPP = g++
CPPFLAGS = -I$(INC_PATH) -g -Wall
RM = rm -f

# phony targets
.PHONY: all clean run

# main target
all: $(TARGET)

# run target
run: $(TARGET)
	LD_LIBRARY_PATH=$(LIB_PATH) ./test

# target
$(TARGET): $(SRC)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIB)

# clean target
clean:
	$(RM) $(TARGET)

