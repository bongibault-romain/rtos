#include "stm32f10x.h"

#define STACK_1_ADDRESS 0x20001000
#define STACK_2_ADDRESS 0x20003000

#define XPSR_DEFAULT 0x01000000
#define LR_DEFAULT 0xFFFFFFFD

uint32_t *stack1 = (uint32_t *) STACK_1_ADDRESS;
uint32_t *stack2 = (uint32_t *) STACK_2_ADDRESS;

char current_process = 0;
char setup = 0;

 void SysTick_Handler(void) {
	if (!setup){
		__set_PSP((uint32_t)stack1);
		setup = 1;
//		SCB->ICSR |= SCB_ICSR_PENDSVSET ;
		return;
	}
	
	if(current_process){
		stack2 = (uint32_t *) __get_PSP();
		__set_PSP((uint32_t)stack1);
	} else {
		stack1 = (uint32_t *) __get_PSP();
		__set_PSP((uint32_t)stack2);
	}
	
	current_process = !current_process;
//	SCB->ICSR |= SCB_ICSR_PENDSVSET ;
	return;
}

//void PendSV_Handler(){
//	
//}

void dummy1(void) {
	while(1){
		int a = 2;
		a++;
	}
}

void dummy2(void) {
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
	SysTick->LOAD |= 72000;
	
//	NVIC_SetPriority(SysTick_IRQn, 0x0);
//	NVIC_SetPriority(PendSV_IRQn, 0x3);
	initialize_stacks();
	
	uint32_t ctrl = __get_CONTROL();
	ctrl |= 3;
	__set_CONTROL(ctrl);
	
	while (1)
	{
	}
}
