#ifndef TUI_H
#define TUI_H

#include <sys/types.h>

pid_t RenderMenu(unsigned int *out_filter);
int pidByname(char*);

#endif