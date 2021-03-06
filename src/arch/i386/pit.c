#include <kernel/pit.h>
#include <kernel/system.h>
#include <kernel/threads.h>
#include <stdint.h>
#include <stdio.h>

static uint8_t tasking = 0;
static uint8_t task_was_on = 0;

void tasks_enable(uint8_t i) {
	if(!task_was_on)
		return;

	tasking = i;
}

void tasks_init() {
	task_was_on = 1;
	tasking = 1;
}


void pit_handler(struct regs *r) {
	if(tasking) {
		schedule(r);
	}
}


inline void __pit_send_cmd(uint8_t cmd) {
	outportb(PIT_REG_COMMAND, cmd);
}

inline void __pit_send_data(uint16_t data, uint8_t counter) {
	uint8_t	port = (counter==PIT_OCW_COUNTER_0) ? PIT_REG_COUNTER0 :
		((counter==PIT_OCW_COUNTER_1) ? PIT_REG_COUNTER1 : PIT_REG_COUNTER2);

	outportb (port, (uint8_t)data);
}

inline uint8_t __pit_read_data (uint16_t counter) {
	uint8_t	port = (counter==PIT_OCW_COUNTER_0) ? PIT_REG_COUNTER0 :
		((counter==PIT_OCW_COUNTER_1) ? PIT_REG_COUNTER1 : PIT_REG_COUNTER2);

	return inportb (port);
}

void pit_start_counter (uint32_t freq, uint8_t counter, uint8_t mode) {
	if (freq==0)
		return;

	uint16_t divisor = (uint16_t)( 1193181 / (uint16_t)freq);

	// send operational command words
	uint8_t ocw = 0;
	ocw = (ocw & ~PIT_OCW_MASK_MODE) | mode;
	ocw = (ocw & ~PIT_OCW_MASK_RL) | PIT_OCW_RL_DATA;
	ocw = (ocw & ~PIT_OCW_MASK_COUNTER) | counter;
	__pit_send_cmd (ocw);

	// set frequency rate
	__pit_send_data (divisor & 0xff, 0);
	__pit_send_data ((divisor >> 8) & 0xff, 0);
}

void pit_install() {
	irq_install_handler(0, pit_handler);
	pit_start_counter (200, PIT_OCW_COUNTER_0, PIT_OCW_MODE_SQUAREWAVEGEN);
}
