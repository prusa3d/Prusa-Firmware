#include "Marlin.h"

#ifdef HAVE_TMC2130_DRIVERS

#include "tmc2130.h"
#include <SPI.h>

//externals for debuging
extern float current_position[4];
extern void st_get_position_xy(long &x, long &y);

//chipselect pins
uint8_t tmc2130_cs[4] = { X_TMC2130_CS, Y_TMC2130_CS, Z_TMC2130_CS, E0_TMC2130_CS };
//holding currents
uint8_t tmc2130_current_h[4] = TMC2130_CURRENTS_H;
//running currents
uint8_t tmc2130_current_r[4] = TMC2130_CURRENTS_R;
//axis stalled flags
uint8_t tmc2130_axis_stalled[4] = {0, 0, 0, 0};
//last homing stalled
uint8_t tmc2130_LastHomingStalled = 0;

uint8_t sg_homing_axis = 0xff;
uint8_t sg_homing_delay = 0;


uint32_t tmc2130_read(uint8_t cs, uint8_t address);
void tmc2130_write(uint8_t cs, uint8_t address, uint8_t wval1, uint8_t wval2, uint8_t wval3, uint8_t wval4);
uint8_t tmc2130_read8(uint8_t cs, uint8_t address);
uint32_t tmc2130_readRegister(uint8_t cs, uint8_t address);
uint16_t tmc2130_readSG(uint8_t cs);
uint16_t tmc2130_readTStep(uint8_t cs);
void tmc2130_chopconf(uint8_t cs, bool extrapolate256 = 0, uint16_t microstep_resolution = 16);
void tmc2130_PWMconf(uint8_t cs, uint8_t PWMautoScale = PWM_AUTOSCALE, uint8_t PWMfreq = PWM_FREQ, uint8_t PWMgrad = PWM_GRAD, uint8_t PWMampl = PWM_AMPL);
void tmc2130_PWMthreshold(uint8_t cs);
void tmc2130_disable_motor(uint8_t driver);


void tmc2130_init()
{
	MYSERIAL.println("tmc2130_init");
	WRITE(X_TMC2130_CS, HIGH);
	WRITE(Y_TMC2130_CS, HIGH);
	WRITE(Z_TMC2130_CS, HIGH);
	WRITE(E0_TMC2130_CS, HIGH);
	SET_OUTPUT(X_TMC2130_CS);
	SET_OUTPUT(Y_TMC2130_CS);
	SET_OUTPUT(Z_TMC2130_CS);
	SET_OUTPUT(E0_TMC2130_CS);
	SPI.begin();
	for (int i = 0; i < 3; i++) //X Y Z axes
	{
		tmc2130_write(tmc2130_cs[i], 0x00, 0, 0, 0, 0x04); //address=0x0 GCONF - bit 2 activate stealthChop
		tmc2130_write(tmc2130_cs[i], 0x10, 0, 15, tmc2130_current_r[i], tmc2130_current_h[i]); //0x10 IHOLD_IRUN
		tmc2130_write(tmc2130_cs[i], 0x11, 0, 0, 0, 0);
		tmc2130_PWMconf(tmc2130_cs[i]); //address=0x70 PWM_CONF //reset default=0x00050480
		//tmc2130_PWMthreshold(tmc2130_cs[i]);
		tmc2130_chopconf(tmc2130_cs[i], 1, 16);
	}
	for (int i = 3; i < 4; i++) //E axis
	{
		tmc2130_write(tmc2130_cs[i], 0x00, 0, 0, 0, 0x00); //address=0x0 GCONF - bit 2 activate stealthChop
		tmc2130_write(tmc2130_cs[i], 0x10, 0, 15, tmc2130_current_r[i], tmc2130_current_h[i]); //0x10 IHOLD_IRUN
		tmc2130_write(tmc2130_cs[i], 0x11, 0, 0, 0, 0);
		tmc2130_chopconf(tmc2130_cs[i], 1, 16);
	}
}

bool tmc2130_update_sg()
{
	if ((sg_homing_axis == X_AXIS) || (sg_homing_axis == Y_AXIS))
	{
		uint8_t cs = tmc2130_cs[sg_homing_axis];
		uint16_t tstep = tmc2130_readTStep(cs);
		if (tstep < TCOOLTHRS)
		{
			if(sg_homing_delay < 10) // wait for a few tens microsteps until stallGuard is used //todo: read out microsteps directly, instead of delay counter
				sg_homing_delay++;
			else
			{
				uint16_t sg = tmc2130_readSG(cs);
				if (sg==0)
				{
					tmc2130_axis_stalled[sg_homing_axis] = true;
					tmc2130_LastHomingStalled = true;
				}
				else
					tmc2130_axis_stalled[sg_homing_axis] = false;
			}
		}
		else
			tmc2130_axis_stalled[sg_homing_axis] = false;
		return true;
	}
	else
	{
		tmc2130_axis_stalled[X_AXIS] = false;
		tmc2130_axis_stalled[Y_AXIS] = false;
	}
	return false;
}

void tmc2130_check_overtemp()
{
	const static char TMC_OVERTEMP_MSG[] PROGMEM = "TMC DRIVER OVERTEMP ";
	static uint32_t checktime = 0;
	//drivers_disabled[0] = 1; //TEST
	if( millis() - checktime > 1000 )
	{
		for(int i = 0; i < 4; i++)
		{
			uint32_t drv_status = tmc2130_read(tmc2130_cs[i], 0x6F); //0x6F DRV_STATUS
			if (drv_status & ((uint32_t)1<<26))
			{ // BIT 26 - over temp prewarning ~120C (+-20C)
				SERIAL_ERRORRPGM(TMC_OVERTEMP_MSG);
				SERIAL_ECHOLN(i);
				for(int x = 0; x < 4; x++) tmc2130_disable_motor(x);
					kill(TMC_OVERTEMP_MSG);
			}
		}
		checktime = millis();
	}
}

void tmc2130_home_enter(uint8_t axis)
{
	MYSERIAL.print("tmc2130_home_enter ");
	MYSERIAL.println((int)axis);
	uint8_t cs = tmc2130_cs[axis];
	sg_homing_axis = axis;
	sg_homing_delay = 0;
	tmc2130_axis_stalled[X_AXIS] = false;
	tmc2130_axis_stalled[Y_AXIS] = false;
	//Configuration to spreadCycle
	//tmc2130_write(cs, 0x0, 0, 0, 0, 0x01);
	tmc2130_write(cs, 0x0, 0, 0, 0, 0x00);
	tmc2130_write(cs, 0x6D, 0, (axis == X_AXIS)?SG_THRESHOLD_X:SG_THRESHOLD_Y,0,0);
	tmc2130_write(cs, 0x14, 0, 0, 0, TCOOLTHRS);
}

void tmc2130_home_exit()
{
	MYSERIAL.println("tmc2130_home_exit");
	if ((sg_homing_axis == X_AXIS) || (sg_homing_axis == Y_AXIS))
	{
		// Configuration back to stealthChop
		tmc2130_write(tmc2130_cs[sg_homing_axis], 0x0, 0, 0, 0, 0x04);
		sg_homing_axis = 0xff;
	}
}

extern uint8_t tmc2130_didLastHomingStall()
{
	uint8_t ret = tmc2130_LastHomingStalled;
	tmc2130_LastHomingStalled = false;
	return ret;
}

void tmc2130_set_current_h(uint8_t axis, uint8_t current)
{
	MYSERIAL.print("tmc2130_set_current_h ");
	MYSERIAL.print((int)axis);
	MYSERIAL.print(" ");
	MYSERIAL.println((int)current);
	if (current > 15) current = 15; //current>15 is unsafe
	tmc2130_current_h[axis] = current;
	tmc2130_write(tmc2130_cs[axis], 0x10, 0, 15, tmc2130_current_r[axis], tmc2130_current_h[axis]); //0x10 IHOLD_IRUN
}

void tmc2130_set_current_r(uint8_t axis, uint8_t current)
{
	MYSERIAL.print("tmc2130_set_current_r ");
	MYSERIAL.print((int)axis);
	MYSERIAL.print(" ");
	MYSERIAL.println((int)current);
	if (current > 15) current = 15; //current>15 is unsafe
	tmc2130_current_r[axis] = current;
	tmc2130_write(tmc2130_cs[axis], 0x10, 0, 15, tmc2130_current_r[axis], tmc2130_current_h[axis]); //0x10 IHOLD_IRUN
}

void tmc2130_print_currents()
{
	MYSERIAL.println("tmc2130_print_currents");
	MYSERIAL.println("\tH\rR");

	MYSERIAL.print("X\t");
	MYSERIAL.print((int)tmc2130_current_h[0]);
	MYSERIAL.print("\t");
	MYSERIAL.println((int)tmc2130_current_r[0]);

	MYSERIAL.print("Y\t");
	MYSERIAL.print((int)tmc2130_current_h[1]);
	MYSERIAL.print("\t");
	MYSERIAL.println((int)tmc2130_current_r[1]);

	MYSERIAL.print("Z\t");
	MYSERIAL.print((int)tmc2130_current_h[2]);
	MYSERIAL.print("\t");
	MYSERIAL.println((int)tmc2130_current_r[2]);

	MYSERIAL.print("E\t");
	MYSERIAL.print((int)tmc2130_current_h[3]);
	MYSERIAL.print("\t");
	MYSERIAL.println((int)tmc2130_current_r[3]);
}

uint32_t tmc2130_read(uint8_t cs, uint8_t address)
{
	uint32_t val32;
	uint8_t val0;
	uint8_t val1;
	uint8_t val2;
	uint8_t val3;
	uint8_t val4;
	//datagram1 - read request (address + dummy write)
	SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE3));
	digitalWrite(cs,LOW);
	SPI.transfer(address);
	SPI.transfer(0);
	SPI.transfer(0);
	SPI.transfer(0);
	SPI.transfer(0);
	digitalWrite(cs, HIGH);
	SPI.endTransaction();
	//datagram2 - response
	SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE3));
	digitalWrite(cs,LOW);
	val0 = SPI.transfer(0);
	val1 = SPI.transfer(0);
	val2 = SPI.transfer(0);
	val3 = SPI.transfer(0);
	val4 = SPI.transfer(0);
	digitalWrite(cs, HIGH);
	SPI.endTransaction();
#ifdef TMC_DBG_READS
	MYSERIAL.print("SPIRead 0x");
	MYSERIAL.print(address,HEX);
	MYSERIAL.print(" Status:");
	MYSERIAL.print(val0 & 0b00000111,BIN);
	MYSERIAL.print("  ");
	MYSERIAL.print(val1,BIN);
	MYSERIAL.print("  ");
	MYSERIAL.print(val2,BIN);
	MYSERIAL.print("  ");
	MYSERIAL.print(val3,BIN);
	MYSERIAL.print("  ");
	MYSERIAL.print(val4,BIN);
#endif
	val32 = (uint32_t)val1<<24 | (uint32_t)val2<<16 | (uint32_t)val3<<8 | (uint32_t)val4;
#ifdef TMC_DBG_READS
	MYSERIAL.print(" 0x");
	MYSERIAL.println(val32,HEX);
#endif
	return val32;
}

void tmc2130_write(uint8_t cs, uint8_t address,uint8_t wval1,uint8_t wval2,uint8_t wval3,uint8_t wval4)
{
	uint32_t val32;
	uint8_t val0;
	uint8_t val1;
	uint8_t val2;
	uint8_t val3;
	uint8_t val4;
	//datagram1 - write
	SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE3));
	digitalWrite(cs,LOW);
	SPI.transfer(address+0x80);
	SPI.transfer(wval1);
	SPI.transfer(wval2);
	SPI.transfer(wval3);
	SPI.transfer(wval4);
	digitalWrite(cs, HIGH);
	SPI.endTransaction();
	//datagram2 - response
	SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE3));
	digitalWrite(cs,LOW);
	val0 = SPI.transfer(0);
	val1 = SPI.transfer(0);
	val2 = SPI.transfer(0);
	val3 = SPI.transfer(0);
	val4 = SPI.transfer(0);
	digitalWrite(cs, HIGH);
	SPI.endTransaction();
#ifdef TMC_DBG_WRITE
	MYSERIAL.print("WriteRead 0x");
	MYSERIAL.print(address,HEX);
	MYSERIAL.print(" Status:");
	MYSERIAL.print(val0 & 0b00000111,BIN);
	MYSERIAL.print("  ");
	MYSERIAL.print(val1,BIN);
	MYSERIAL.print("  ");
	MYSERIAL.print(val2,BIN);
	MYSERIAL.print("  ");
	MYSERIAL.print(val3,BIN);
	MYSERIAL.print("  ");
	MYSERIAL.print(val4,BIN);
	val32 = (uint32_t)val1<<24 | (uint32_t)val2<<16 | (uint32_t)val3<<8 | (uint32_t)val4;
	MYSERIAL.print(" 0x");
	MYSERIAL.println(val32,HEX);
#endif //TMC_DBG_READS
}

uint8_t tmc2130_read8(uint8_t cs, uint8_t address)
{
	//datagram1 - write
	SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE3));
	digitalWrite(cs,LOW);
	SPI.transfer(address);
	SPI.transfer(0x00);
	SPI.transfer(0x00);
	SPI.transfer(0x00);
	SPI.transfer(0x00);
	digitalWrite(cs, HIGH);
	SPI.endTransaction();
	uint8_t val0;
	//datagram2 - response
	SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE3));
	digitalWrite(cs,LOW);
	val0 = SPI.transfer(0);
	digitalWrite(cs, HIGH);
	SPI.endTransaction();
	return val0;
}

uint32_t tmc2130_readRegister(uint8_t cs, uint8_t address)
{
	//datagram1 - write
	SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE3));
	digitalWrite(cs,LOW);
	SPI.transfer(address);
	SPI.transfer(0x00);
	SPI.transfer(0x00);
	SPI.transfer(0x00);
	SPI.transfer(0x00);
	digitalWrite(cs, HIGH);
	SPI.endTransaction();
	uint32_t val0;
	//datagram2 - response
	SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE3));
	digitalWrite(cs,LOW);
	SPI.transfer(0); // ignore status bits
	val0 = SPI.transfer(0); // MSB
	val0 = (val0 << 8) | SPI.transfer(0);
	val0 = (val0 << 8) | SPI.transfer(0);
	val0 = (val0 << 8) | SPI.transfer(0); //LSB
	digitalWrite(cs, HIGH);
	SPI.endTransaction();
	return val0;
}

uint16_t tmc2130_readSG(uint8_t cs)
{
	uint8_t address = 0x6F;
	uint32_t registerValue = tmc2130_readRegister(cs, address);
	uint16_t val0 = registerValue & 0x3ff;
	return val0;
}

uint16_t tmc2130_readTStep(uint8_t cs)
{
	uint8_t address = 0x12;
	uint32_t registerValue = tmc2130_readRegister(cs, address);
	uint16_t val0 = 0;
	if(registerValue & 0x000f0000)
		val0 = 0xffff;
	else
		val0 = registerValue & 0xffff;
	return val0;
}

void tmc2130_chopconf(uint8_t cs, bool extrapolate256, uint16_t microstep_resolution)
{
	uint8_t mres = 0b0100;
	if(microstep_resolution == 256) mres = 0b0000;
	if(microstep_resolution == 128) mres = 0b0001;
	if(microstep_resolution == 64)  mres = 0b0010;
	if(microstep_resolution == 32)  mres = 0b0011;
	if(microstep_resolution == 16)  mres = 0b0100;
	if(microstep_resolution == 8)   mres = 0b0101;
	if(microstep_resolution == 4)   mres = 0b0110;
	if(microstep_resolution == 2)   mres = 0b0111;
	if(microstep_resolution == 1)   mres = 0b1000;
	mres |= extrapolate256 << 4; //bit28 intpol
	//tmc2130_write(cs, 0x6C, mres, 0x01, 0x00, 0xD3);
	tmc2130_write(cs, 0x6C, mres, 0x01, 0x00, 0xC3);
}

void tmc2130_PWMconf(uint8_t cs, uint8_t PWMautoScale, uint8_t PWMfreq, uint8_t PWMgrad, uint8_t PWMampl)
{
	tmc2130_write(cs, 0x70, 0x00, (PWMautoScale+PWMfreq), PWMgrad, PWMampl); // TMC LJ -> For better readability changed to 0x00 and added PWMautoScale and PWMfreq
}

void tmc2130_PWMthreshold(uint8_t cs)
{
	tmc2130_write(cs, 0x13, 0x00, 0x00, 0x00, 0x00); // TMC LJ -> Adds possibility to swtich from stealthChop to spreadCycle automatically
}

void tmc2130_disable_motor(uint8_t driver)
{
	tmc2130_write(tmc2130_cs[driver], 0x6C, 0, 01, 0, 0);
}

#endif //HAVE_TMC2130_DRIVERS
