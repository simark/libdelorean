# Makefile ben ben simple


# Dossiers pour include (-I)
INC_DIR=.

# Dossier contenant la source à tester
SRC_DIR=../src

# Librairie à tester
LIB_PATH = ../src/.libs
LIB = -L$(LIB_PATH) -lrbrntrvll

# Fichier sources contenant les tests
TEST_SRC=\
	node/NodeTest.cpp \
	interval/IntervalTest.cpp \
	interval/IntervalCreatorTest.cpp \
	interval/CustomInterval.cpp \
	interval/CustomIntervalFactory.cpp \
	tree/TreeTest.cpp

# Fichier source démarrant les tests
RUN_SRC=test.cpp

## Fin des paramètres à modifier

SRC:=$(addprefix $(SRC_DIR)/, $(SRC))
CFLAGS=$(addprefix -I$(SRC_DIR)/, $(INC_DIR))

.PHONY: run
run: test
	LD_LIBRARY_PATH=../src/.libs ./test

test: $(SRC) $(TEST_SRC) $(RUN_SRC)

	g++  -I./interval/ $(CFLAGS) -o test $(TEST_SRC) $(RUN_SRC) $(LIB) -lcppunit

