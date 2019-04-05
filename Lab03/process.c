#include "3140_concur.h" 
#include <stdlib.h> 
#include <fsl_device_registers.h>

// Node 
struct process_state {
		unsigned int *sp; 
		unsigned int *cursp;
		int size;
		struct process_state *next ;
};


/** Appends process to end of process_queue **/
void enqueue(process_t *process){
		if (process_queue == NULL) {
			process_queue = process;        
			process->next = NULL;    
		}else {
			process_t *tmp = process_queue;
			while (tmp->next != NULL) {
				// while there are more elements in the list    
				tmp = tmp->next;
			}        
			// now tmp is the last element in the list   
			tmp->next = process;  
			process->next = NULL;
		};
};


/**  Removes and returns the process at the head of the queue **/
process_t* dequeue(){
	if (process_queue == NULL) {
     return process_queue;
  }else {
		process_t *tmp = process_queue;
		process_queue = process_queue->next;	
		tmp->sp = process_queue->sp; 
		tmp->next = NULL; // clear the ptr to next queue element
		return tmp;
	}; 	
};	


// initial state of the processor
process_t * current_process = NULL;
process_t * process_queue = NULL;


/* Create a new process. Return -1 if creation failed */
int process_create (void (*f)(void), int n){
	
	// allocate space in heap using provided function
	unsigned int *sp = process_stack_init((*f), n);
	
	if(sp == NULL){
		// fail
		return -1;
	}else{
		// allocate space for process_state structure
		process_t *new_process =(process_t*)malloc(sizeof(process_t)); 
		// initialize the new_process
		new_process->sp = sp;  
		new_process->cursp = sp;
		new_process->size = n;
		new_process->next = NULL; 

		// add the process to process_queue
		enqueue(new_process); 
		// success
		return 0;
	};
};



/* Starts up the concurrent execution */
void process_start (void){
	//Timer initialization
	NVIC_EnableIRQ(PIT0_IRQn); /* enable PIT0 Interrupts */
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK; // Enable clock to PIT module
	PIT->MCR = 0;  // enable PIT MCR
	PIT->CHANNEL[0].LDVAL = 0x3D090; //250000- approx 1/100 second 
	
	// starts off the processor by executing from the process queue
	process_begin();
};



/** This function is called by our provided (PIT0) interrupt 
handler code to select the next ready process. cursp will be 
the current stack pointer for the currently running process 
(NULL if there is no currently running process). If there is 
no process ready, this should return NULL; if there is a ready 
process, this must return the value of the stack pointer for 
the next ready process. Your implementation must always maintain 
the global variable:  
		process_t *current_process 
as the currently running process. This variable is set to 
NULL when a process terminates, and should also be NULL until 
process_start() is called. **/
unsigned int * process_select (unsigned int * cursp){
	unsigned int* return_val;
	if(cursp == NULL){
		// terminated or no program running
		
		process_t *temp = dequeue();
		if(temp == NULL){
				// there was nothing to run in queue
				return_val = NULL;
		}else{
				// there was something in the queue
				return_val = temp->cursp;
		}
		
		if (current_process != NULL){
				// and our process is terminated -> free up memory
				process_stack_free(current_process->sp, current_process->size);
				free(current_process);
		}
		
		// current process get the value drom dequeue
		current_process = temp;
		
		
	}else	{
		// When cursp not null -> current program isnt done executing
		
		current_process->cursp = cursp;  // update curr_process ->cursp
		enqueue(current_process);
		current_process = dequeue();
		return_val = current_process->cursp; // update the return val to new cursp
	}
	
	// return the new cursp
	return return_val;
};

