#Compiler features
CC      := g++
CFLAGS :=  -std=c++17 -W  -Wall      -Werror         -pedantic \
 		   -Wextra        -Wfloat-equal   -pedantic-errors \
 		   -Wcast-align   -Wpointer-arith -Wformat-security \
 		   -Wformat       -Wwrite-strings -Wcast-align \
 		   -Wunused       -Wcast-qual    -Wmissing-declarations
LDFLAGS := -fsanitize=address

TEST_RUNNER_DIR = ./../utils/

test: tests
	./tests

tests: main.o allocator.o
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

main.o: main.cpp allocator.o
	$(CC) -c $< -o $@ $(CFLAGS) -I $(TEST_RUNNER_DIR)

allocator.o: allocator.cpp allocator.h
	$(CC) -c $< -o $@ $(CFLAGS)

.PHONY: clean

clean:
	rm *.o tests