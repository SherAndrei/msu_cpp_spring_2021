#Compiler features
CC      := g++
CFLAGS :=  -std=c++17 -W  -Wall      -Werror         -pedantic \
 		   -Wextra        -Wfloat-equal   -pedantic-errors \
 		   -Wcast-align   -Wpointer-arith -Wformat-security \
 		   -Wformat       -Wwrite-strings -Wcast-align \
 		   -Wunused       -Wcast-qual    -Wmissing-declarations
LDFLAGS := -fsanitize=address

TEST_RUNNER_DIR = ./../utils/
PROJECT_NAME = allocator

all: test

test: main.o $(PROJECT_NAME).o
	$(CC) $^ -o $@.out $(CFLAGS) $(LDFLAGS)
	./$@.out

main.o: main.cpp $(PROJECT_NAME).o
	$(CC) -c $< -o $@ $(CFLAGS) -I $(TEST_RUNNER_DIR)

$(PROJECT_NAME).o: $(PROJECT_NAME).cpp $(PROJECT_NAME).h
	$(CC) -c $< -o $@ $(CFLAGS)

.PHONY: clean debug release

debug: CFLAGS += -g -O0 -DDEBUG
debug: test

release: CFLAGS += -O3 -DRELEASE
release: test

clean:
	rm -f *.o *.a test.out
