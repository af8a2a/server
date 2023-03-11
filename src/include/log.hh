#include <cstdarg>
#include <cstdio>
#include <syslog.h>

enum loglevel{LOG_INFO_L,LOG_DEBUG_L,LOG_USER_L};

extern int log_level;

void Setlogprio(int level);

void Log(int level, const char *filename, int linenum, const char *format, ...);
