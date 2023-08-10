#include "TM4C123.h"                    // Device header
/*PIN OUT 
CLK = PD0
TX = PD3
FSS = PF3
*/
/**@brief sending A wiil put on the LED while B will switch off the LED
  * there is a delay to enable blinking of the LED

*/
void SPI_init (void);
void delay_Ms(int time );
void SPI_Write (unsigned char data);


int main ()
{
	unsigned char val1 = 'A' ;
	unsigned char val2 = 'B';
	
	SPI_init();
	while (1)
	{
		SPI_Write(val1);
		delay_Ms(100);
		SPI_Write(val2);
		delay_Ms(100);
		
	}
	
	
}

void SPI_init (void)
{
	
	SYSCTL->RCGCSSI |= 0x02; //Enable clock for SPI 1
	SYSCTL->RCGCGPIO |= 0x80 ;//Enable clock for PD for SPI 1
	SYSCTL->RCGCGPIO |= 0x20 ; //Enable clock for PF for slave select
	
	//initialize TX and CLK AFSEL
	GPIOD->AFSEL |= 0x09; //PD0,PD3C alternate function select
	GPIOD->PCTL &= ~0x0000F00F ;/* assign RD0 and RD3 pins to SPI1  */
	GPIOD->PCTL |= 0x00002002; /* assign RD0 and RD3 pins to SPI1  */
	
	//initialize slave select pin
 /* Initialize PF3 as a digital output as a slave select pin */
	
	GPIOF->DEN |= 0x08 ;
	GPIOF->DIR |= 0x08 ;
	GPIOF->DATA |= 0x08 ; ///* keep SS idle high */
	
	//    /* Select SPI1 as a Master, POL = 0, PHA = 0, clock = 4 MHz, 8 bit data */
 SSI1->CR1 = 0x00; // SSI master select , SSI disabled
 SSI1->CC |= 0x00 ; //system clock configuration 
 SSI1->CPSR = 4 ;/* Select prescaler value of 4 .i.e 16MHz/4 = 4MHz */
 SSI1->CR0 = 0x07 ; /* 4MHz SPI1 clock, SPI mode, 8 bit data */
 SSI1->CR1 |= 0x02; //enable SSI
 
 }
void SPI_Write (unsigned char data)
{
	GPIOF->DATA &= ~0x08;  /* Make PF3 Selection line (SS) low */
 while ((SSI1->SR & 2) == 0 );  /* wait untill Tx FIFO is not full */
  SSI1->DR = data ;              /* transmit byte over SSI1Tx line */
 while (SSI1->SR & 0x10 );       /* wait until transmit complete */
	GPIOF->DATA |= 0x08 ;          /* keep selection line (PF3) high in idle condition */



}
void delay_Ms(int time )
{
	int i;
	SYSCTL->RCGCTIMER |= 0x01 ; //enable clock for timer 0
	TIMER0->CTL &= ~0x01; //disable timer 
	TIMER0->CFG = 0x04 ; //16 Bit timer
	TIMER0->TAMR = 0x02 ; // periodic mode
	TIMER0->TAILR = 16-1 ;
	TIMER0->ICR = 0x01 ; //clear prior timers flag
	TIMER0->CTL |= 0x01 ; //enable timer
	
	for(i=0;i<time;i++)
	{
		while((TIMER0->RIS & 0x01)== 0);
		TIMER0->ICR = 0x01 ; //clear timeout flag
	}
}
