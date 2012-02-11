# simple Makefile in order to start with something...
#
# author:	Philippe Proulx <philippe.proulx@polymtl.ca>

# files
SRC = \
	HistoryTreeBackend.cpp \
	HistoryTree.cpp \
	HistoryTreeNode.cpp \
	Interval.cpp \
	StringInterval.cpp \
	StringIntervalFactory.cpp \
	UIntInterval.cpp \
	UIntIntervalFactory.cpp \
	IntInterval.cpp \
	IntIntervalFactory.cpp \
	FloatInterval.cpp \
	FloatIntervalFactory.cpp \
	IntervalCreator.cpp \
	ex/UnknownIntervalTypeEx.cpp \
	ex/ExistingIntervalTypeEx.cpp
H_SRC = \
	HistoryTreeBackend.hpp \
	HistoryTree.hpp \
	HistoryTreeNode.hpp \
	Interval.hpp \
	StringInterval.hpp \
	StringIntervalFactory.hpp \
	UIntInterval.hpp \
	UIntIntervalFactory.hpp \
	IntInterval.hpp \
	IntIntervalFactory.hpp \
	FloatInterval.hpp \
	FloatIntervalFactory.hpp \
	IPrintable.hpp \
	IIntervalFactory.hpp \
	IntervalCreator.hpp \
	ex/UnknownIntervalTypeEx.hpp \
	ex/ExistingIntervalTypeEx.hpp
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

