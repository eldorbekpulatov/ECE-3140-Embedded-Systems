#include "3140_concur.h"
#include "realtime.h" 
#include <stdlib.h>
#include <fsl_device_registers.h> 

// process state is PCB and represents node on LL 
struct process_state {
	// keep track of original sp in order to free
	unsigned int *orig_sp;
	// track current sp in case process is interrupted
	unsigned int *sp;
	// track size of process to run properly and free
	unsigned int size;
	// track next process_state which is next node in LL
	process_t *next;
	// 1 if rt, 0 if not rt
	unsigned int rt;
	// hold start time
	realtime_t *start;
	// hold deadline
	realtime_t *deadline;
};

// initialize fields 
realtime_t current_time = {0, 0};
process_t *current_process = NULL;

process_t *process_queue_head   	 = NULL;
process_t *process_queue_tail 	 	 = NULL;
process_t *rt_unready_queue_head   = NULL;
process_t *rt_unready_queue_tail   = NULL;
process_t *rt_ready_queue_head   	 = NULL;
process_t *rt_ready_queue_tail     = NULL;

// track num deadlines met/missed
int process_deadline_met  = 0;
int process_deadline_miss = 0;


/* Frees space of a given proc */
void process_free(process_t *proc) {
	process_stack_free(proc->orig_sp, proc->size);
	free(proc);
}

/*  PIT Interrupt Handler  */
void PIT1_IRQHandler(void) {
	NVIC_DisableIRQ(PIT1_IRQn); //disable timer
	PIT->CHANNEL[1].TFLG = 0x1;	//reset TFLG
	current_time.msec++;
	if ( current_time.msec == 1000 ) {
		current_time.msec = 0;
		current_time.sec++;
	}
	NVIC_EnableIRQ(PIT1_IRQn); //enable timer and interrupt		
};

/*
 * This function sets up a period interrupt to be generated using the PIT[0] interrupt.
 * It ends with a call to process_begin(), which initiates the first process.
 */
void process_start() {
	// Enable clock to PIT module
	SIM->SCGC6 = SIM_SCGC6_PIT_MASK;  
	//enable timer and interrupt
	PIT->MCR = (0<<1);
	PIT->CHANNEL[0].LDVAL = DEFAULT_SYSTEM_CLOCK / 10;
	PIT->CHANNEL[1].LDVAL = DEFAULT_SYSTEM_CLOCK / 1000;
	NVIC_EnableIRQ(PIT0_IRQn);
	NVIC_EnableIRQ(PIT1_IRQn);
	NVIC_EnableIRQ(SVCall_IRQn);
	// Don't enable the timer yet.
	PIT->CHANNEL[1].TCTRL = 3; 
	NVIC_SetPriority(PIT1_IRQn,0);
	NVIC_SetPriority(SVCall_IRQn,1);
	NVIC_SetPriority(PIT0_IRQn,2);
	
	if (!process_queue_head && !rt_ready_queue_head && !rt_unready_queue_head ) {
		// Bail out fast if no processes were ever created
		return;
	}else{
		// call process begin to start
		process_begin();
	}
};


// helper functions
unsigned int getVal(realtime_t *time){
	return (time->sec*1000)+(time->msec);
};
unsigned int getStart(process_t *proc){
	return getVal(proc->start);
};
unsigned int getDeadline(process_t *proc){
	return getVal(proc->start)+getVal(proc->deadline);
};

/* enqueues besed on START time */
void enqueue_unready(process_t *proc){
	proc->next = NULL; // clean the node
	if(!rt_unready_queue_head){ // unready queue empty
		rt_unready_queue_head = proc;
		rt_unready_queue_tail = proc;
	}else{ // unready queue not empty
		if (!rt_unready_queue_head->next){ //one process in queue
			if (getStart(proc) > getStart(rt_unready_queue_head)){
				rt_unready_queue_head->next = proc;
				rt_unready_queue_tail = proc;
			}else {
				proc->next = rt_unready_queue_head;
				rt_unready_queue_head = proc;
			}
		}else { // more than one in the queue
			process_t *temp1 = NULL;
			process_t *temp2 = rt_unready_queue_head;
			while(getStart(proc) > getStart(temp2)&& temp2){
				temp1 = temp2;
				temp2 = temp2->next;
			}
			proc->next = temp1->next;
			temp1->next = proc;
		}
	}			
}

/* enqueues besed on DEADLINE time */
void enqueue_ready(process_t *proc){
	proc->next = NULL; // clean the node
	if(!rt_ready_queue_head){ // unready queue empty
		rt_ready_queue_head = proc;
		rt_ready_queue_tail = proc;
	}else{ // unready queue not empty
		if (!rt_ready_queue_head->next){ //one process in queue
			if (getDeadline(proc) > getDeadline(rt_ready_queue_head)){
				rt_ready_queue_head->next = proc;
				rt_ready_queue_tail = proc;
			}else {
				proc->next = rt_ready_queue_head;
				rt_ready_queue_head = proc;
			}
		}else { // more than one in the queue
			process_t *temp1 = NULL;
			process_t *temp2 = rt_ready_queue_head;
			while(getDeadline(proc) > getDeadline(temp2) && temp2){
				temp1 = temp2;
				temp2 = temp2->next;
			}
			proc->next = temp1->next;
			temp1->next = proc;
		}
	}			
}

/* ENQUEUE */
void enqueue(process_t *proc) {
	if (proc->rt){ // real-time
		if(getVal(&current_time) < getStart(proc)){ // unready, sort by arrival
			enqueue_unready(proc);
		}else{ // ready, sort by deadline
			enqueue_ready(proc);
		}
	}else{ // non-real-time
		if (!process_queue_head) {
			process_queue_head = proc;
		}
		if (process_queue_tail) {
			process_queue_tail->next = proc;
		}
		process_queue_tail = proc;
		proc->next = NULL;
	}
}

/* DEQUEUE  */
process_t* dequeue(process_t *queue_head, process_t *queue_tail) {
	if (!queue_head) return NULL;
	process_t *proc = queue_head;
	queue_head = proc->next;
	if (queue_tail == proc) {
		queue_tail = NULL;
	}
	proc->next = NULL;
	return proc;
};


/*
 * This function creates a process that starts at function f, with an initial stack size of n. It should
 * return -1 on an error, and 0 on success. The implementation of this function may require that
 * you allocate memory for a process_t structure, and you can use malloc() for this purpose
 */
int process_create (void (*f)(void), int n) {	
	// create new process_stack on heap
	unsigned int *sp = process_stack_init(f, n);
	// check if either malloc failed, in which case return -1
	if (sp == NULL) return -1;
	// create new process_state on heap
	process_t *new_proc = (process_t*) malloc(sizeof(process_t));
	if (new_proc == NULL) {
		process_stack_free(sp, n);
		return -1;
	}
	// set process_state fields
	new_proc->next = NULL;
	new_proc->size = n;
	new_proc->orig_sp = sp;
	new_proc->sp = sp;
	new_proc->rt = 0;
	new_proc->deadline = NULL;
	new_proc->start = NULL;
	enqueue(new_proc); //enqueue
	return 0;
}


/* Create a new realtime process out of the function f with the given parameters.
 * Returns -1 if unable to malloc a new process_t, 0 otherwise. */
int process_rt_create(void (*f)(void), int n, realtime_t* start, realtime_t* deadline){
	// create new process_stack on heap
	unsigned int *sp = process_stack_init(f, n);
	// check if either malloc failed, in which case return -1
	if (!sp) return -1;
	// create new process_state on heap
	process_t *new_proc = (process_t*) malloc(sizeof(process_t));
	if (new_proc == NULL) {
		process_stack_free(sp, n);
		return -1;
	}
	// set process_state fields
	new_proc->next = NULL;
	new_proc->size = n;
	new_proc->orig_sp = sp;
	new_proc->sp = sp;
	new_proc->rt = 1;
	new_proc->deadline = deadline;
	new_proc->start = start;
	enqueue(new_proc); //enqueue
	return 0;
}


/*
 * This function is called to select the next ready process. 
 * Cursp will be the current stack pointer for the currently running process (NULL if there
 * is no currently running process). If there is no process ready, this returns NULL; 
 * if there is a ready process, this returns the value of the stack pointer for the next 
 * ready process. 
 */
unsigned int * process_select (unsigned int * cursp) { // TODO: modify to support real-time
	
	// take ready processes off unready queue and move them to ready queue
	while (rt_unready_queue_head != NULL && getStart(rt_unready_queue_head) < getVal(&current_time)) {
			enqueue_ready(dequeue(rt_unready_queue_head, rt_unready_queue_tail));
	}
	
	if (cursp) { // process has not finished
		current_process->sp = cursp; // save the state
		enqueue(current_process); // re-enqueue
	} 
	else { // process has finished
		if (current_process) { // process was running 
			if (current_process->rt){ // real-time
				if (getDeadline(current_process) > getVal(&current_time)){ // met the deadline
					process_deadline_met +=1;
				}
				else{ // did not meet the deadline
					process_deadline_miss +=1;
				}
			}
			process_free(current_process);  // free the space
		}
	}
	
	if (rt_ready_queue_head){ //  ready processes exist
		current_process = dequeue(rt_ready_queue_head, rt_ready_queue_tail); 
	}else{ //  no ready processes
		current_process = dequeue(process_queue_head, process_queue_tail);
	}
	
	if (current_process) { // process was selected
		return current_process->sp; // Launch the process which was just popped off the queue
	} else { // no process was selected
		return NULL;  // exit the scheduler
	}
}
