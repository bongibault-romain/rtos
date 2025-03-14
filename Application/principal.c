#include "stm32f10x.h"

char global = 1;
char setup = 0;

uint32_t *stack1 = (uint32_t *) 0x20001000;
uint32_t *stack2 = (uint32_t *) 0x20003500;

char current_process = 0;

void SysTick_Handler(void) {
	if (!setup){
		__set_PSP((uint32_t)stack1);
		setup = 1;
//		SCB->ICSR |= SCB_ICSR_PENDSVSET ;
		return;
	}
	
	global = !global;
	
	if(global){
		stack2 = (uint32_t *) __get_PSP();
		__set_PSP((uint32_t)stack1);
	} else {
		stack1 = (uint32_t *) __get_PSP();
		__set_PSP((uint32_t)stack2);
	}
//	SCB->ICSR |= SCB_ICSR_PENDSVSET ;
	return;
}

//void PendSV_Handler(){
//	
//}

void dummy1(void){
	while(1){
	}
}

void dummy2(void){
	while(1){
	}
}

int main ( void )
{
	SysTick->CTRL |= SysTick_CTRL_ENABLE + SysTick_CTRL_TICKINT + SysTick_CTRL_CLKSOURCE;
	SysTick->LOAD |= 72000;
	
//	NVIC_SetPriority(SysTick_IRQn, 0x0);
//	NVIC_SetPriority(PendSV_IRQn, 0x3);
	
	for (int i = 0; i<6; i++) {
		stack1[i] = 0;
		stack2[i] = 0;
	}
	
	stack1[7] = 0x01000000;
	stack2[7] = 0x01000000;
	
	stack1[6] = (uint32_t) dummy1;
	stack2[6] = (uint32_t) dummy2;
	
	uint32_t ctrl = __get_CONTROL();
	ctrl |= 0b11;
	__set_CONTROL(ctrl);
	
	while (1)
	{
	}
}
