#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <time.h>

//File & Logging Functions
void log_msg(const char *filename, const char *sender, const char *message);
void quit_opt(const char *filename);

#endif // LOGGER_H
// logger.h