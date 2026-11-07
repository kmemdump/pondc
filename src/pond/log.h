#ifndef POND_LOG_H
#define POND_LOG_H

#include <stdio.h>

void pond_log(const char *prefix, const char *message);
void log_warn(const char *message);
void log_error(const char *message);
void log_debug(const char *message);
void log_info(const char *message);

#endif
