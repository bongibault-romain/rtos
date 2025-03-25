#include "stm32f10x.h"

#define STACK_1_ADDRESS 0x20001000
#define STACK_2_ADDRESS 0x20003000

#define XPSR_DEFAULT 0x01000000
#define LR_DEFAULT 0xFFFFFFFD

uint32_t *stack1 = (uint32_t *) STACK_1_ADDRESS;
uint32_t *stack2 = (uint32_t *) STACK_2_ADDRESS;

uint32_t schedule[40] = {0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0};
uint32_t time = 0;
uint32_t current_process = 0;

#include <stdio.h>

void SysTick_Handler(void) {
	 time = (time + 1) % 40;
	current_process = schedule[time];
		 
	 if (!time || schedule[time-1] == schedule[time]) {
		 return;
	 }
	
	if(!schedule[time]){
		stack2 = (uint32_t *) __get_PSP();
		__set_PSP((uint32_t)stack1);
	} else {
		stack1 = (uint32_t *) __get_PSP();
		__set_PSP((uint32_t)stack2);
	}
	
	SCB->ICSR |= SCB_ICSR_PENDSVSET ;
	return;
}

void PendSV_Handler(){
	uint32_t *previous_stack = current_process ? stack1 : stack2;
	uint32_t *next_stack = current_process ? stack2 : stack1;
	
  __ASM volatile ("MOV %0, r4"  : "=r" (previous_stack[8]));
	__ASM volatile ("MOV r4, %0" : : "r" (next_stack[8]) : );
}

void dummy1(void) {
		int a = 0;
	while(1){
		a++;
		__ASM volatile ("MOV r4, %0" : : "r" (a) : );
	}
}

void dummy2(void) {
	__ASM volatile ("MOV r4, #15" );
	while(1){
		int a = 2;
		a++;
	}
}

void initialize_stacks() {
	for (int i = 0; i<6; i++) {
		stack1[i] = 0;
		stack2[i] = 0;
	}
	
	stack1[7] = XPSR_DEFAULT;
	stack2[7] = XPSR_DEFAULT;
	stack1[6] = (uint32_t) dummy1;
	stack2[6] = (uint32_t) dummy2;
	stack1[5] = LR_DEFAULT;
	stack2[5] = LR_DEFAULT;
}
 
int main ( void )
{
	SysTick->CTRL |= SysTick_CTRL_ENABLE + SysTick_CTRL_TICKINT + SysTick_CTRL_CLKSOURCE;
	SysTick->LOAD |= (72000*2);
	
	NVIC_SetPriority(SysTick_IRQn, 0x0);
	NVIC_SetPriority(PendSV_IRQn, 0x3);
	initialize_stacks();
	
	
	__set_PSP((uint32_t)stack1);
	
	uint32_t ctrl = __get_CONTROL();
	ctrl |= 3;
	__set_CONTROL(ctrl);
	
	dummy1();

	while (1)
	{
	}
}
