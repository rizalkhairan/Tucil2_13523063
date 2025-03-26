CC			= g++
SRC_FILES	= $(wildcard src/*.cpp)
TARGET		= bin/main

build: $(SRC_FILES)
	$(CC) -o $(TARGET) $(SRC_FILES)

run:
	./$(TARGET)

all: build run