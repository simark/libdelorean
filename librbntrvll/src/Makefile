# simple Makefile in order to start with something...
#
# author:	Philippe Proulx <philippe.proulx@polymtl.ca>

# files
SRC = \
	AbstractHistoryTree.cpp \
	OutHistoryTree.cpp \
	InHistoryTree.cpp \
	HistoryTree.cpp \
	HistoryTreeNode.cpp \
	HistoryTreeCoreNode.cpp \
	HistoryTreeLeafNode.cpp \
	HistoryTreeConfig.cpp \
	Interval.cpp \
	StringInterval.cpp \
	StringIntervalFactory.cpp \
	UIntInterval.cpp \
	UIntIntervalFactory.cpp \
	NullInterval.cpp \
	NullIntervalFactory.cpp \
	IntInterval.cpp \
	IntIntervalFactory.cpp \
	FloatInterval.cpp \
	FloatIntervalFactory.cpp \
	IntervalCreator.cpp \
	ex/UnknownIntervalTypeEx.cpp \
	ex/UnknownNodeTypeEx.cpp \
	ex/ExistingIntervalTypeEx.cpp \
	ex/TimeRangeEx.cpp \
	ex/NodeFullEx.cpp \
	ex/IOEx.cpp
H_SRC = \
	AbstractHistoryTree.hpp \
	OutHistoryTree.hpp \
	InHistoryTree.hpp \
	HistoryTreeBackend.hpp \
	HistoryTree.hpp \
	HistoryTreeNode.hpp \
	HistoryTreeCoreNode.hpp \
	HistoryTreeLeafNode.hpp \
	HistoryTreeConfig.hpp \
	Interval.hpp \
	StringInterval.hpp \
	StringIntervalFactory.hpp \
	UIntInterval.hpp \
	UIntIntervalFactory.hpp \
	NullInterval.hpp \
	NullIntervalFactory.hpp \
	IntInterval.hpp \
	IntIntervalFactory.hpp \
	FloatInterval.hpp \
	FloatIntervalFactory.hpp \
	IPrintable.hpp \
	IIntervalFactory.hpp \
	IntervalCreator.hpp \
	ex/UnknownIntervalTypeEx.hpp \
	ex/UnknownNodeTypeEx.hpp \
	ex/ExistingIntervalTypeEx.hpp \
	ex/TimeRangeEx.hpp \
	ex/NodeFullEx.hpp \
	ex/IOEx.hpp \
	fixed_config.h \
	basic_types.h
	
TARGET_NAME = librbntrvll
TARGET_SONAME = $(TARGET_NAME).so.1
TARGET_SO = $(TARGET_NAME).so.1.0
TARGET_PLAIN = $(TARGET_NAME).so

# automatic
OBJ = $(SRC:.cpp=.o)

# commands
CPP = g++
CPPFLAGS = -g -Wall -Wno-unused-variable
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

