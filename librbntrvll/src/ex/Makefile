# exceptions Makefile
#
# author:	Philippe Proulx <philippe.proulx@polymtl.ca>
include ../Makefile.inc

TRG = out.a
SRC = \
	UnknownIntervalTypeEx.cpp \
	UnknownNodeTypeEx.cpp \
	ExistingIntervalTypeEx.cpp \
	TimeRangeEx.cpp \
	NodeFullEx.cpp \
	IOEx.cpp \
	InvalidFormatEx.cpp
H_SRC = \
	UnknownIntervalTypeEx.hpp \
	UnknownNodeTypeEx.hpp \
	ExistingIntervalTypeEx.hpp \
	TimeRangeEx.hpp \
	NodeFullEx.hpp \
	IOEx.hpp \
	InvalidFormatEx.hpp

# automatic
OBJ = $(SRC:.cpp=.o)

# phony targets
.PHONY: clean cleanall

# objects
%.o: %.cpp $(H_SRC)
	@echo $(CXX_TEXT) $@
	@$(CPP) $(CPPFLAGS) -c -fPIC -o $@ $<

# subdir library
$(TRG): $(OBJ)
	@echo "[AR]	" $@
	@$(AR) $@ $^

# clean target
clean:
	$(RM) $(OBJ) $(TRG)

cleanall: clean

