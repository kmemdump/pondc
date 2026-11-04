CC := gcc
CFLAGS := -Isrc -I/usr/include
CDEBUGFLAGS := -g -rdynamic -fdiagnostics-color=always
# fsanitize=address exits the program before the program crash from a 
# segmentation fault
CDEBUGFLAGS_SANITIZE := -g -rdynamic -fsanitize=address -fdiagnostics-color=always
LDLIBS := -lraylib -lm

BUILD_DIR := build
TARGET := $(BUILD_DIR)/pond
DEBUG_TARGET := $(TARGET)-debug
DASAN_TARGET := $(TARGET)-dasan
LOG_DIR := logs
DEBUG_LOG := ./${LOG_DIR}/debug.log
DASAN_LOG := ./${LOG_DIR}/dasan.log
SRCS := src/main.c src/pond/log.c src/pond/pangenome.c src/pond/motor.c src/pond/senses.c src/pond/appearance.c src/pond/organism.c src/pond/painter.c src/pond/population.c src/pond/pond.c

.PHONY: all build run clean

all: build

build: $(TARGET)

debug: $(SRCS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CDEBUGFLAGS) $(CFLAGS) $(SRCS) -o $(DEBUG_TARGET) $(LDLIBS)

dasan: $(SRCS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CDEBUGFLAGS_SANITIZE) $(CFLAGS) $(SRCS) -o $(DASAN_TARGET) $(LDLIBS)

debug_test: debug
	timeout 5000 ./${TARGET}-debug &> ${DEBUG_LOG}

dasan_test: dasan
	timeout 5000 ./${TARGET}-dasan &> ${DASAN_LOG}

$(TARGET): $(SRCS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LDLIBS)

run: $(TARGET)

clean:
	rm -rf $(BUILD_DIR)
