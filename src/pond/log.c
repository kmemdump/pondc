#include "pond/log.h"
#include <stdio.h>

void pond_log(const char *prefix, const char *message)
{
    printf("%s: %s\n", prefix, message);
}

void pond_log_warning(const char *message) { pond_log("WARNING", message); }
void pond_log_error(const char *message) { pond_log("ERROR", message); }
void pond_log_debug(const char *message) { pond_log("DEBUG", message); }
void pond_log_info(const char *message) { pond_log("INFO", message); }
