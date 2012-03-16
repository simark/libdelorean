# this is for testing librbrntrvll
#
# author:	Philippe Proulx <philippe.proulx@polymtl.ca>

# files
SRC = test.cpp
TARGET = test
LIB_PATH = ../../librbntrvll/src
INC_PATH = $(LIB_PATH)
LIB = -L$(LIB_PATH) -lrbntrvll

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
	@LD_LIBRARY_PATH=$(LIB_PATH) ./test
	
# run target
rundb: $(TARGET)
	@LD_LIBRARY_PATH=$(LIB_PATH) gdb ./test

# target
$(TARGET): $(SRC)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIB)

# clean target
clean:
	$(RM) $(TARGET)

