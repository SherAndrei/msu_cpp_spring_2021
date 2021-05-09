#Compiler features
CC      := g++
CFLAGS :=  -std=c++17 -W  -Wall      -Werror         -pedantic \
 		   -Wextra        -Wfloat-equal   -pedantic-errors \
 		   -Wcast-align   -Wpointer-arith -Wformat-security \
 		   -Wformat       -Wwrite-strings -Wcast-align \
 		   -Wunused       -Wcast-qual    -Wmissing-declarations
LDFLAGS := -fsanitize=address

UTILS_DIR = ./../utils/
PROJECT_NAME = serialize

all: test.out
	./test.out

test.out: test.o $(PROJECT_NAME).o
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

test.o: test.cpp $(PROJECT_NAME).o
	$(CC) -c $< -o $@ $(CFLAGS) -I$(UTILS_DIR)

$(PROJECT_NAME).o: $(PROJECT_NAME).cpp $(PROJECT_NAME).h sererr.h
	$(CC) -c $< -o $@ $(CFLAGS) -I$(UTILS_DIR)

.PHONY: clean debug release

debug: CFLAGS += -g -O0 -DDEBUG
debug: test.out

release: CFLAGS += -O3 -DRELEASE
release: test.out

clean:
	rm -f *.o *.a test.out
