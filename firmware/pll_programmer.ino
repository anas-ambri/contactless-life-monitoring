#include "TimerOne.h"

int ADF_CLK = 2;
int ADF_LE = 4;
int ADF_CE = 7;
int ADF_DATA = 8;
int ADF_MUXOUT = 12;

extern TimerOne Timer1;

#define DEBUG 1
bool test_freq = false;
bool muxout = false;

unsigned long R7, R6_1, R6_2, R5_1, R5_2, R4, R3, R2, R1, R0, FLAG;

const int MAX_BITS = 32;
#ifdef DEBUG
const int NUM_REGS = 12;
#else
const int NUM_REGS = 10;
#endif
int counterBit = MAX_BITS -1; //31 because 0-indexed
int counter = 0;
unsigned long regs[NUM_REGS]; 

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(ADF_CLK, OUTPUT);
  pinMode(ADF_LE, OUTPUT);
  pinMode(ADF_CE, OUTPUT);
  pinMode(ADF_DATA, OUTPUT);
  pinMode(ADF_MUXOUT, INPUT);

  Timer1.initialize(1000); //1000us, 1KHz frequency
  Timer1.attachInterrupt(callback);

  R7 = 7; //00000000000000000000000000000111
  R6_1 = 200006; //00000000000000110000110101000110;
  R6_2 = 8588614;//00000000100000110000110101000110;

  if (test_freq) {
    R5_1 = 100727725; //00000110000000001111101110101101
    R5_2 = 109116333; //00000110100000001111101110101101;
  } else {
    R5_1 = 134282157; //00001000000000001111101110101101;
    R5_2 = 142670765; //00001000100000001111101110101101;    
  }

  if (muxout) {
    R4 = 25559172; //00000001100001100000000010000100;
  } else {
    R4 = 26738820; //00000001100110000000000010000100;
  }

  R3 = 32867; //00000000000000001000000001100011;
  R2 = 255885330; //00001111010000001000000000010010;
  R1 = 1;//00000000000000000000000000000001;

  if (muxout) {
    R0 = 4164501504; //11111000001110010100000000000000;
  } else {
    R0 = 2151235584; //10000000001110010100000000000000;
  }
  FLAG = 4294967295;
  
  regs[0] = R7;
  regs[1] = R6_1;
  regs[2] = R6_2;
  regs[3] = R5_1;
  regs[4] = R5_2;
  regs[5] = R4;
  regs[6] = R3;
  regs[7] = R2;
  regs[8] = R1;
  regs[9] = R0;
  #ifdef DEBUG
  regs[10] = FLAG;
  regs[11] = FLAG;
  #endif

  digitalWrite(ADF_CE, HIGH);
}

void callback() {
  //Get the state of the CLK
  //(whether it is up or down)
  bool clk_Pin = digitalRead(ADF_CLK);

  //Toggle the clock
  digitalWrite(ADF_CLK, clk_Pin ^ 1);

  //If the rising edge of the clock
  if(clk_Pin) {
    
    if (counter < NUM_REGS) {

      if(counterBit == MAX_BITS -1) {
        digitalWrite(ADF_LE, LOW);
      }
  
      //Write bit at position counterBit in the register
      
      digitalWrite(ADF_DATA, (regs[counter] >> (counterBit)) & 1);

      //Move to the next bit
      counterBit--;

      //If we got to the last bit on the register
      if (counterBit < 0) {
        //Reset the position of the bit counter
        counterBit = MAX_BITS -1;
        //Move to the next register
        counter++;
      }
    } else {
      //Start all over again
      counter = 0;
    }
  } else {
      //If the next one is the last bit of the register,
      //we need to set LE to high
      if (counterBit == -1) {
        digitalWrite(ADF_LE, HIGH);
      }
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
