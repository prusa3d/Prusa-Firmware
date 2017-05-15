/**
 * Author Teemu Mäntykallio
 * 
 * Plot TMC2130 motor load using the stallGuard value.
 * You can finetune the reading by changing the STALL_VALUE.
 * This will let you control at which load the value will read 0
 * and the stall flag will be triggered. This will also set pin DIAG1 high.
 * A higher STALL_VALUE will make the reading less sensitive and
 * a lower STALL_VALUE will make it more sensitive.
 * 
 * You can control the rotation speed with
 * 0 Stop
 * 1 Resume
 * + Speed up
 * - Slow down
 */
#define MAX_SPEED  40 // In timer value
#define MIN_SPEED  1000

#define STALL_VALUE 0 // [-64..63]

// Note: You also have to connect GND, 5V and VM.
//       A connection diagram can be found in the schematics.
#define EN_PIN    38  // Nano v3:  16 Mega:  38  //enable (CFG6)
#define DIR_PIN   55  //           19        55  //direction
#define STEP_PIN  54  //           18        54  //step
#define CS_PIN    40  //           17        64  //chip select

#include <SPI.h>
#include <TMC2130Stepper.h>
TMC2130Stepper TMC2130 = TMC2130Stepper(EN_PIN, DIR_PIN, STEP_PIN, CS_PIN);

void setup() {
  //init serial port
  {
	  Serial.begin(250000); //init serial port and set baudrate
	  while(!Serial); //wait for serial port to connect (needed for Leonardo only)
	  Serial.println("\nStart...");
    TMC2130.begin();
	}

  //set TMC2130 config
  {
		TMC2130.rms_current(600); // mA
		TMC2130.microsteps(16);
		TMC2130.diag1_stall(1);
		TMC2130.diag1_active_high(1);
		TMC2130.coolstep_min_speed(0xFFFFF); // 20bit max
		TMC2130.sg_stall_value(STALL_VALUE);
  }

  // Set stepper interrupt
  {
    cli();//stop interrupts
    TCCR1A = 0;// set entire TCCR1A register to 0
    TCCR1B = 0;// same for TCCR1B
    TCNT1  = 0;//initialize counter value to 0
    OCR1A = 256;// = (16*10^6) / (1*1024) - 1 (must be <65536)
    // turn on CTC mode
    TCCR1B |= (1 << WGM12);
    // Set CS11 bits for 8 prescaler
    TCCR1B |= (1 << CS11);// | (1 << CS10);  
    // enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);
    sei();//allow interrupts
  }

  //TMC2130 outputs on (LOW active)
  digitalWrite(EN_PIN, LOW);
}

ISR(TIMER1_COMPA_vect){
  PORTF |= 1 << 0;
  PORTF &= ~(1 << 0);
}

void loop()
{
  static uint32_t last_time=0;
  uint32_t ms = millis();

  while(Serial.available() > 0) {
  	int8_t read_byte = Serial.read();
  	if (read_byte == '0') 		 { TIMSK1 &= ~(1 << OCIE1A); digitalWrite( EN_PIN, HIGH ); }
  	else if (read_byte == '1') { TIMSK1 |=  (1 << OCIE1A); digitalWrite( EN_PIN,  LOW ); }
    else if (read_byte == '+') if (OCR1A > MAX_SPEED) OCR1A -= 20;
    else if (read_byte == '-') if (OCR1A < MIN_SPEED) OCR1A += 20;
  }
    
  if((ms-last_time) > 100) //run every 0.1s
  {
    last_time = ms;   
    Serial.print("0 ");
    Serial.println(TMC2130.sg_result(), DEC);
  }
}

