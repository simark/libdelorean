# intervals Makefile
#
# author:	Philippe Proulx <philippe.proulx@polymtl.ca>
include ../Makefile.inc

TRG = out.a
SRC = \
	AbstractInterval.cpp \
	StringInterval.cpp \
	StringIntervalFactory.cpp \
	UIntInterval.cpp \
	UIntIntervalFactory.cpp \
	NullInterval.cpp \
	NullIntervalFactory.cpp \
	IntInterval.cpp \
	IntIntervalFactory.cpp \
	FloatInterval.cpp \
	FloatIntervalFactory.cpp
H_SRC = \
	IIntervalFactory.hpp \
	AbstractInterval.hpp \
	StringInterval.hpp \
	StringIntervalFactory.hpp \
	UIntInterval.hpp \
	UIntIntervalFactory.hpp \
	NullInterval.hpp \
	NullIntervalFactory.hpp \
	IntInterval.hpp \
	IntIntervalFactory.hpp \
	FloatInterval.hpp \
	FloatIntervalFactory.hpp

# automatic
OBJ = $(SRC:.cpp=.o)

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

