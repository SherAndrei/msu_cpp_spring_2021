#Compiler features
CC      := g++-10
CFLAGS :=  -std=c++20 -W  -Wall      -Werror         -pedantic \
 		   -Wextra        -Wfloat-equal   -pedantic-errors \
 		   -Wcast-align   -Wpointer-arith -Wformat-security \
 		   -Wformat       -Wwrite-strings -Wcast-align \
 		   -Wunused       -Wcast-qual    -Wmissing-declarations
LDFLAGS := -fsanitize=address

UTILS_DIR = ./../utils/
PROJECT_NAME = bigint

# all: test
# 	./test

test: test.o $(PROJECT_NAME).o
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

test.o: test.cpp $(PROJECT_NAME).o
	$(CC) -c $< -o $@ $(CFLAGS) -I$(UTILS_DIR)

$(PROJECT_NAME).o: $(PROJECT_NAME).cpp $(PROJECT_NAME).h
	$(CC) -c $< -o $@ $(CFLAGS) -I$(UTILS_DIR)

.PHONY: clean debug release

debug: CFLAGS += -g -O0 -DDEBUG
debug: test

release: CFLAGS += -O3 -DRELEASE
release: test

clean:
	rm -f *.o *.a test
