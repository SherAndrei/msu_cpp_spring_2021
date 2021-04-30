#Compiler features
CC      := g++
CFLAGS :=  -std=c++17 -W  -Wall      -Werror         -pedantic \
 		   -Wextra        -Wfloat-equal   -pedantic-errors \
 		   -Wcast-align   -Wpointer-arith -Wformat-security \
 		   -Wformat       -Wwrite-strings -Wcast-align \
 		   -Wunused       -Wcast-qual    -Wmissing-declarations
LDFLAGS := -fsanitize=address

TEST_RUNNER_DIR = ./../utils/

test.out: test.o libparser.a
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS) -L. -lparser
	./$@

test.o: test.cpp libparser.a
	$(CC) -c $< -o $@ $(CFLAGS) -I$(TEST_RUNNER_DIR)

libparser.a: parser.cpp parser.h
	$(CC) -c $< -o parser.o $(CFLAGS)
	ar rc $@ parser.o

.PHONY: clean debug release

debug: CFLAGS += -g -O0 -DDEBUG
debug: test.out

release: CFLAGS += -O3 -DRELEASE
release: test.out

clean:
	rm -f *.o *.a test.out