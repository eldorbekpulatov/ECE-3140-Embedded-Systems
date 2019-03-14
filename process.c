#include "3140_concur.h" 
#include <stdlib.h> 
#include <fsl_device_registers.h>


struct process_state {
		unsigned int *sp; 
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


// initial state of the processor
process_t * current_process = NULL;
process_t * process_queue = NULL;

/**  Removes and returns the process at the head of the queue **/
process_t* dequeue(){
	process_t *head = NULL;
	if (process_queue == NULL) {
     head = process_queue;
  }else {
		head->sp = process_queue->sp;
		head->next = NULL; // clear the ptr to next queue element
		process_queue = process_queue->next;	
	}; 
	return head;
};	


/* Create a new process. Return -1 if creation failed */
int process_create (void (*f)(void), int n){
	// disable the interrupt
	PIT->CHANNEL[0].TCTRL |=(0<<1);
	
	// allocate space in heap using provided function
	unsigned int *sp = process_stack_init((*f), n);
	
	if(sp == NULL){
		return -1;
	}else{
		// create a blank new process
		process_t *new_process =(process_t*)malloc(sizeof(process_t)); 
		// initialize the new_process -> sp
		new_process->sp = sp;   
		new_process->next = NULL; 
		// add the process to process_queue
		enqueue(new_process);  
		return 0;
	};
};

/* Starts up the concurrent execution */
void process_start (void){
	//Timer initialization
	NVIC_EnableIRQ(PIT0_IRQn); /* enable PIT0 Interrupts */
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK; // Enable clock to PIT module
	PIT->MCR &= 0x0;  // enable PIT MCR
	PIT->CHANNEL[0].LDVAL = 0xfff; // Set load value to PIT 
	
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
unsigned int * process_select (unsigned int *cursp){
	
	if(cursp != NULL){ // if the current process has NOT terminated, enqueue
		current_process->sp = cursp;  // update the current process->sp	
		current_process->next = NULL;
		enqueue(current_process);  // add to the back of the list
	}else{
		process_t * tmp = current_process; 
		process_stack_free(tmp->sp, sizeof(tmp));
	};
		
	current_process = dequeue(); //  update current process by deque()
	
	if(current_process == NULL){
		return NULL; 	// if there was no process in queue, return null
	}else{
		return current_process->sp; // otherwise, return current_process->sp
	};	
};
