PROG = a.out
OBJ_DIR = obj
SRC_DIR = src
INCLUDE_DIR = include

SOURCES = ${SRC_DIR}/%.cpp
OBJECTS = ${OBJ_DIR}/%.o

CFLAGS = -std=c++11 -Wall -pedantic -pthread -I${INCLUDE_DIR}
CC = g++ ${CFLAGS}

OBJS = $(patsubst ${SOURCES}, ${OBJECTS}, $(wildcard ${SRC_DIR}/*.cpp))

# If the first argument is "run"...
ifeq (run,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "run"
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(RUN_ARGS):;@:)
endif

.PHONY: build all run obj clean

build: obj ${PROG}

all: clean build

run: build
	./${PROG} $(RUN_ARGS)

${PROG}: ${OBJS} main.cpp
	$(CC) ${OBJS} main.cpp -o ${PROG}

${OBJECTS}: ${SOURCES}
	$(CC) -c $< -o $@

obj:
	mkdir -p ${OBJ_DIR}

clean:
	rm -rf ${OBJ_DIR} ${PROG}
