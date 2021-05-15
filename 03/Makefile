#Compiler features
CC      := g++
CFLAGS :=  -std=c++17 -W  -Wall      -Werror         -pedantic \
 		   -Wextra        -Wfloat-equal   -pedantic-errors \
 		   -Wcast-align   -Wpointer-arith -Wformat-security \
 		   -Wformat       -Wwrite-strings -Wcast-align \
 		   -Wunused       -Wcast-qual    -Wmissing-declarations
LDFLAGS := -fsanitize=address

TEST_RUNNER_DIR = ./../utils/
PROJECT_NAME = matrix

all: test

test: test.o lib$(PROJECT_NAME).a
	$(CC) $^ -o $@.out $(CFLAGS) $(LDFLAGS) -L. -l$(PROJECT_NAME)
	./$@.out

test.o: test.cpp lib$(PROJECT_NAME).a
	$(CC) -c $< -o $@ $(CFLAGS) -I$(TEST_RUNNER_DIR)

lib$(PROJECT_NAME).a: $(PROJECT_NAME).cpp $(PROJECT_NAME).h
	$(CC) -c $< -o $(PROJECT_NAME).o $(CFLAGS)
	ar rc $@ $(PROJECT_NAME).o

.PHONY: clean debug release

debug: CFLAGS += -g -O0 -DDEBUG
debug: test

release: CFLAGS += -O3 -DRELEASE
release: test

clean:
	rm -f *.o *.a test.out