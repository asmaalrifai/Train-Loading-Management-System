# prgram:
# 	gcc src/file_ops.c src/material.c src/test.c src/train.c src/utils.c src/wagon.c src/main.c -I include -o test


CC = gcc
CFLAGS = -Iinclude -Wall
SRC = src/main.c src/train.c src/wagon.c src/material.c src/file_ops.c src/utils.c
OBJ = $(SRC:.c=.o)
TARGET = train_manager

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
