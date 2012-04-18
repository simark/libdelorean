# Makefile ben ben simple


# Dossiers pour include (-I)
INC_DIR=.

# Dossier contenant la source à tester
SRC_DIR=../include/rbrntrvll

# Librairie à tester
LIB_PATH = ../src/.libs
LIB =  -lboost_thread -L$(LIB_PATH) -lrbrntrvll

# Fichier sources contenant les tests
TEST_SRC=\
	interval/IntervalTest.cpp \
	interval/IntervalCreatorTest.cpp \
	interval/CustomInterval.cpp \
	interval/CustomIntervalFactory.cpp \
	node/NodeTest.cpp \
	tree/TreeTest.cpp \
	tree/ThreadedTreeTest.cpp \
	tree/MemoryTreeTest.cpp

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

