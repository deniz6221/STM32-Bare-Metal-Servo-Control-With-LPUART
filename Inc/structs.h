
#ifndef STRUCTS_H_
#define STRUCTS_H_


typedef struct{
	volatile uint32_t CR1, CR2;
	uint32_t reserved1;
	volatile uint32_t DIER, SR, EGR, CCMR1;
	uint32_t reserved2;
	volatile uint32_t CCER, CNT, PSC, ARR, RCR, CCR1;
	uint32_t reserved3[3];
	volatile uint32_t BDTR, DCR, DMAR, OR1;
	uint32_t reserved4[3];
	volatile uint32_t OR2;
} TIM17Type;

typedef struct{
	volatile uint32_t CR1; //0
	volatile uint32_t CR2; //4
	volatile uint32_t CR3; //8
	volatile uint32_t BRR; //C
	volatile uint32_t GTPR; //10
	volatile uint32_t RTOR; //14
	volatile uint32_t RQR; //18
	volatile uint32_t ISR; //1C
	volatile uint32_t ICR; //20
	volatile uint32_t RDR; //24
	volatile uint32_t TDR; //28
	volatile uint32_t PRESC; //2C
} USARTType;

typedef struct{
	volatile uint32_t MODER;
	volatile uint32_t OTYPER;
	volatile uint32_t OSPEEDR;
	volatile uint32_t PUPDR;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t LCKR;
	volatile uint32_t AFRL;
	volatile uint32_t AFRH;
	volatile uint32_t BRR;
	uint32_t reserved;
	volatile uint32_t SECCFGR;
} GPIOType;


typedef struct{
volatile uint32_t CR1; //0
volatile uint32_t CR2; //4
volatile uint32_t CR3; //8
volatile uint32_t BRR; //C
uint32_t reserved1[2]; //10 14
volatile uint32_t RQR; //18
volatile uint32_t ISR; //1C
volatile uint32_t ICR; //20
volatile uint32_t RDR; //24
volatile uint32_t TDR; //28
volatile uint32_t PRESC; //2C
} LPUARTType;


#endif /* STRUCTS_H_ */
