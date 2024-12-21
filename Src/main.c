#include <stdint.h>
#include <structs.h>
#include <string.h>
#define GPIOB ((GPIOType*) 0x42020400)
#define GPIOG ((GPIOType*)  0x42021800)

#define RCC_AHB2ENR *((volatile uint32_t*) (0x40021000 + 0x04C))
#define RCC_APB2ENR *((volatile uint32_t*) (0x40021000 + 0x060))
#define RCC_APB1ENR1 *((volatile uint32_t*) (0x40021000 + 0x058))
#define RCC_APB1ENR2 *((volatile uint32_t*) (0x40021000 + 0x05C))
#define RCC_CCIPR1 *((volatile uint32_t*) (0x40021000 + 0x088))

#define PWR_CR2 *((volatile uint32_t*) (0x40007004))

#define ISER2 *((volatile uint32_t*) (0xE000E108))
#define TIM17 ((TIM17Type*)  0x40014800)
#define LPUART1 ((LPUARTType*) 0x40008000)


int transmittingString = 0;
int stringPos = 0;
char currentStr[256];
void init_gpio(void){
	RCC_APB1ENR1 |= (1 << 28); // Enable PWR


	PWR_CR2 |= (1 << 9); // Enable GPIOG Power

	RCC_AHB2ENR |= (1 << 1); // GPIOB
	RCC_AHB2ENR |= (1 << 6); // GPIOG


	GPIOG->MODER &= ~((0xF << (7*2)));
	GPIOG->MODER |= ((10U << (7*2))); // AF mode for PG7-PG8

	GPIOB->MODER &= ~(3 << (9 *2));
	GPIOB->MODER |= (2 << (9 *2)); // AF mode for PB9

	GPIOG->AFRL &= ~(0xF << (7*4));
	GPIOG->AFRH &= ~(0xF); // Clear bits for PG AFs

	GPIOG->AFRL |= (8U << (7*4)); // LPUART TX alternate function
	GPIOG->AFRH |= (8U); // LPUART RX alternate function


	GPIOB->AFRH &= (0xF << 4); // Clear bits for PB AFs

	GPIOB->AFRH |= (14U << 4*1); // TIM17_CH1 Alternate Function
}

void init_oc(void){
	RCC_APB2ENR |= (1 << 18); // Enable Timer 17

	TIM17->PSC = 399; // 1 clock cycle corresponds to 0.1 ms
	TIM17->ARR = 200; // 20 ms Period

	TIM17->BDTR |= (1 << 15); // MOE

	TIM17->CCMR1 |= (3 << 5); // PWM1

	TIM17->CCER |= 1; // Enable output

	TIM17->CCR1 = 10; // 1 ms duty cycle for closed position.

	TIM17->CR1 |= 1; // Enable Timer

}


void init_lpuart(void){



	RCC_APB1ENR2 |= (1); // Enable LPUART1


	RCC_CCIPR1 |= (1U << 10); // SYSCLOCK As LPUART1 clock

	LPUART1->BRR = 8888; // 115200 Baud Rate

	LPUART1->CR1 |= (3 << 2); //Tx and Rx Enabled

	LPUART1->CR1 |= (1 << 29); //FIFO

	LPUART1->CR1 |= (1 << 5); //FIFO Not Empty Interrupt



	ISER2 |= (1 << 2); //NVIC 66

	LPUART1->CR1 |= 1; //Enable LPUART
}



void uart_transmit_string(char* str){
	if (transmittingString){ // Exit the function if the board is already transmitting a string. Can busy wait here for longer transmissions.
		return;
	}
	if (*(str)){ // Check if the length is greater than 0.
		strcpy(currentStr, str); // Copy the string to the global variable.
		transmittingString = 1; // Transmission began.
		LPUART1->TDR = *currentStr; // Transmit the first character.
		stringPos = 1; // Move to the second character
		LPUART1->CR1 |= (1 << 6); //Enable Transmission Complete Interrupt
	}


}


void LPUART1_IRQHandler(void){
	if ((LPUART1->ISR & (1 << 5))){
		char byte_read = LPUART1->RDR; // Read the byte from the recieved data register.

		if (byte_read == 'O'){
			if (TIM17->CCR1 == 20){ // The duty cycle is already 2ms. This means the door is already open.
				uart_transmit_string("The door is already open.\r\n");
			}
			else{
				TIM17->CCR1 = 20; //Set the duty cycle to 2ms (Servo on position)
				uart_transmit_string("The door has opened.\r\n");
			}
		}
		else if (byte_read == 'C'){
			if (TIM17->CCR1 == 10){ // The duty cycle is already 1ms. This means the door is already closed.
				uart_transmit_string("The door is already closed.\r\n");
			}
			else{
				TIM17->CCR1 = 10; //Set the duty cycle to 1ms (Servo off position)
				uart_transmit_string("The door has closed.\r\n");
			}

		}
	}
	if ((LPUART1->ISR & (1 << 6))){ //Transmission complete interrupt.
		LPUART1->ICR |= (1 << 6); // Clear the isr flag for transmission complete.
		if (*(currentStr + stringPos)){ //Check if the stringPos is within the string bounds.
			LPUART1->TDR = *(currentStr + stringPos); // Transmit the current character.
			stringPos++; // Increase the stringPos counter variable.
		}
		else{ //Transmission completed, set variables to initial position.
			stringPos = 0;
			transmittingString = 0;
			LPUART1->CR1 &= ~(1 << 6); //Disable the Transmission Complete Interrupt
		}
	}
}

int main(void)
{
   init_gpio(); // Init GPIO
   init_oc(); // Init Tim17 OC
   init_lpuart(); // Init LPUART1
   uart_transmit_string("Door is currently closed.\r\n"); // Since duty cycle is 1ms at first, the door is closed.
   while(1){
	   __asm volatile(
		"wfi"
	   );
   }
}
