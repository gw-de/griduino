#include <avr/io.h>
#include <avr/interrupt.h>


//defining variables
//
//array for every possible output
unsigned char ziffern[2][11] = {{0x02, 0x9E, 0x24, 0x0C, 0x98, 0x48, 0x40, 0x1E, 0x00, 0x08, 0xFF}, {0x03, 0x9F, 0x25, 0x0D, 0x99, 0x49, 0x41, 0x1F, 0x01, 0x09, 0xFF}};
unsigned char disp1 = 0x00;           //showing all at startup
unsigned char disp2 = 0x01;           //showing all at startup
unsigned char blueLED = 1;            //0 = LOW; 1 = HIGH, for values under 50Hz, ON AT STARTUP
unsigned char redLED = 1;             //0 = LOW; 1 = HIGH, for over 100mHz difference, ON AT STARTUP
unsigned char seconds = 5;            //timeframe for measuring
unsigned char mux = 0;                //variable for toggling displays
unsigned int  edges_counter;          //counter for rising edges
unsigned long counter;                //a global variable used as counter/timer
float         temp;                   //variable for calculation


//function to display numbers
void numberToDisplay(int x){
  //check if x is over 100
  if (x >= 100){
    redLED = 1;   //set red LED
    x -= 100;     //cut the 3rd digit (100) off
  }
  else{
    redLED = 0;   //if not over 100 reset red LED to off
  }


  //number for display 1
  //for loop counting down from 9 to 0
  for(int i = 9; i >= 0; i--){
      //true if x is less than i*10+10 (e.g. 9*10+10=100 for i=9) and greater/equals than i*10 (e.g. 9*10=90)
      if(x < i*10+10 && x >= i*10){
          //if true save digit with value of i to display 1
          disp1 = ziffern[0][i];
          //cut 2nd digit off
          x -= i*10;
          //correct digit found, then break the loop
          break;
      }
  }

  //number for display 2
  //check if rest value equals 0
  if(x==0){
      disp2 = ziffern[1][0];
  }
  else{
      //for loop counting down from 9 to 1
      for(int i = 9; i > 0; i--) {
          //if x equals i the modulo will give 0
          if(x % i == 0){
              //assign the value of i to display 2
              disp2 = ziffern[1][i];
              //correct digit found, then break the loop
              break;
          }
      }
  }
}


//function for multiplexer
void Multiplexer (void){
  //at mux = 0 and if blueLED is set turn on blue LED, skipped when blueLED is off
  if(mux == 0 && blueLED == 1){
      PORTA = 0xAC;
      PORTA |= 0x80;
      PORTA &= ~0x10;
  }
  //at mux = 1 show display 1
  else if (mux == 1){
      PORTA = disp1;
  }
  //at mux = 2 and if redLED is set turn on red LED, skipped when redLED is off
  else if(mux == 2 && redLED == 1){
      PORTA = 0x28;
      PORTA |= 0x20;
      PORTA &= ~0x04;
  }
  //else (so if mux = 4) show display 2
  else{
      PORTA = disp2;
  }

  //multiplexer counting up and keeping from 0 to 3
  mux++;
  mux%=4;
}


//function for calculation and setting blue LED
int calculateMeasurement (void){
  //calculating real frequency, if counter is over ideal the resulting frequency is less than 50
  temp = 50.0 * 1600.0 * (float)seconds / (float) counter;

  //if less than ideal 50Hz set blueLED and calculate difference up to 50
  if (temp < 50.0){
    blueLED = 1;
    temp = 50.0 - temp;
  }
  //else reset blueLED and calculate difference from 50
  else{
    blueLED = 0;
    temp = temp - 50.0;
  }

  //convert to mHz
  temp *= 1000.0;

  //reset edge counter and time counter
  edges_counter = 0;
  counter = 0;

  //cast to int and return to showing
  return (int)temp;
}


//is called every time an edge is detected on the input
ISR(INT0_vect){
  //count edges
  edges_counter++;

  //after x seconds calculate the output
  if (edges_counter == 100*seconds) {
     numberToDisplay(calculateMeasurement());

     //reset the calculation value
     temp = 0.0;
  }
}


//function that is called after the defined timer frequency
ISR(TIM0_OVF_vect){
   counter++;

   //call the multiplexer
   Multiplexer();
}


//starts edge detection
void start_edge_detection(void){
    DDRB&=~0x04;    //PB2 is configures for input for counting the edges
    MCUCR|=0x03;    //rising edge detection for INT0
    GIMSK|=0x40;    //enable interrupt/automatic call of ISR(INT0_vect)
    sei();      //enable all interrupts globally
}


//these macros can be used as parameter for the following function
#define CALL_FREQUENCY_12800HZ 0x01
#define CALL_FREQUENCY_1600HZ 0x02
#define CALL_FREQUENCY_200HZ 0x03
#define CALL_FREQUENCY_50HZ 0x04
#define CALL_FREQUENCY_12dot5HZ 0x05


//configures the timer call frequency
void configure_timer0(unsigned char freq){
    TCCR0A=0x00;    //normal mode
    TCCR0B=freq;    //prescaler frequency
    TIMSK0|=0x01;   //enable interrupt/automatic call of ISR(TIM0_OVF_vect)
    sei();      //enable all interrupts globally
}


int main(void){
    configure_timer0(CALL_FREQUENCY_1600HZ);   //defines the frequency the timer is called

    start_edge_detection();         //start edge detection

    DDRA=0xFF;              //all port pins of port A are configured for output
    PORTA=0xFF;             //all port pins of port A are outputting logic 1

    while(1){               //do nothing for now and let the ISR functions do their job

    }
    /*Make sure your code never reaches this position as it may leave the main
    function. On a µC this might result in undefined behaviour. Never leave main by
    e.g. keeping the empty while(1) in your code. */
}
