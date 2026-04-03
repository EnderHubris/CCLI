TARGET = awesome_app
CC = gcc
CFLAGS = -Wall -Wextra

all: build
	@echo "Running program 1"
	@echo "-----------------"
	./$(TARGET)
	./$(TARGET) -v
	@echo
	@echo "Running program 2"
	@echo "-----------------"
	./$(TARGET)2
	./$(TARGET)2 -v
	@echo
	@echo "Running demo program"
	@echo "--------------------"
	./demo
	./demo -o outsiders.txt -t 23 -d 3.24
build:
	$(CC) $(CFLAGS) test.c -o $(TARGET)
	$(CC) $(CFLAGS) test2.c -o $(TARGET)2
	$(CC) $(CFLAGS) demo.c -o demo