# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -I include

# Source files
SRC = src/file_ops.c src/material.c src/train.c src/utils.c src/wagon.c src/main.c

# Output executable
TARGET = program

# Default rule
all: $(TARGET)

# Build the program
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@

# Clean build artifacts
clean:
	rm -f $(TARGET) *.o