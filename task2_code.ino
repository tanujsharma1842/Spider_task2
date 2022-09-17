#include<avr/io.h>
#include<util/delay.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BRC ((F_CPU / 16 / BAUD_RATE)-1)//for baud rate prescale internal clock
#define SCL_CLK 100000

int i=0;
void initialize()
{
  TWSR = 0x00;//prescaler
  TWBR = 0x47;//serial clock frequency
  TWCR =(1<<TWEN) ;//enabling the TWI
}

void start()
{
TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);//setting the control register for start condition
while ((TWCR & (1 << TWINT)) == 0);// waiting for the acknowledgement or to check wether communication is still going on
}

void write(unsigned char data)
{
  TWDR = data ;//transfeering the data to TWDR register
  TWCR = (1 << TWINT) | (1<<TWEN);
  while((TWCR & (1 << TWINT)) == 0);
}

unsigned char read(unsigned char Last_char){
if (Last_char == 0) //if want to read more than 1 byte
TWCR = (1<< TWINT) | (1<<TWEN) | (1<<TWEA) ;
else //if want to read only one byte
TWCR = (1<< TWINT) | (1<<TWEN) ;
while ( (TWCR & (1 <<TWINT) ) == 0) ;
return TWDR ;
}
unsigned char print_alertmsg[] = "DANGER!!!";
void Alert_msg()
{
   while (print_alertmsg[i]!=0)
        {
            while (!( UCSR0A & (1<<UDRE0)));//waiting for empty buffer
            UDR0 = print_alertmsg[i];//sending data to buffer
            i++;
        }
        while (!( UCSR0A & (1<<UDRE0)));
        UDR0 = '\n';//for new linw
        while (!( UCSR0A & (1<<UDRE0)));
        UDR0 = '\r';//cursor to beginning of line
        i=0; 
}

void stop()
{
TWCR = (1<< TWINT) | (1<<TWEN) | (1<<TWSTO) ;
}
int main (void){
  
  unsigned char temp = 0 ;// temp variable stores temperature read from tc74
  DDRB = (1<<PB5);//relay is set as output
  UBRR0H = (BRC>>8);//contains baud rate
  UBRR0L = (BRC);
  UCSR0B = (1<<TXEN0);//enabling transmitter
  UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);//for defining the size of data for 8bits both bits should be set 1
  while(1){
  initialize() ; //initialize TWI for master mode
  start(); //transmit START condition
  write(0b10011010) ; //transmit SLA + w(1)
  write(0b00000000) ; //for reading temp from tc74
  start();
  write(0b10011011) ;//trabsmit SLA + R(1)
  temp=read (1) ; //to read one byte
  stop();
  if(temp>=30)
  {
    PORTB |= (1<<PB5);//to close relay switch and turn fan on
     Alert_msg();
    }
  
  else
  {
    PORTB &= (0<<PB5);// to open relay circuit and make fan off
    }

  
  
  }

return 0 ;

}
