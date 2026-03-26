TARGET = awesome_app
CC = gcc
CFLAGS = -Wall

all: build
	./$(TARGET)
build:
	$(CC) test.c -o $(TARGET)