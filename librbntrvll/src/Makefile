# main Makefile
#
# author:	Philippe Proulx <philippe.proulx@polymtl.ca>
include Makefile.inc

# subdirectories
SUBDIRS = ex intervals

# files
SRC = \
	AbstractHistoryTree.cpp \
	OutHistoryTree.cpp \
	InHistoryTree.cpp \
	HistoryTree.cpp \
	AbstractThreadedHistoryTree.cpp \
	ThreadedInHistoryTree.cpp \
	ThreadedOutHistoryTree.cpp \
	AbstractNode.cpp \
	CoreNode.cpp \
	LeafNode.cpp \
	HistoryTreeConfig.cpp \
	IntervalCreator.cpp
H_SRC = \
	AbstractHistoryTree.hpp \
	OutHistoryTree.hpp \
	InHistoryTree.hpp \
	HistoryTree.hpp \
	AbstractThreadedHistoryTree.hpp \
	ThreadedInHistoryTree.hpp \
	ThreadedOutHistoryTree.hpp \
	AbstractNode.hpp \
	CoreNode.hpp \
	LeafNode.hpp \
	HistoryTreeConfig.hpp \
	IPrintable.hpp \
	IntervalCreator.hpp \
	fixed_config.h \
	basic_types.h
	
# target
TARGET_NAME = librbntrvll
TARGET_SONAME = $(TARGET_NAME).so.1
TARGET_SO = $(TARGET_NAME).so.1.0
TARGET_PLAIN = $(TARGET_NAME).so

# automatic
OBJ = $(SRC:.cpp=.o)
SUBDIRS_LIBS = $(addsuffix /$(OUTLIB_NAME), $(SUBDIRS))

# phony targets
.PHONY: all clean cleanall subdirs $(SUBDIRS)

# main target
all: $(TARGET_SO)

# shared library
$(TARGET_SO): $(OBJ) subdirs
	@echo "[LD]	" $@
	@$(CPP) -shared -Wl,-soname,$(TARGET_SONAME) -o $@ $(OBJ) $(SUBDIRS_LIBS) /usr/lib/libboost_thread.so
	@ln -f -s $(TARGET_SO) $(TARGET_SONAME)
	@ln -f -s $(TARGET_SO) $(TARGET_PLAIN)

# objects
%.o: %.cpp $(H_SRC)
	@echo $(CXX_TEXT) $@
	@$(CPP) $(CPPFLAGS) -c -fPIC -lboost_thread -o $@ $<

# subdirectories:
subdirs: $(SUBDIRS)

$(SUBDIRS):
	@$(MAKE) -C $@

# clean targets
clean:
	$(RM) $(OBJ) $(TARGET_SO) $(TARGET_SONAME) $(TARGET_PLAIN)

cleanall: clean
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
