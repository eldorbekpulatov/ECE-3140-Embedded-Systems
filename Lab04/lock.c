#include "lock.h"

// helper function declarations - implemented below
void enqueue_blocked(process_t *process, lock_t *l);  
process_t* dequeue_blocked(lock_t *l); 


/**
 * Initialises the lock structure
 *
 * @param l pointer to lock to be initialised
 */
void l_init(lock_t* l){
	l->state = 0; // initialize as unlocked
	l->blocked_queue_head = NULL; // initialize as empty queue
	l->blocked_queue_tail = NULL;
};

/**
 * Grab the lock or block the process until lock is available
 *
 * @param l pointer to lock to be grabbed
 */
void l_lock(lock_t* l){
	__disable_irq(); //disables interrupts
	if(l->state == 0){  // if unlocked 
		l->state = 1; // lock it
	}else{
		current_process ->blocked = 1;  // flag the process as blocked
		enqueue_blocked(current_process, l); // add it to the given lock's queue
		process_blocked(); // call given function -> saves current state and switches context
	}
	__enable_irq(); //enables interrupts
};

/**
 * Release the lock along with the first process that may be waiting on
 * the lock. This ensures fairness wrt lock acquisition.
 *
 * @param l pointer to lock to be unlocked
 */
void l_unlock(lock_t* l){
	__disable_irq(); //disables interrupts
	if(!l->blocked_queue_head){  // if no competing process
		l->state = 0; // unlock
	}else{
		process_t *process = dequeue_blocked(l);  // return a pointer to the next competing process
		process -> blocked = 0;  // set process as unblocked
		push_tail_process(process); // add the unblocked process to the process queue
	}
	__enable_irq(); //enables interrupts
};


/**
* Enqueues the blocked process to the lock's blocked queue
**/
void enqueue_blocked(process_t *process, lock_t *l){
	if (!l->blocked_queue_head) {
		l->blocked_queue_head = process;
	}
	if (l->blocked_queue_tail) {
		l->blocked_queue_tail->next = process;
	}
	l->blocked_queue_tail = process;
	process->next = NULL;
};


/**
* Dequeues a process from lock's blocked queue and returns a ptr to it
**/
process_t* dequeue_blocked(lock_t *l){
	if (!l->blocked_queue_head) return NULL;
	process_t *proc = l->blocked_queue_head;
	l->blocked_queue_head = proc->next;
	if (l->blocked_queue_tail == proc) {
		l->blocked_queue_tail = NULL;
	}
	proc->next = NULL;
	return proc;
};  
