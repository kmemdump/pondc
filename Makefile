CC := gcc
CFLAGS := -I/usr/include
LDLIBS := -lraylib

BUILD_DIR := build
TARGET := $(BUILD_DIR)/pond

.PHONY: all build run clean

all: build

build: $(TARGET)

$(TARGET): src/main.c
	mkdir -p $(BUILD_DIR)
	$(CC) src/main.c -o $(TARGET) $(CFLAGS) $(LDLIBS)

run: $(TARGET)

clean:
	rm -rf $(BUILD_DIR)
