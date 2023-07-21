#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdarg.h>

/// include the liquid crystal library code:
#include <LiquidCrystal.h>
#define B1 9
#define B0 8
#define D4 4
#define D5 5
#define D7 7
#define D3 3

// All definitions used for variables or ease of use
#define SET_BIT(reg, pin)			(reg) |= (1 << (pin))
#define CLEAR_BIT(reg, pin)			(reg) &= ~(1 << (pin))
#define WRITE_BIT(reg, pin, value)	(reg) = (((reg) & ~(1 << (pin))) | ((value) << (pin)))
#define BIT_VALUE(reg, pin)			(((reg) >> (pin)) & 1)
#define BIT_IS_SET(reg, pin)		(BIT_VALUE((reg),(pin))==1)
#define FREQ (16000000UL)
#define PRESCALE (8.0) 
#define BAUD 9600
#define MYUBRR FREQ/16/BAUD-1

// initialize the library by associating any needed LCD interface pin
// with the Arduino pin number it is connected to
uint8_t rs = B1, en = B0;
 
LiquidCrystal lcd(rs, en, D4, D5, D3, D7);
 


// Global Variables
bool mode; // Variable to represent which button is currently on / true
volatile uint32_t cycle_count = 0;
int hour = 12; // Initial time 12:59 PM
int minute=59;
double sec;
int endbit=1; //PM
int sensorInput;
int temp;

// Time Set Buttons
int button1;
int button2;
int channel = 1;
// Pins Buttons
int pinTemp = 1;



// Initialise Uart
void uart_init(unsigned int ubrr) {
	
	UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)(ubrr);
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (3 << UCSZ00);
	
}
//Get character from serial and use character function function

void uart_getchar(void) {
  

 	
    char getchar = char(UDR0);
  if (mode == false){
  	if (getchar == 'b'){
    	SET_BIT(PORTC, 4);
      lcd.setCursor(0,1);
      lcd.print("             ");
      lcd.setCursor(0,1);
      lcd.print("Blue Added");
  	
  }
  if ( getchar == 'g'){
    	SET_BIT(PORTC, 5);
    lcd.setCursor(0,1);
    lcd.print("             ");
    lcd.setCursor(0,1);
      lcd.print("Green Added");
  	
  }
  if ( getchar == 'r'){
    CLEAR_BIT(PORTC, 5);
    CLEAR_BIT(PORTC, 4);
    lcd.print("             ");
    lcd.setCursor(0,1);
      lcd.print("Reset Colours");
  	
  }
 }

}
void uart_putchar(unsigned char data) {
	
  
    while (!( UCSR0A & (1<<UDRE0)));
          
    UDR0 = data;
        	

}
//Function to send a string using putchar
void uart_putstring(unsigned char* s)
{
    while(*s > 0) uart_putchar(*s++);
}
//Function to initialise program / what to send at the start
void UseSerial(void) {
  
   //Strings to send on boot
    unsigned char sent_char[] = "Press Button 1 for Temperature";
  	unsigned char sent_char2[] = "Press Button 2 to change the colours of the lights";
  unsigned char sent_char3[] = "Please input 'r' to remove the added colours";
  unsigned char sent_char4[] = "Please input 'g' to add green";
  unsigned char sent_char5[] = "Please input 'b' to add blue";
  unsigned char sent_char6[] = "Input nothing to continue pulsing red";
   //send serial data
	uart_putstring(sent_char);
  	uart_putchar('\n');
  	uart_putstring(sent_char2);
  	uart_putchar('\n');
  uart_putstring(sent_char3);
  	uart_putchar('\n');
  uart_putstring(sent_char4);
  	uart_putchar('\n');
  uart_putstring(sent_char5);
  	uart_putchar('\n');
  uart_putstring(sent_char6);
  	uart_putchar('\n');
  	
  
}

// Sets up timer 1 for counting the clock
void setup_timer()
{
	TCCR1A = 0;
	TCCR1B = 2; 
    TIMSK1 = 1; 
  sei();
}
ISR(TIMER1_OVF_vect) {
cycle_count ++;
}  
int current_time(void){
     return (cycle_count * 65536.0 + TCNT1 ) * PRESCALE  / FREQ;
}

//Setsup Anaalogue channel reading

void setup_adc() {
    ADMUX  = (1<<REFS0);
    ADCSRA = (1 << ADEN) | (0 << ADPS2) | (1 << ADPS1) | (0 << ADPS0);
}
//Reads the analogue channel
uint16_t adc( uint8_t channel ) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return (ADC);
}
// Sets up the PWM using timer 0 & Duty cycle will be set by the light in main
void pwm_setup() {
    DDRD |= (1 << PD6);
    // PD6 is now an output
 
    TCCR0A |= (1 << COM0A1);
    // set none-inverting mode
 
    // TinkerCAD Errata: timer clocking must be enabled before WGM
    // set prescaler to 8 and starts PWM
    TCCR0B = (1 << CS01);
 
    TCCR0A |= (1 << WGM01) | (1 << WGM00);
    // set fast PWM Mode
}



void toggle_mode() {
	// If left button is pressed, Turn on YELLOW LED and show temperature.
	if ( ( (PINB >> PB2) & 1) ) {
      
		SET_BIT(PORTB, 4); // turn on green led
        CLEAR_BIT(PORTB,5); // turn off red led
      mode = false;  //Which state the LCD should be in
	}

	// Otherwise, if right button is pressed, turn on WHITE LED, and allow for colours to be added to the RGB LED.
	if ( BIT_IS_SET(PINB, 3) ) {
		SET_BIT(PORTB, 5);    // turn on green led
        CLEAR_BIT(PORTB,4); // turn off red led
       mode = true; //Which state the LCD should be in
  		
	}
  
    
}
//What the buttom line of the LCD should print at any given moment
void what_to_print() 
{
  //If LEFT button was pressed show temperature
  if (mode == true){
    sensorInput = adc(pinTemp); // Use ADC to read the voltage from temperature sensor
    lcd.setCursor(0,1);
    
    lcd.print("Temp is:");
  	float voltage = sensorInput * (5000/1024.0);
    temp = (voltage - 500) / 10;
    char text[20];
 	sprintf(text, "%d", temp);
    // Equation to dictate temperature from the voltage reading
    lcd.setCursor(8,1);
  	lcd.print(text);
    lcd.setCursor(10, 1);
    lcd.print(" ");
    lcd.setCursor(11, 1);
    lcd.print("C");
    lcd.setCursor(12,1);
    lcd.write(0b10110010); //The dot above the celsius sign
  }
  //If Right button allow serial input
  else{
    
    lcd.setCursor(0,1);
    lcd.print("Input colours");
  }
}

//What the TOP line of the LCD should print at any given moment
void displayfunction(void)
{
 lcd.setCursor(0,0);
 lcd.print("Time ");
 lcd.setCursor(5,0);
 if(hour<10)lcd.print("0");// always 2 digits
 char text[20];
 sprintf(text, "%d", hour);
 lcd.print(text);
 lcd.setCursor(7,0);
 lcd.print(":");
 lcd.setCursor(8,0);
 if(minute<10)lcd.print("0");
 sprintf(text, "%d", minute);
 lcd.print(text);
 lcd.setCursor(10,0);
 lcd.print(":");
 lcd.setCursor(11,0);
 int time_now = current_time();
 if(time_now < 10)lcd.print("0");
 sprintf(text, "%d", time_now);
 lcd.print(text); 

 if(endbit==0) lcd.print(" AM");
 if(endbit==1) lcd.print(" PM");
 button1=adc(2);// Read Buttons using ADC
 button2=adc(3);


 if(button1 > 1){
   hour=hour+1;  
  }
 if(button2 > 1){
  sec = 0;
  minute = minute + 1; 
  }

// manage overflow from seconds, minutes, hours am/pm 
 if(time_now == 60){
  time_now = 0;
  cycle_count = 0;
  minute = minute + 1;
 }
 if(minute==60)
 {
  minute = 0;
  hour = hour + 1;
 }
 if(hour == 13)
 {
  hour = 1;
  endbit=endbit+1;
  if(endbit==2)endbit=0;
 }
}
//Initial Setup 
void setup()
{
  lcd.begin(16,2); //START LCD
  uart_init(MYUBRR); //Uart Initialiser
	SET_BIT(DDRB, 5); // LED's
    SET_BIT(DDRB, 4); 
  
	CLEAR_BIT(PORTC, 4);
  	CLEAR_BIT(PORTC, 5);
	// Input from the left and right buttons
	CLEAR_BIT(DDRB, 3); // right (switch 2)
	CLEAR_BIT(DDRB, 2); // left (switch 1)
  lcd.setCursor(0,0);
  
}



int main(void)
{ 
  //All Initial setups
  setup();
  UseSerial();
  setup_timer();
  setup_adc();
  pwm_setup();
  //Initial mode is RGB select mode, therefore white light should be on
  SET_BIT(PORTB, 4);
  
  
  int number = 0;  
  for (;;) {   
    //Functions that need to be repeated every 1 second (Delay is within the switch cases)
    
  displayfunction();
    toggle_mode();
  what_to_print();
    uart_getchar();
    int i;
    
  //Use PWM to create flashing 'ambience' light
  //Changes the DUTY CYCLE in a for loop that approaches 0 then back to 250 DUTYCYCLE
    /*    
 switch(number){
     case 0:
     for(i=250; i>0; (i=i-25))
         {
       
         OCR0A = i;
         _delay_ms(50);
       		
         } 
    number = 1;
    	case 1:
     for( int i=0; i<250; (i=i+25))
         {
       
         OCR0A = i;
         _delay_ms(50);
       		
         } 
   number = 0;
 		}
        */
    OCR0A = 127.5;
	}
    
  
}