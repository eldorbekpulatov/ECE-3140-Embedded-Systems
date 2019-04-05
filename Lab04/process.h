#ifndef __PROCESS_H_INCLUDED__
#define __PROCESS_H_INCLUDED__

#include "3140_concur.h"
#include <fsl_device_registers.h>
#include "shared_structs.h"

extern process_t * current_process; 
extern process_t * process_queue;
extern process_t * process_tail;

void push_tail_process(process_t *proc);

#endif
