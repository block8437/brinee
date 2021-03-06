#include <stdint.h>
#include <stdbool.h>
#include <kernel/system.h>

typedef enum {
	PROCESS_STATE_ALIVE,
	PROCESS_STATE_ZOMBIE,
	PROCESS_STATE_DEAD
} processstate_t;

typedef struct process_t {
	struct process_t* prev;
	struct process_t* next;
	char* name;

	uint32_t pid;
	uint32_t esp;
	uint32_t cr3;
	processstate_t state;
	uint32_t stacktop;

	void (*notify)(int);

	bool started;
} process_t;

process_t* thread_new(char* name, void(*addr)());
void threads_install();
int thread_add(process_t* proc);
void threads_killcurrent();


void schedule(struct regs *r);
