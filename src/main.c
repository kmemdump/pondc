#include "pond/pond.h"
#include "raylib.h"
#include <execinfo.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

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

int main(void) {
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
  g_width = SCREEN_WIDTH;
  g_height = SCREEN_HEIGHT;
  g_background = WHITE;

  Pond pond;
  pond_init(&pond, SCREEN_WIDTH, SCREEN_HEIGHT);

  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    BeginDrawing();
    pond_update(&pond);

    DrawFPS(10, 10);
    DrawText(TextFormat("frame: %.3f ms", GetFrameTime() * 1000.0f), 10, 30, 20,
             DARKGRAY);
    EndDrawing();
  }

  pond_free(&pond);

  CloseWindow();
  return 0;
}
