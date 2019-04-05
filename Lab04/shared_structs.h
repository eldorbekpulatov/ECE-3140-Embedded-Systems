#ifndef __SHARED_STRUCTS_H__
#define __SHARED_STRUCTS_H__
#include "3140_concur.h"
#include "utils.h"
#include <stdlib.h>
#include <fsl_device_registers.h>


/** Implement your structs here */

/**
 * This structure holds the process structure information
 */
struct process_state {
		unsigned int *orig_sp; 
		unsigned int *cursp;
		unsigned int blocked; // indicates whether process is blocked (1) or not (0)
		unsigned int size;
		process_t *next ;
};
/**
 * This defines the lock structure
 */
typedef struct lock_state {
	unsigned int state; // 1 if locked; 0 otherwise
	process_t *blocked_queue_head;
	process_t *blocked_queue_tail;
} lock_t;

/**
 * This defines the conditional variable structure
 */
typedef struct cond_var {

} cond_t;

#endif
