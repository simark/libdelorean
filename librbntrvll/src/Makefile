# simple Makefile in order to start with something...
#
# author:	Philippe Proulx <philippe.proulx@polymtl.ca>

# files
SRC = HistoryTreeBackend.cpp HistoryTree.cpp HistoryTreeNode.cpp Interval.cpp \
	UIntInterval.cpp IntInterval.cpp IntIntervalFactory.cpp IntervalCreator.cpp \
	ex/NonexistentFactoryEx.cpp ex/FactoryOverwritingEx.cpp
H_SRC = HistoryTreeBackend.hpp HistoryTree.hpp HistoryTreeNode.hpp Interval.hpp \
	UIntInterval.hpp IntInterval.hpp IPrintable.hpp IIntervalFactory.hpp IntIntervalFactory.hpp \
	IntervalCreator.hpp ex/NonexistentFactoryEx.hpp ex/FactoryOverwritingEx.hpp
TARGET_NAME = librbntrvll
TARGET_SONAME = $(TARGET_NAME).so.1
TARGET_SO = $(TARGET_NAME).so.1.0
TARGET_PLAIN = $(TARGET_NAME).so

# automatic
OBJ = $(SRC:.cpp=.o)

# commands
CPP = g++
CPPFLAGS = -g -Wall
RM = rm -f

# phony targets
.PHONY: all clean

# main target
all: $(TARGET_SO)

# shared library
$(TARGET_SO): $(OBJ)
	$(CPP) -shared -Wl,-soname,$(TARGET_SONAME) -o $@ $^
	ln -f -s $(TARGET_SO) $(TARGET_SONAME)
	ln -f -s $(TARGET_SO) $(TARGET_PLAIN)

# objects
%.o: %.cpp $(H_SRC)
	$(CPP) $(CPPFLAGS) -c -fPIC -o $@ $<

# clean target
clean:
	$(RM) $(OBJ) $(TARGET_SO) $(TARGET_SONAME) $(TARGET_PLAIN)

