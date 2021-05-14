#Compiler features
CC      := g++
CFLAGS :=  -std=c++17 -W  -Wall      -Werror         -pedantic \
 		   -Wextra        -Wfloat-equal   -pedantic-errors \
 		   -Wcast-align   -Wpointer-arith -Wformat-security \
 		   -Wformat       -Wwrite-strings -Wcast-align \
 		   -Wunused       -Wcast-qual    -Wmissing-declarations \
		   -Wfatal-errors
LDFLAGS := -fsanitize=address

UTILS_DIR = ./../utils/
SVECTOR_DIR = ./../07/
PROJECT_NAME = bigint

all: test

test: test.o $(PROJECT_NAME).o block.o
	$(CC) $^ -o $@.out $(CFLAGS) $(LDFLAGS)
	./$@.out

test.o: test.cpp $(PROJECT_NAME).o
	$(CC) -c $< -o $@ $(CFLAGS) -I$(UTILS_DIR) -I$(SVECTOR_DIR)

$(PROJECT_NAME).o: $(PROJECT_NAME).cpp $(PROJECT_NAME).h block.o
	$(CC) -c $< -o $@ $(CFLAGS) -I$(UTILS_DIR) -I$(SVECTOR_DIR)

block.o: block.cpp block.h
	$(CC) -c $< -o $@ $(CFLAGS)

.PHONY: clean debug release

debug: CFLAGS += -g -O0 -DDEBUG
debug: test

release: CFLAGS += -O3 -DRELEASE
release: test

clean:
	rm -f *.o *.a test.out
