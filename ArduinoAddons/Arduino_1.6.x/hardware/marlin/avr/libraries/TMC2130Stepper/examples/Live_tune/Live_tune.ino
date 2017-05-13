#define EN_PIN    38  // Nano v3:	16 Mega:	38	//enable (CFG6)
#define DIR_PIN   55  //			19			55	//direction
#define STEP_PIN  54  //			18			54	//step
#define CS_PIN    40  //			17			40	//chip select

int speed = 10;
bool running = false;
float Rsense = 0.11;
float hold_x = 0.5;
boolean toggle1 = 0;

#include <TMC2130Stepper.h>
TMC2130Stepper myStepper = TMC2130Stepper(EN_PIN, DIR_PIN, STEP_PIN, CS_PIN);

ISR(TIMER1_COMPA_vect){//timer1 interrupt 1Hz toggles pin 13 (LED)
//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
  if (toggle1){
    digitalWrite(STEP_PIN, HIGH);
    toggle1 = 0;
  }
  else{
    digitalWrite(STEP_PIN, LOW);
    toggle1 = 1;
  }
}

void initTimer() {
	cli();//stop interrupts
	//set timer1 interrupt at 1Hz
	TCCR1A = 0;// set entire TCCR1A register to 0
	TCCR1B = 0;// same for TCCR1B
	TCNT1  = 0;//initialize counter value to 0
	// set compare match register for 1hz increments
	OCR1A = 256;// = (16*10^6) / (1*1024) - 1 (must be <65536)
	// turn on CTC mode
	TCCR1B |= (1 << WGM12);
	// Set CS12 and CS10 bits for 1024 prescaler
	TCCR1B |= (1 << CS11);// | (1 << CS10);  
	// enable timer compare interrupt
	TIMSK1 |= (1 << OCIE1A);
	sei();//allow interrupts
}

void setTimer(int t) {
	cli();
	TCNT1 = 0;
	OCR1A = t;
	sei();
}

void serialTuple(String cmd, int arg) {
	Serial.print("Received command: ");
	Serial.print(cmd);
	Serial.print("(");
	Serial.print(arg);
	Serial.println(")");
}


void setup() {
	initTimer();
	Serial.begin(9600);
	myStepper.begin();
	myStepper.SilentStepStick2130(1000);
	digitalWrite(EN_PIN, LOW);
	Serial.println("Setup ready");
}

void loop() {
	if (Serial.available() > 0) {
		String cmd = Serial.readStringUntil(' ');
		String strArg = Serial.readStringUntil('\n');

		int arg = strArg.toInt();

		if (cmd == "run") {
			serialTuple("run", arg);
			running = arg;
			arg ? digitalWrite(EN_PIN, LOW) : digitalWrite(EN_PIN, HIGH);
		}
		else if (cmd == "speed") {
			serialTuple("speed", arg);
			//speed = arg;
			setTimer(arg);
		}
		else if (cmd == "setCurrent") {
			serialTuple("setCurrent", arg);
			myStepper.setCurrent(arg, Rsense, hold_x);
		}
		else if (cmd == "Rsense") {
			Serial.print("Setting R sense value to: ");
			Serial.println(arg);
			Rsense = arg;
		}
		else if (cmd == "hold_multiplier") {
			Serial.print("Setting hold multiplier to: ");
			Serial.println(arg);
			hold_x = arg;
		}
		else if (cmd == "GCONF") {
			Serial.print("GCONF: 0b");
			Serial.println(myStepper.GCONF(), BIN);
		}
		else if (cmd == "external_ref") {
			serialTuple("external_ref", arg);
			myStepper.external_ref(arg);
		}
		else if (cmd == "internal_sense_R") {
			serialTuple("internal_sense_R", arg);
			myStepper.internal_sense_R(arg);
		}
		else if (cmd == "stealthChop") {
			serialTuple("stealthChop", arg);
			myStepper.stealthChop(arg);
		}
		else if (cmd == "commutation") {
			serialTuple("commutation", arg);
			myStepper.commutation(arg);
		}
		else if (cmd == "shaft_dir") {
			serialTuple("shaft_dir", arg);
			myStepper.shaft_dir(arg);
		}
		else if (cmd == "diag0_errors") {
			serialTuple("diag0_errors", arg);
			myStepper.diag0_errors(arg);
		}
		else if (cmd == "diag0_temp_prewarn") {
			serialTuple("diag0_temp_prewarn", arg);
			myStepper.diag0_temp_prewarn(arg);
		}
		else if (cmd == "diag0_stall") {
			serialTuple("diag0_stall", arg);
			myStepper.diag0_stall(arg);
		}
		else if (cmd == "diag1_stall") {
			serialTuple("diag1_stall", arg);
			myStepper.diag1_stall(arg);
		}
		else if (cmd == "diag1_index") {
			serialTuple("diag1_index", arg);
			myStepper.diag1_index(arg);
		}
		else if (cmd == "diag1_chopper_on") {
			serialTuple("diag1_chopper_on", arg);
			myStepper.diag1_chopper_on(arg);
		}
		else if (cmd == "diag1_steps_skipped") {
			serialTuple("diag1_steps_skipped", arg);
			myStepper.diag1_steps_skipped(arg);
		}
		else if (cmd == "diag0_active_high") {
			serialTuple("diag0_active_high", arg);
			myStepper.diag0_active_high(arg);
		}
		else if (cmd == "diag1_active_high") {
			serialTuple("diag1_active_high", arg);
			myStepper.diag1_active_high(arg);
		}
		else if (cmd == "small_hysterisis") {
			serialTuple("small_hysterisis", arg);
			myStepper.small_hysterisis(arg);
		}
		else if (cmd == "stop_enable") {
			serialTuple("stop_enable", arg);
			myStepper.stop_enable(arg);
		}
		else if (cmd == "direct_mode") {
			serialTuple("direct_mode", arg);
			myStepper.direct_mode(arg);
		}
		else if (cmd == "hold_current") {
			serialTuple("hold_current", arg);
			myStepper.hold_current(arg);
		}
		else if (cmd == "run_current") {
			serialTuple("run_current", arg);
			myStepper.run_current(arg);
		}
		else if (cmd == "hold_delay") {
			serialTuple("hold_delay", arg);
			myStepper.hold_delay(arg);
		}
		else if (cmd == "power_down_delay") {
			serialTuple("power_down_delay", arg);
			myStepper.power_down_delay(arg);
		}
		else if (cmd == "microstep_time") {
			Serial.print("microstep_time: ");
			Serial.println(myStepper.microstep_time());
		}
		else if (cmd == "stealth_max_speed") {
			serialTuple("stealth_max_speed", arg);
			myStepper.stealth_max_speed(arg);
		}
		else if (cmd == "coolstep_min_speed") {
			serialTuple("coolstep_min_speed", arg);
			myStepper.coolstep_min_speed(arg);
		}
		else if (cmd == "mode_sw_speed") {
			serialTuple("mode_sw_speed", arg);
			myStepper.mode_sw_speed(arg);
		}
		else if (cmd == "coil_A_current") {
			serialTuple("coil_A_current", arg);
			myStepper.coil_A_current(arg);
		}
		else if (cmd == "coil_B_current") {
			serialTuple("coil_B_current", arg);
			myStepper.coil_B_current(arg);
		}
		else if (cmd == "DCstep_min_speed") {
			serialTuple("DCstep_min_speed", arg);
			myStepper.DCstep_min_speed(arg);
		}
		else if (cmd == "CHOPCONF") {
			Serial.print("CHOPCONF: 0b");
			Serial.println(myStepper.CHOPCONF(), BIN);
		}
		else if (cmd == "off_time") {
			serialTuple("off_time", arg);
			myStepper.off_time(arg);
		}
		else if (cmd == "hysterisis_start") {
			serialTuple("hysterisis_start", arg);
			myStepper.hysterisis_start(arg);
		}
		else if (cmd == "fast_decay_time") {
			serialTuple("fast_decay_time", arg);
			myStepper.fast_decay_time(arg);
		}
		else if (cmd == "hysterisis_low") {
			serialTuple("hysterisis_low", arg);
			myStepper.hysterisis_low(arg);
		}
/*		else if (cmd == "sine_offset") {
			serialTuple("sine_offset", arg);
			myStepper.sine_offset(arg);
		}*/
		else if (cmd == "disable_I_comparator") {
			serialTuple("disable_I_comparator", arg);
			myStepper.disable_I_comparator(arg);
		}
		else if (cmd == "random_off_time") {
			serialTuple("random_off_time", arg);
			myStepper.random_off_time(arg);
		}
		else if (cmd == "chopper_mode") {
			serialTuple("chopper_mode", arg);
			myStepper.chopper_mode(arg);
		}
		else if (cmd == "blank_time") {
			serialTuple("blank_time", arg);
			myStepper.blank_time(arg);
		}
		else if (cmd == "high_sense_R") {
			serialTuple("high_sense_R", arg);
			myStepper.high_sense_R(arg);
		}
		else if (cmd == "fullstep_threshold") {
			serialTuple("fullstep_threshold", arg);
			myStepper.fullstep_threshold(arg);
		}
		else if (cmd == "high_speed_mode") {
			serialTuple("high_speed_mode", arg);
			myStepper.high_speed_mode(arg);
		}
		else if (cmd == "sync_phases") {
			serialTuple("sync_phases", arg);
			myStepper.sync_phases(arg);
		}
		else if (cmd == "microsteps") {
			serialTuple("microsteps", arg);
			myStepper.microsteps(arg);
		}
		else if (cmd == "interpolate") {
			serialTuple("interpolate", arg);
			myStepper.interpolate(arg);
		}
		else if (cmd == "double_edge_step") {
			serialTuple("double_edge_step", arg);
			myStepper.double_edge_step(arg);
		}
		else if (cmd == "disable_short_protection") {
			serialTuple("disable_short_protection", arg);
			myStepper.disable_short_protection(arg);
		}
		else if (cmd == "sg_min") {
			serialTuple("sg_min", arg);
			myStepper.sg_min(arg);
		}
		else if (cmd == "sg_max") {
			serialTuple("sg_max", arg);
			myStepper.sg_max(arg);
		}
		else if (cmd == "sg_step_width") {
			serialTuple("sg_step_width", arg);
			myStepper.sg_step_width(arg);
		}
		else if (cmd == "sg_current_decrease") {
			serialTuple("sg_current_decrease", arg);
			myStepper.sg_current_decrease(arg);
		}
		else if (cmd == "smart_min_current") {
			serialTuple("smart_min_current", arg);
			myStepper.smart_min_current(arg);
		}
		else if (cmd == "sg_stall_value") {
			serialTuple("sg_stall_value", arg);
			myStepper.sg_stall_value(arg);
		}
		else if (cmd == "sg_filter") {
			serialTuple("sg_filter", arg);
			myStepper.sg_filter(arg);
		}
		else if (cmd == "stealth_amplitude") {
			serialTuple("stealth_amplitude", arg);
			myStepper.stealth_amplitude(arg);
		}
		else if (cmd == "stealth_gradient") {
			serialTuple("stealth_gradient", arg);
			myStepper.stealth_gradient(arg);
		}
		else if (cmd == "stealth_freq") {
			serialTuple("stealth_freq", arg);
			myStepper.stealth_freq(arg);
		}
		else if (cmd == "stealth_freq") {
			serialTuple("stealth_freq", arg);
			myStepper.stealth_freq(arg);
		}
		else if (cmd == "stealth_autoscale") {
			serialTuple("stealth_autoscale", arg);
			myStepper.stealth_autoscale(arg);
		}
		else if (cmd == "stealth_symmetric") {
			serialTuple("stealth_symmetric", arg);
			myStepper.stealth_symmetric(arg);
		}
		else if (cmd == "standstill_mode") {
			serialTuple("standstill_mode", arg);
			myStepper.standstill_mode(arg);
		}
		else if (cmd == "DRVSTATUS") {
			Serial.print("DRVSTATUS: 0b");
			Serial.println(myStepper.DRV_STATUS(), BIN);
		}
		else if (cmd == "PWM_SCALE") {
			Serial.print("PWM_SCALE: 0b");
			Serial.println(myStepper.PWM_SCALE(), BIN);
		}
		else if (cmd == "invert_encoder") {
			serialTuple("invert_encoder", arg);
			myStepper.invert_encoder(arg);
		}
		else if (cmd == "maxspeed") {
			serialTuple("maxspeed", arg);
			myStepper.maxspeed(arg);
		}
		else if (cmd == "interpolate") {
			serialTuple("interpolate", arg);
			myStepper.interpolate(arg);
		}
		else if (cmd == "LOST_STEPS") {
			Serial.print("LOST_STEPS: 0b");
			Serial.println(myStepper.LOST_STEPS(), BIN);
		}
		else {
			Serial.println("Invalid command!");
		}
	}
/*	
	if (running) {
		digitalWrite(STEP_PIN, HIGH);
		delayMicroseconds(speed);
		digitalWrite(STEP_PIN, LOW);
		delayMicroseconds(speed);
	}
*/
}
