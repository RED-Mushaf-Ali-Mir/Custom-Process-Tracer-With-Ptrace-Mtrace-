#include "Ptracer.h"
#include "TUI.h"

void shell(void){    
    while(1){
    pid_t tracee_id = RenderMenu();
    do_tracer (tracee_id);
    }
}