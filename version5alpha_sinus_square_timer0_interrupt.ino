#define CLOCKSPEED 16000000L

// Headers

#include "variables.h"
#include "IOPINS.h"
#include "global.h"

// Sinus Wave Data

const int sinus[] = {127, 134, 142, 150, 158, 166, 173, 181, 188, 195, 201, 207, 213, 219, 224, 229, 234, 238, 241, 245, 247, 250, 251, 252, 253, 254, 253, 252, 251, 250, 247, 245, 241, 238, 234, 229, 224, 219, 213, 207, 201, 195, 188, 181, 173, 166, 158, 150, 142, 134, 127, 119, 111, 103, 95, 87, 80, 72, 65, 58, 52, 46, 40, 34, 29, 24, 19, 15, 12, 8, 6, 3, 2, 1, 0, 0, 0, 1, 2, 3, 6, 8, 12, 15, 19, 24, 29, 34, 40, 46, 52, 58, 65, 72, 80, 87, 95, 103, 111, 119,};

// Wave variables

volatile int counter; // 100 step cycle counter
volatile int frequency;
volatile Wave wave;

void setup() { // Initialize

  Serial.begin(9600);

  Serial.println("Hello, world!");

  pinMode(13, OUTPUT);

  DDRC = DDRC | B11111100; // Set bit 0,1,2,3,5 to output

  frequency = 0;
  
  PORTC = PORTC | B10000000; // Set bit 0 to 1, indicating sinus wave
  wave = sinusWave; // Wave is set to sinus wave
  counter = 0; // Cycle counter is 0

  cli(); // Disable all interrupt functions

  TIMER4_init();

  sei(); // Enable all interrupts functions

  attachInterrupt(digitalPinToInterrupt(INTERRUPT_BTN_0), toggleWave, FALLING); // Attach the interrupts to their input and functions
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_BTN_1), toggleFrequency, FALLING);

}

void loop() {

}

ISR(TIMER4_COMPA_vect) { // Internal interrupt for timer 4

  play();

}

void play() {

  if(wave == sinusWave) PORTA = sinus[counter]; // Plays sinus wave based on cycle counter on port A

  if(wave == squareWave && counter < 49) digitalWrite(13, HIGH); else digitalWrite(13, LOW); // Plays square wave on the built in diode connected to pin 13 based on cycle counter

  counter++;

  if (counter > 99) { // If counter is 100, reset it to 0
    counter = 0;
  }

}

void toggleWave() {

  if (!debounce()) return; // Debounce

  cli();

  PORTC = PORTC & B00111111; // Sets bit 0,1 in port C to 0

  if (wave == sinusWave) {
    Serial.println("Sinus Wave.");
    wave = squareWave;
    PORTC = PORTC | B10000000; // Set bit 0 to 1
  }
  else {
    Serial.println("Square Wave.");
    wave = sinusWave;
    PORTC = PORTC | B01000000; // Set bit 1 to 1

  }

  sei();

}

void toggleFrequency() { // Function to toggle frequency

  if (!debounce()) return;

  cli();

  PORTC = PORTC & B11000011; // Sets bit 2,3,4,5 to 0 in port C

  switch (frequency) {
    case 0:
      PORTC = PORTC | B00100000; // Set bit 2 to 1
      TIMER4(0);
      break;
    case 1:
      PORTC = PORTC | B00010000; // Set bit 3 to 1
      TIMER4(1);
      break;
    case 2:
      PORTC = PORTC | B00001000; // Set bit 4 to 1
      TIMER4(2);
      break;
    case 3:
      PORTC = PORTC | B00000100; // Set bit 5 to 1
      TIMER4(3);
      break;
    default:
      break;
  }

  if (frequency < 3) frequency++;
  else frequency = 0;

  sei();
}

void TIMER4_init() { // Initialization of timer 4, 16-bit timer

  Serial.println("Initialize TIMER4.");

  TCCR4A = 0; // Timer Register
  TCCR4A |= (1 << WGM41);

  TIMER4(0);
  PORTC = PORTC | B00100000; // Indicate frequency 1

  TIMSK4 = 0;
  TIMSK4 |= (1 << OCIE4A);

  Serial.println("TIMER4 Initialized.");

}

void TIMER4(int option) {

  TCCR4B = 0; //Reset control register B

  //Case 0 100Hz: Prescaler = 1024L, Match Register = 156L
  //Case 1 1.000Hz: Prescaler = 64L, Match Register = 249L
  //Case 2 10.000Hz: Prescaler = 8L, Match Register = 199L
  //Case 3 100.000Hz: Prescaler = 1L, Match Register = 159L

  OCR4AL = 0; //Reset low match register
  OCR4AH = 0; //Reset high match register

  switch (option) {
    case 0: // 100Hz
      Serial.println("Setting Case 0");
      OCR4AL = 156;
      TCCR4B |= (1 << CS42) | (0 << CS41) | (1 << CS40);
      break;
    case 1: // 1.000Hz
      Serial.println("Setting Case 1");
      OCR4AL = 249;
      TCCR4B |= (0 << CS42) | (1 << CS41) | (1 << CS40);
      break;
    case 2: // 10.000Hz
      Serial.println("Setting Case 2");
      OCR4AL = 199;
      TCCR4B |= (0 << CS42) | (1 << CS41) | (0 << CS40);
      break;
    case 3: // 100.000Hz
      Serial.println("Setting Case 3");
      OCR4AL = 159;
      TCCR4B |= (0 << CS42) | (0 << CS41) | (1 << CS40);
      break;
    default:
      break;
  }

  TCNT4L = 0; //Reset low counter register
  TCNT4H = 0; //Reset high counter register

}

