# Compiler and flags
CC = gcc
CFLAGS = -Wall

# Libraries
LDFLAGS = -lGL -lGLEW -lglfw -lcglm -lm

# Source and output
SRC = main.c
OUT = main

# Default target
all: $(OUT)

# Link the program
$(OUT): $(SRC)
	$(CC) $(SRC) -o $(OUT) $(LDFLAGS)

# Clean up
clean:
	rm -f $(OUT)

.PHONY: all clean
