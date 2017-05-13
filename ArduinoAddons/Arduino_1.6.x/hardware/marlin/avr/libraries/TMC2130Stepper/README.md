# TMC2130Stepper
Arduino library for Trinamic TMC2130 Stepper driver

## TODO:
- [ ] Comments
- [x] Documentation
- [x] Examples
- [x] Easy setup for NEMA17 with default settings
- [x] Convert functions returning 0/1 to boolean
- [ ] Fritzing image of the wiring for an example setup

## Installation:

Use the Arduino IDE library manager (Sketch -> Include library -> Manage libraries...)

Search for TMC2130Stepper and then install.

Or download the zip file from Github and extract it to<br>
your-scetchbook-location/libraries<br>
and restart the IDE.

or goto to you arduino libraries folder and in command line
    git clone https://github.com/teemuatlut/TMC2130Stepper.git

## What works:
Nearly all the features in the registeries are configurable through get/set functions. See below for a list of functions. Datasheet ([link](https://www.trinamic.com/fileadmin/assets/Products/ICs_Documents/TMC2130_datasheet.pdf)) from Trinamic also provides further detail into the settings available.

## Simple example
```cpp
/*
Initializes the library and turns the motor in alternating directions.
*/
#define EN_PIN    38  // Nano v3:	16 Mega:	38	//enable (CFG6)
#define DIR_PIN   55  //			19			55	//direction
#define STEP_PIN  54  //			18			54	//step
#define CS_PIN    40  //			17			64	//chip select

bool dir = true;


#include <TMC2130Stepper.h>
TMC2130Stepper TMC2130 = TMC2130Stepper(EN_PIN, DIR_PIN, STEP_PIN, CS_PIN);

void setup() {
	Serial.begin(9600);
	TMC2130.begin(); // Initiate pins and registeries
	TMC2130.SilentStepStick2130(600); // Set stepper current to 600mA
	TMC2130.stealthChop(1); // Enable extremely quiet stepping
	
	digitalWrite(EN_PIN, LOW);
}

void loop() {
	digitalWrite(STEP_PIN, HIGH);
	delayMicroseconds(10);
	digitalWrite(STEP_PIN, LOW);
	delayMicroseconds(10);
	uint32_t ms = millis();
	static uint32_t last_time = 0;
	if ((ms - last_time) > 2000) {
		if (dir) {
			Serial.println("Dir -> 0");
			TMC2130.shaft_dir(0);
		} else {
			Serial.println("Dir -> 1");
			TMC2130.shaft_dir(1);
		}
		dir = !dir;
		Serial.println(TMC2130.GCONF(), BIN);
		last_time = ms;
	}
}
```

## Current calculations

A simple way to set the current is to use the setCurrent() method.

    I_rms = (CS+1)/32 * V_fs/(R_sense+0.02mOhm) * 1/sqrt(2)
    I_motor = I_sine/248 * (CS+1/32) * V_fs/(R_sense+0.02mOhm)

Where:<br>
I_rms is the rms current<br>
I_motor is the motor current<br>
CS is the Current Scale value<br>
V_fs is the voltage determined by v_sense<br>
R_sense is the chosen sense resistor<br>
I_sine is the current position in the sine table. I_motor reaches peak value at I_sine = 248<p>

If external_ref is enabled, V_fs is scaled by V_ain/2.5V

## Functions

Function 			| Argument range | Returns | Description
--------------------|-----|---|-----------------------------
begin 				|  -  | - | Initialized pins Enable, Direction, Step and Chip Select.<br>Initialized the SPI pins MOSI, MISO and SCK.<br>Calls spi.begin()<br>Sets off_time = 2 and blank_time = 24
setCurrent			|  0..2000<br>0.1 .. 1<br>0..1 | - | Helper function to set the motor RMS current.<br>Arguments:<br><b>uint16_t</b> Desired current in milliamps<br><b>float</b> Sense resistor value<br><b>float</b> Multiplier for holding current<br>Example for SilentStepStick2130: setCurrent(1200, 0.11, 0.5)<p>Makes use of the run_current() and hold_current() funtions.
SilentStepStick2130 |  0..2000  | - | Calls the begin() functions and according to the argument sets the current with sense resistor being 0.11 and multiplier being 0.5

## Register functions:

Note: You can read the saved value by calling a function without providing an argument.

### GCONF register
Function 			| Argument range | Returns | Description
--------------------|-----|---------|-----------------------------
GCONF 				|  -  | uint32_t| Read actual bits from the register
external_ref 		| 0/1 | bool | Use external voltage reference for coil currents
internal_sense_R 	| 0/1 | bool | Use internal sense resistors
stealthChop 		| 0/1 | bool | Enable stealthChop (dependant on velocity thresholds)
commutation 		| 0/1 | bool | Enable commutation by full step encoder
shaft_dir 			| 0/1 | bool | Inverse motor direction
diag0_errors 		| 0/1 | bool | Enable DIAG0 active on driver errors: Over temperature (ot), short to GND (s2g), undervoltage chargepump (uv_cp)
diag0_temp_prewarn 	| 0/1 | bool | Enable DIAG0 active on driver over temperature prewarning
diag0_stall 		| 0/1 | bool | Enable DIAG0 active on motor stall (set TCOOLTHRS before using this feature)
diag1_stall 		| 0/1 | bool | Enable DIAG1 active on motor stall (set TCOOLTHRS before using this feature) 
diag1_index 		| 0/1 | bool | Enable DIAG1 active on index position (microstep look up table position 0) 
diag1_chopper_on 	| 0/1 | bool | Enable DIAG1 active when chopper is on
diag1_steps_skipped | 0/1 | bool | Enable output toggle when steps are skipped in dcStep mode (increment of LOST_STEPS). Do not enable in conjunction with other DIAG1 options. 
diag0_active_high 	| 0/1 | bool | Set DIAG0 to active high
diag1_active_high 	| 0/1 | bool | Set DIAG1 to active high
small_hysterisis 	| 0/1 | bool | <b>0:</b> Hysteresis for step frequency comparison is 1/16 <br><b>1:</b> Hysteresis for step frequency comparison is 1/32 
stop_enable 		| 0/1 | bool | Emergency stop: DCIN stops the sequencer when tied high (no steps become executed by the sequencer, motor goes to standstill state).
direct_mode 		| 0/1 | bool | Motor coil currents and polarity are directly controlled by the SPI interface.

### IHOLD_IRUN register
Function 		| Argument 	| Returns | Description
----------------|-----------|---------|-----------------------------
hold_current 	| 0..31 	| uint8_t | Standstill current (0=1/32…31=32/32)
run_current 	| 0..31 	| uint8_t | Motor run current (0=1/32…31=32/32)
hold_delay 		| 0..15 	| uint8_t | Controls the number of clock cycles for motor power down after a motion as soon as standstill is detected (stst=1) and TPOWERDOWN has expired. 

### REG_TPOWERDOWN register
Function 		 | Argument | Returns 	| Description
-----------------|----------|-----------|-----------------------------
power_down_delay | 0..255 	| uint8_t  	| power_down_delay sets the delay time after stand still (stst) of the motor to motor current power down. Time range is about 0 to 4 seconds. <br>0…((2^8)-1) * 2^18 tCLK 

### REG_TSTEP register
Function 		| Argument 	| Returns  | Description
----------------|-----------|----------|-----------------------------
microstep_time 	| - 		| uint32_t | Read the actual measured time between two 1/256 microsteps derived from the step input frequency in units of 1/fCLK. 

### REG_TPWMTHRS register
Function 			| Argument 		| Returns	| Description
--------------------|---------------|-----------|-----------------------------
stealth_max_speed 	| 0..1,048,575 	| uint32_t 	| This is the upper velocity for stealthChop voltage PWM mode. TSTEP ≥ TPWMTHRS - stealthChop PWM mode is enabled, if configured - dcStep is disabled 

### REG_TCOOLTHRS register
Function 			| Argument 		| Returns 	| Description
--------------------|---------------|-----------|-----------------------------
coolstep_min_speed 	| 0..1,048,575 	| uint32_t 	| This is the lower threshold velocity for switching on smart energy coolStep and stallGuard feature. 

### REG_THIGH register
Function 		| Argument 		| Returns 	| Description
----------------|---------------|-----------|-----------------------------
mode_sw_speed 	| 0..1,048,575 	| uint32_t  | This velocity setting allows velocity dependent switching into a different chopper mode and fullstepping to maximize torque. 

### REG_XDRIRECT register
Function 		| Argument 		| Returns 	| Description
----------------|---------------|-----------|-----------------------------
coil_A_current 	| -255..+255 	| int16_t  	| Specifies Motor coil currents and polarity directly programmed via the serial interface. In this mode, the current is scaled by IHOLD setting. 
coil_B_current 	| -255..+255 	| int16_t  	| As above.

### REG_VDCMIN register
Function 			| Argument 		| Returns 	| Description
--------------------|---------------|-----------|-----------------------------
DCstep_min_speed 	| 0..8,388,607 	| uint32_t  | The automatic commutation dcStep becomes enabled by the external signal DCEN. VDCMIN is used as the minimum step velocity when the motor is heavily loaded. <br>Hint: Also set DCCTRL parameters in order to operate dcStep.

### REG_CHOPCONF register
Function 				| Argument  | Returns 	| Description
------------------------|-----------|-----------|-----------------------------
CHOPCONF 				| - 		| uint32_t 	| Read actual bits from the register
off_time 				| 0..15 	| uint8_t 	| Off time setting controls duration of slow decay phase<br>NCLK= 12 + 32*TOFF<br>Initialized to value 2 (NCLK = 76) by begin()
hysterisis_start 		| 1..8 		| uint8_t 	| Add 1, 2, …, 8 to hysteresis low value HEND (1/512 of this setting adds to current setting) Attention: Effective HEND+HSTRT ≤ 16. Hint: Hysteresis decrement is done each 16 clocks
fast_decay_time 		| 0..15 	| uint8_t 	| Fast decay time setting TFD with  NCLK= 32*HSTRT
hysterisis_low 			| -3..12	| int8_t 	| This is the hysteresis value which becomes used for the hysteresis chopper. 
sine_offset 			| -3..12	| int8_t 	| This is the sine wave offset and 1/512 of the value becomes added to the absolute value of each sine wave entry.
disable_I_comparator 	| 0/1		| bool 	| <b>1:</b> Disables current comparator usage for termination of the fast decay cycle.<br>chopper_mode needs to be 1.
random_off_time 		| 0/1		| bool 	| <b>0:</b> Chopper off time is fixed as set by TOFF <br><b>1:</b> Random mode, TOFF is random modulated by dNCLK= -12 … +3 clocks. 
chopper_mode 			| 0/1		| uint8_t 	| <b>0:</b> Standard mode (spreadCycle) <br><b>1:</b> Constant off time with fast decay time.  <br>Fast decay time is also terminated when the negative nominal current is reached. Fast decay is after on time. 
blank_time 				| 16, 24,<br>36, 54| uint8_t | Set comparator blank time to 16, 24, 36 or 54 clocks. <br>Hint: 24 or 36 is recommended for most applications<br>Initialized to 36 (register value = 3) by begin()
high_sense_R 			| 0/1 		| bool 	| <b>0:</b> Low sensitivity, high sense resistor voltage <br><b>1:</b> High sensitivity, low sense resistor voltage 
fullstep_threshold 		| 0/1		| bool 	| This bit enables switching to fullstep, when VHIGH is exceeded. Switching takes place only at 45° position. The fullstep target current uses the current value from the microstep table at the 45° position. 
high_speed_mode 		| 0/1		| bool 	| This bit enables switching to chm=1 and fd=0, when VHIGH is exceeded. This way, a higher velocity can be achieved. Can be combined with vhighfs=1. If set, the TOFF setting automatically becomes doubled during high velocity operation in order to avoid doubling of the chopper frequency. 
sync_phases 			| 0..15		| bool 	| Synchronization of the chopper for both phases of a two phase motor in order to avoid the occurrence of a beat, especially at low motor velocities. It is automatically switched off above VHIGH.
microsteps 				| 255, 128, 64, 32, 16,<br>8, 4, 2, 0 (FULLSTEP) | uint8_t | Reduced microstep resolution for Step/Dir operation. The resolution gives the number of microstep entries per sine quarter wave.
interpolate 			| 0/1 		| bool 	| The actual microstep resolution becomes extrapolated to 256 microsteps for smoothest motor operation.
double_edge_step 		| 0/1 		| bool 	| Enable step impulse at each step edge to reduce step frequency requirement. 
disable_short_protection| 0/1 		| bool 	| <b>0:</b> Short to GND protection is on <br><b>1:</b> Short to GND protection is disabled 

### REG_COOLCONF register
Function 			| Argument 	 | Returns  | Description
--------------------|------------|----------|-----------------------------
COOLCONF 			| - 		 | uint32_t | Read actual bits from the register
sg_min 				| 0..15 	 | uint8_t 	| If the stallGuard2 result falls below sg_min*32, the motor current becomes increased to reduce motor load angle.
sg_max 				| 0..15 	 | uint8_t 	| If the stallGuard2 result is equal to or above (sg_min+sg_max+1)*32, the motor current becomes decreased to save energy. 
sg_step_width 		| 1, 2, 4, 8 | uint8_t 	| Current increment steps per measured stallGuard2 value
sg_current_decrease | 1, 2, 8, 32| uint8_t 	| For each (value) stallGuard2 values decrease by one 
smart_min_current 	| uint8_t 	 | uint8_t 	| <b>0:</b> 1/2 of current setting (IRUN)<br><b>1:</b> 1/4 of current setting (IRUN) 
sg_stall_value		| int8_t 	 | int8_t  	| This signed value controls stallGuard2 level for stall output and sets the optimum measurement range for readout. A lower value gives a higher sensitivity. Zero is the starting value working with most motors.  -64 to +63:  A higher value makes stallGuard2 less sensitive and requires more torque to indicate a stall. 
sg_filter 			| uint8_t 	 | uint8_t 	| <b>0:</b> Standard mode, high time resolution for stallGuard2<br><b>1:</b> Filtered mode, stallGuard2 signal updated for each four fullsteps (resp. six fullsteps for 3 phase motor) only to compensate for motor pole tolerances 

### REG_PWMCONF register
Function | Argument | Returns | Description
--------------------|-----------|----------|-----------------------------
PWMCONF 			| - 		| uint32_t | Read actual bits from the register
stealth_amplitude 	| 0..255 	| uint8_t  | pwm_ autoscale=0 <br>User defined  PWM amplitude offset (0-255) The resulting amplitude (limited to 0…255) is: PWM_AMPL + PWM_GRAD * 256 / TSTEP <p>pwm_ autoscale=1 <br>User defined maximum PWM amplitude when switching back from current chopper mode to voltage PWM mode (switch over velocity defined by TPWMTHRS). Do not set too low values, as the regulation cannot measure the current when the actual PWM value goes below a setting specific value. Settings above 0x40 recommended. 
stealth_gradient 	| 0..255 	| uint8_t  | pwm_ autoscale=0 <br>Velocity dependent gradient for PWM amplitude:  PWM_GRAD * 256 / TSTEP  is added to PWM_AMPL <p>pwm_ autoscale=1 <br>User defined  maximum PWM amplitude change per half wave (1 to 15) 
stealth_freq 		| 0..3 		| uint8_t  | <b>0:</b> fPWM=2/1024 fCLK<br><b>1:</b> fPWM=2/683 fCLK<br><b>2:</b> fPWM=2/512 fCLK<br><b>3:</b> fPWM=2/410 fCLK
stealth_autoscale 	| 0/1		| bool     | <b>0:</b> User defined PWM amplitude. The current settings have no influence.<br><b>1:</b> Enable automatic current control Attention: When using a user defined sine wave table, the amplitude of this sine wave table should not be less than 244. Best results are obtained with 247 to 252 as peak values.
stealth_symmetric 	| 0/1	 	| bool     | <b>0:</b> The PWM value may change within each PWM cycle (standard mode)<br><b>1:</b> A symmetric PWM cycle is enforced 
standstill_mode 	| 0..3		| uint8_t  | Stand still option when motor current setting is zero (I_HOLD=0).<br><b>0:</b>  Normal operation<br><b>1:</b>  Freewheeling<br><b>2:</b> Coil shorted using LS drivers<br><b>3:</b>  Coil shorted using HS drivers 

### REG_DRVSTATUS register
Function 	| Argument 	| Returns 	| Description
------------|-----------|-----------|-----------------------------
DRVSTATUS 	| - 		| uint32_t 	| Read actual bits from the register

### REG_PWM_SCALE register
Function 	| Argument 	| Returns 	| Description
------------|-----------|-----------|-----------------------------
PWM_SCALE 	| - 		| uint32_t 	| Read actual bits from the register

### REG_ENCM_CTRL register
Function 		| Argument 	| Returns 	| Description
----------------|-----------|-----------|-----------------------------
invert_encoder 	| 0/1 		| bool  	| Invert encoder inputs
maxspeed 		| 0/1	 	| bool  	| Ignore Step input. If set, the hold current IHOLD determines the motor current, unless a step source is activated

### REG_LOST_STEPS register
Function 	| Argument 	| Returns 	| Description
------------|-----------|-----------|-----------------------------
LOST_STEPS 	| - 		| uint32_t 	| Read actual bits from the register

### Read raw registry:
