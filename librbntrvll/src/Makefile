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
	intervals/Interval.cpp \
	intervals/StringInterval.cpp \
	intervals/StringIntervalFactory.cpp \
	intervals/UIntInterval.cpp \
	intervals/UIntIntervalFactory.cpp \
	intervals/NullInterval.cpp \
	intervals/NullIntervalFactory.cpp \
	intervals/IntInterval.cpp \
	intervals/IntIntervalFactory.cpp \
	intervals/FloatInterval.cpp \
	intervals/FloatIntervalFactory.cpp \
	IntervalCreator.cpp \
	ex/UnknownIntervalTypeEx.cpp \
	ex/UnknownNodeTypeEx.cpp \
	ex/ExistingIntervalTypeEx.cpp \
	ex/TimeRangeEx.cpp \
	ex/NodeFullEx.cpp \
	ex/IOEx.cpp \
	ex/InvalidFormatEx.cpp
H_SRC = \
	AbstractHistoryTree.hpp \
	OutHistoryTree.hpp \
	InHistoryTree.hpp \
	HistoryTree.hpp \
	HistoryTreeNode.hpp \
	HistoryTreeCoreNode.hpp \
	HistoryTreeLeafNode.hpp \
	HistoryTreeConfig.hpp \
	intervals/Interval.hpp \
	intervals/StringInterval.hpp \
	intervals/StringIntervalFactory.hpp \
	intervals/UIntInterval.hpp \
	intervals/UIntIntervalFactory.hpp \
	intervals/NullInterval.hpp \
	intervals/NullIntervalFactory.hpp \
	intervals/IntInterval.hpp \
	intervals/IntIntervalFactory.hpp \
	intervals/FloatInterval.hpp \
	intervals/FloatIntervalFactory.hpp \
	IPrintable.hpp \
	intervals/IIntervalFactory.hpp \
	IntervalCreator.hpp \
	ex/UnknownIntervalTypeEx.hpp \
	ex/UnknownNodeTypeEx.hpp \
	ex/ExistingIntervalTypeEx.hpp \
	ex/TimeRangeEx.hpp \
	ex/NodeFullEx.hpp \
	ex/IOEx.hpp \
	ex/InvalidFormatEx.hpp \
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

