CC := cc
CFLAGS := -Isrc -I/usr/include
CDEBUGFLAGS := -g -rdynamic -fdiagnostics-color=always
# fsanitize=address exits the program before the program crash from a 
# segmentation fault
CDEBUGFLAGS_SANITIZE := -g -rdynamic -fsanitize=address -fdiagnostics-color=always
LDLIBS := -lraylib -lm

BUILD_DIR := build
FRAMES_DIR := frames
TARGET := $(BUILD_DIR)/pond
DEBUG_TARGET := $(TARGET)-debug
DASAN_TARGET := $(TARGET)-dasan
DEMO_TARGET := demo
LOG_DIR := logs
DEBUG_LOG := ./${LOG_DIR}/debug.log
DASAN_LOG := ./${LOG_DIR}/dasan.log
SRCS := src/main.c src/pond/log.c src/pond/pangenome.c src/pond/motor.c src/pond/appearance.c src/pond/organism.c src/pond/painter.c src/pond/population.c src/pond/pond.c

.PHONY: all build run clean

all: build debug dasan

build: $(TARGET)

dirs: 
	mkdir -p $(BUILD_DIR)
	mkdir -p $(FRAMES_DIR)

debug: dirs $(SRCS)
	$(CC) $(CDEBUGFLAGS) $(CFLAGS) $(SRCS) -o $(DEBUG_TARGET) $(LDLIBS)

dasan: dirs $(SRCS)
	$(CC) $(CDEBUGFLAGS_SANITIZE) $(CFLAGS) $(SRCS) -o $(DASAN_TARGET) $(LDLIBS)

debug_test: debug
	timeout 5000 ./${TARGET}-debug &> ${DEBUG_LOG}

dasan_test: dasan
	timeout 5000 ./${TARGET}-dasan &> ${DASAN_LOG}

video: 
	ffmpeg -y -framerate 30 -i ${FRAMES_DIR}/frame_%05d.png \
		-c:v libx264 -pix_fmt yuv420p -movflags +faststart ${DEMO_TARGET}.mp4

gif:
	ffmpeg -y -framerate 30 -i ${FRAMES_DIR}/frame_%05d.png \
		-vf "scale=480:-1:flags=lanczos,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" \
		${DEMO_TARGET}.gif

$(TARGET): dirs $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LDLIBS)

run: $(TARGET)

clean:
	rm -rf $(BUILD_DIR)
