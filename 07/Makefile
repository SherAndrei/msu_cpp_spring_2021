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

all: test

test: test.o
	$(CC) $^ -o $@.out $(CFLAGS) $(LDFLAGS)
	./$@.out

test.o: test.cpp Vector.h Vector.tcc Allocator.h Iterator.h
	$(CC) -c $< -o $@ $(CFLAGS) -I$(UTILS_DIR)

.PHONY: clean debug release

debug: CFLAGS += -g -O0 -DDEBUG
debug: test

release: CFLAGS += -O3 -DRELEASE
release: test

clean:
	rm -f *.o *.a test.out
