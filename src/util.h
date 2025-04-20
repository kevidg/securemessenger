#ifndef UTIL_H
#define UTIL_H

#include <pwd.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

/ User Input Functions
int validate_ip(const char *in_addr);
const char *get_default_name();

#endif // UTIL_H
// util.h