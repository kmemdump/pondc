CC := gcc
CFLAGS := -Isrc -I/usr/include
CDEBUGFLAGS := -g -rdynamic -fsanitize=address
LDLIBS := -lraylib -lm

BUILD_DIR := build
TARGET := $(BUILD_DIR)/pond
SRCS := src/main.c src/pond/log.c src/pond/pangenome.c src/pond/motor.c src/pond/senses.c src/pond/appearance.c src/pond/organism.c src/pond/painter.c src/pond/population.c src/pond/pond.c

.PHONY: all build run clean

all: build

build: $(TARGET)

debug: $(SRCS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CDEBUGFLAGS) $(CFLAGS) $(SRCS) -o $(TARGET)-debug $(LDLIBS)

$(TARGET): $(SRCS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LDLIBS)

run: $(TARGET)

clean:
	rm -rf $(BUILD_DIR)
