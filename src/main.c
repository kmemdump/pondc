#include "pond/pond.h"
#include "pond/types.h"
#include "raylib.h"
#include <execinfo.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 1200;
const char *RECORDED_FRAMES_DIR = "frames";

int g_width = SCREEN_WIDTH;
int g_height = SCREEN_HEIGHT;

typedef struct Args Args;
struct Args {
  bool record;
  int recordedFrames;
};

int log_file_descriptor = -1;

void signal_action_handler(int sig) {
  if (log_file_descriptor != -1) {
    const char *msg = "\nSEGMENTATION FAULT CALLBACK STACK DUMP\n\n.";
    write(log_file_descriptor, msg, 43);

    void *buffer[20];
    int nptrs = backtrace(buffer, 20);
    backtrace_symbols_fd(buffer, nptrs, log_file_descriptor);

    fsync(log_file_descriptor);
    close(log_file_descriptor);
  }

  signal(sig, SIG_DFL);
  raise(sig);
}

int main(int argc, char **argv) {
  Args args;
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--record") == 0) {
      args.record = true;
    }
  }

  log_file_descriptor =
      open("./logs/sigsev.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
  if (log_file_descriptor == -1) {
    perror("Failed to open log file; app.log");
    return 1;
  }

  struct sigaction sa;
  sa.sa_handler = signal_action_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESETHAND;

  if (sigaction(SIGSEGV, &sa, NULL) == -1) {
    perror("Failed to set up signal handler");
    return 1;
  }

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pond");

  Pond pond;
  pond_init(&pond, SCREEN_WIDTH, SCREEN_HEIGHT);

  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    BeginDrawing();
    pond_update(&pond);

    if (args.record) {
      char path[64];
      snprintf(path, 64, "%s/frame_%05d.png", RECORDED_FRAMES_DIR,
               args.recordedFrames++);
      TakeScreenshot(path);
    } else {
      DrawFPS(10, 10);
      DrawText(TextFormat("frame: %.3f ms", GetFrameTime() * 1000.0f), 10, 30,
               20, DARKGRAY);
    }

    EndDrawing();
  }

  pond_free(&pond);

  CloseWindow();
  return 0;
}
