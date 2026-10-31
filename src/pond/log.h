#ifndef POND_LOG_H
#define POND_LOG_H

void pond_log(const char *prefix, const char *message);
void pond_log_warning(const char *message);
void pond_log_error(const char *message);
void pond_log_debug(const char *message);
void pond_log_info(const char *message);

#endif
