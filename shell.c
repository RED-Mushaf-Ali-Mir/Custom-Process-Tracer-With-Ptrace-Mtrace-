#include "TUI.h"
#include "Ptracer.h"
#include "Shell.h"

void shell (void) {
    while(1){
    unsigned int filter = 0;
    pid_t pid = RenderMenu(&filter);
    do_tracer(pid, filter);
    }
}