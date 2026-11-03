#include "pond/log.h"
#include <stdio.h>

void pond_log(const char *prefix, const char *message) {
  printf("%s: %s\n", prefix, message);
}

void log_warn(const char *message) { pond_log("WARNING", message); }
void log_error(const char *message) { pond_log("ERROR", message); }
void log_debug(const char *message) { pond_log("DEBUG", message); }
void log_info(const char *message) { pond_log("INFO", message); }
