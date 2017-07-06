#include "Marlin.h"

#ifdef HAVE_TMC2130_DRIVERS

#include "tmc2130.h"
#include <SPI.h>

//externals for debuging
extern float current_position[4];
extern void st_get_position_xy(long &x, long &y);

//chipselect pins
uint8_t tmc2130_cs[4] = { X_TMC2130_CS, Y_TMC2130_CS, Z_TMC2130_CS, E0_TMC2130_CS };
//mode
uint8_t tmc2130_mode = TMC2130_MODE_NORMAL;
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
uint8_t sg_thrs_x = TMC2130_SG_THRS_X;
uint8_t sg_thrs_y = TMC2130_SG_THRS_Y;


//TMC2130 registers
#define TMC2130_REG_GCONF      0x00 // 17 bits
#define TMC2130_REG_GSTAT      0x01 // 3 bits
#define TMC2130_REG_IOIN       0x04 // 8+8 bits
#define TMC2130_REG_IHOLD_IRUN 0x10 // 5+5+4 bits
#define TMC2130_REG_TPOWERDOWN 0x11 // 8 bits
#define TMC2130_REG_TSTEP      0x12 // 20 bits
#define TMC2130_REG_TPWMTHRS   0x13 // 20 bits
#define TMC2130_REG_TCOOLTHRS  0x14 // 20 bits
#define TMC2130_REG_THIGH      0x15 // 20 bits
#define TMC2130_REG_XDIRECT    0x2d // 32 bits
#define TMC2130_REG_VDCMIN     0x33 // 23 bits
#define TMC2130_REG_MSLUT0     0x60 // 32 bits
#define TMC2130_REG_MSLUT1     0x61 // 32 bits
#define TMC2130_REG_MSLUT2     0x62 // 32 bits
#define TMC2130_REG_MSLUT3     0x63 // 32 bits
#define TMC2130_REG_MSLUT4     0x64 // 32 bits
#define TMC2130_REG_MSLUT5     0x65 // 32 bits
#define TMC2130_REG_MSLUT6     0x66 // 32 bits
#define TMC2130_REG_MSLUT7     0x67 // 32 bits
#define TMC2130_REG_MSLUTSEL   0x68 // 32 bits
#define TMC2130_REG_MSLUTSTART 0x69 // 8+8 bits
#define TMC2130_REG_MSCNT      0x6a // 10 bits
#define TMC2130_REG_MSCURACT   0x6b // 9+9 bits
#define TMC2130_REG_CHOPCONF   0x6c // 32 bits
#define TMC2130_REG_COOLCONF   0x6d // 25 bits
#define TMC2130_REG_DCCTRL     0x6e // 24 bits
#define TMC2130_REG_DRV_STATUS 0x6f // 32 bits
#define TMC2130_REG_PWMCONF    0x70 // 22 bits
#define TMC2130_REG_PWM_SCALE  0x71 // 8 bits
#define TMC2130_REG_ENCM_CTRL  0x72 // 2 bits
#define TMC2130_REG_LOST_STEPS 0x73 // 20 bits


uint16_t tmc2130_rd_TSTEP(uint8_t cs);
uint16_t tmc2130_rd_DRV_STATUS(uint8_t chipselect);

void tmc2130_wr_CHOPCONF(uint8_t cs, bool extrapolate256 = 0, uint16_t microstep_resolution = 16);
void tmc2130_wr_PWMCONF(uint8_t cs, uint8_t PWMautoScale = TMC2130_PWM_AUTO, uint8_t PWMfreq = TMC2130_PWM_FREQ, uint8_t PWMgrad = TMC2130_PWM_GRAD, uint8_t PWMampl = TMC2130_PWM_AMPL);
void tmc2130_wr_TPWMTHRS(uint8_t cs, uint32_t val32);
void tmc2130_wr_THIGH(uint8_t cs, uint32_t val32);

uint8_t tmc2130_txrx(uint8_t cs, uint8_t addr, uint32_t wval, uint32_t* rval);
uint8_t tmc2130_wr(uint8_t cs, uint8_t addr, uint32_t wval);
uint8_t tmc2130_rd(uint8_t cs, uint8_t addr, uint32_t* rval);



void tmc2130_init()
{
	MYSERIAL.print("tmc2130_init mode=");
	MYSERIAL.println(tmc2130_mode, DEC);
	WRITE(X_TMC2130_CS, HIGH);
	WRITE(Y_TMC2130_CS, HIGH);
	WRITE(Z_TMC2130_CS, HIGH);
	WRITE(E0_TMC2130_CS, HIGH);
	SET_OUTPUT(X_TMC2130_CS);
	SET_OUTPUT(Y_TMC2130_CS);
	SET_OUTPUT(Z_TMC2130_CS);
	SET_OUTPUT(E0_TMC2130_CS);
	SPI.begin();
	for (int i = 0; i < 2; i++) // X Y axes
	{
		tmc2130_wr(tmc2130_cs[i], TMC2130_REG_GCONF, (tmc2130_mode == TMC2130_MODE_SILENT)?0x00000004:0x00000000);
		tmc2130_wr(tmc2130_cs[i], TMC2130_REG_IHOLD_IRUN, 0x000f0000 | ((tmc2130_current_r[i] & 0x1f) << 8) | (tmc2130_current_h[i] & 0x1f));
		tmc2130_wr(tmc2130_cs[i], TMC2130_REG_TPOWERDOWN, 0x00000000);
		tmc2130_wr_PWMCONF(tmc2130_cs[i]); //PWM_CONF //reset default=0x00050480
		tmc2130_wr_TPWMTHRS(tmc2130_cs[i], TMC2130_TPWMTHRS);
		//tmc2130_wr_THIGH(tmc2130_cs[i], TMC2130_THIGH);
		tmc2130_wr_CHOPCONF(tmc2130_cs[i], TMC2130_EXP256_XY, TMC2130_USTEPS_XY);
	}
	for (int i = 2; i < 3; i++) // Z axis
	{
		tmc2130_wr(tmc2130_cs[i], TMC2130_REG_GCONF, (tmc2130_mode == TMC2130_MODE_SILENT)?0x00000004:0x00000000);
		tmc2130_wr(tmc2130_cs[i], TMC2130_REG_IHOLD_IRUN, 0x000f0000 | ((tmc2130_current_r[i] & 0x1f) << 8) | (tmc2130_current_h[i] & 0x1f));
		tmc2130_wr(tmc2130_cs[i], TMC2130_REG_TPOWERDOWN, 0x00000000);
		tmc2130_wr_PWMCONF(tmc2130_cs[i]); //PWM_CONF //reset default=0x00050480
		tmc2130_wr_TPWMTHRS(tmc2130_cs[i], TMC2130_TPWMTHRS);
		//tmc2130_wr_THIGH(tmc2130_cs[i], TMC2130_THIGH);
		tmc2130_wr_CHOPCONF(tmc2130_cs[i], TMC2130_EXP256_Z, TMC2130_USTEPS_Z);
	}
	for (int i = 3; i < 4; i++) // E axis
	{
		tmc2130_wr(tmc2130_cs[i], TMC2130_REG_GCONF, 0x00000004); //GCONF - bit 2 activate stealthChop
		tmc2130_wr(tmc2130_cs[i], TMC2130_REG_IHOLD_IRUN, 0x000f0000 | ((tmc2130_current_r[i] & 0x1f) << 8) | (tmc2130_current_h[i] & 0x1f));
		tmc2130_wr(tmc2130_cs[i], TMC2130_REG_TPOWERDOWN, 0x00000000);
		tmc2130_wr_CHOPCONF(tmc2130_cs[i], TMC2130_EXP256_E, TMC2130_USTEPS_E);
	}
}

bool tmc2130_update_sg()
{
#if (defined(TMC2130_SG_HOMING) && defined(TMC2130_SG_HOMING_SW))
	if ((sg_homing_axis == X_AXIS) || (sg_homing_axis == Y_AXIS))
	{
		uint8_t cs = tmc2130_cs[sg_homing_axis];
		uint16_t tstep = tmc2130_rd_TSTEP(cs);
		if (tstep < TMC2130_TCOOLTHRS)
		{
			if(sg_homing_delay < TMC2130_SG_DELAY) // wait for a few tens microsteps until stallGuard is used //todo: read out microsteps directly, instead of delay counter
				sg_homing_delay++;
			else
			{
				uint16_t sg = tmc2130_rd_DRV_STATUS(cs) & 0x3ff;
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
#endif
	return false;
}

void tmc2130_check_overtemp()
{
	const static char TMC_OVERTEMP_MSG[] PROGMEM = "TMC DRIVER OVERTEMP ";
	uint8_t cs[4] = { X_TMC2130_CS, Y_TMC2130_CS, Z_TMC2130_CS, E0_TMC2130_CS };
	static uint32_t checktime = 0;
	//drivers_disabled[0] = 1; //TEST
	if( millis() - checktime > 1000 )
	{
		//SERIAL_ECHOLNPGM("drv_status:");
		for(int i=0;i<4;i++)
		{
			uint32_t drv_status = 0;
			tmc2130_rd(cs[i], TMC2130_REG_DRV_STATUS, &drv_status);
			//MYSERIAL.print(drv_status);
			//SERIAL_ECHOPGM(" ");
			if (drv_status & ((uint32_t)1<<26))
			{ // BIT 26 - over temp prewarning ~120C (+-20C)
				SERIAL_ERRORRPGM(TMC_OVERTEMP_MSG);
				SERIAL_ECHOLN(i);
				for(int i=0; i < 4; i++)
					tmc2130_wr(tmc2130_cs[i], TMC2130_REG_CHOPCONF, 0x00010000);
				kill(TMC_OVERTEMP_MSG);
			}
		}
		//SERIAL_ECHOLNPGM("");
		checktime = millis();
	}
}

void tmc2130_home_enter(uint8_t axis)
{
	MYSERIAL.print("tmc2130_home_enter ");
	MYSERIAL.println((int)axis);
#ifdef TMC2130_SG_HOMING
	uint8_t cs = tmc2130_cs[axis];
	sg_homing_axis = axis;
	sg_homing_delay = 0;
	tmc2130_axis_stalled[X_AXIS] = false;
	tmc2130_axis_stalled[Y_AXIS] = false;
	//Configuration to spreadCycle
	tmc2130_wr(cs, TMC2130_REG_GCONF, 0x00000000);
	tmc2130_wr(cs, TMC2130_REG_COOLCONF, ((axis == X_AXIS)?sg_thrs_x:sg_thrs_y) << 16);
	tmc2130_wr(cs, TMC2130_REG_TCOOLTHRS, TMC2130_TCOOLTHRS);
#ifndef TMC2130_SG_HOMING_SW
	tmc2130_wr(cs, TMC2130_REG_GCONF, 0x00000080); //stallguard output to DIAG0
#endif
#endif
}

void tmc2130_home_exit()
{
	MYSERIAL.println("tmc2130_home_exit ");
	MYSERIAL.println((int)sg_homing_axis);
#ifdef TMC2130_SG_HOMING
	if ((sg_homing_axis == X_AXIS) || (sg_homing_axis == Y_AXIS))
	{
		if (tmc2130_mode == TMC2130_MODE_SILENT)
			tmc2130_wr(tmc2130_cs[sg_homing_axis], TMC2130_REG_GCONF, 0x00000004); // Configuration back to stealthChop
		else
			tmc2130_wr(tmc2130_cs[sg_homing_axis], TMC2130_REG_GCONF, 0x00000000);
		sg_homing_axis = 0xff;
	}
#endif
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
	tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_IHOLD_IRUN, 0x000f0000 | ((tmc2130_current_r[axis] & 0x1f) << 8) | (tmc2130_current_h[axis] & 0x1f));
}

void tmc2130_set_current_r(uint8_t axis, uint8_t current)
{
	MYSERIAL.print("tmc2130_set_current_r ");
	MYSERIAL.print((int)axis);
	MYSERIAL.print(" ");
	MYSERIAL.println((int)current);
	if (current > 15) current = 15; //current>15 is unsafe
	tmc2130_current_r[axis] = current;
	tmc2130_wr(tmc2130_cs[axis], TMC2130_REG_IHOLD_IRUN, 0x000f0000 | ((tmc2130_current_r[axis] & 0x1f) << 8) | (tmc2130_current_h[axis] & 0x1f));
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

uint16_t tmc2130_rd_TSTEP(uint8_t cs)
{
	uint32_t val32 = 0;
	tmc2130_rd(cs, TMC2130_REG_TSTEP, &val32);
	if (val32 & 0x000f0000) return 0xffff;
	return val32 & 0xffff;
}

uint16_t tmc2130_rd_DRV_STATUS(uint8_t cs)
{
	uint32_t val32 = 0;
	tmc2130_rd(cs, TMC2130_REG_DRV_STATUS, &val32);
	return val32;
}

void tmc2130_wr_CHOPCONF(uint8_t cs, bool extrapolate256, uint16_t microstep_resolution)
{
	uint8_t mres=0b0100;
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
	//tmc2130_write(cs,0x6C,mres,0x01,0x00,0xD3);
//	tmc2130_write(cs,0x6C,mres,0x01,0x00,0xC3);
	tmc2130_wr(cs,TMC2130_REG_CHOPCONF,((uint32_t)mres << 24) | 0x0100C3);
}

void tmc2130_wr_PWMCONF(uint8_t cs, uint8_t PWMautoScale, uint8_t PWMfreq, uint8_t PWMgrad, uint8_t PWMampl)
{
	tmc2130_wr(cs, TMC2130_REG_PWMCONF, ((uint32_t)(PWMautoScale+PWMfreq) << 16) | ((uint32_t)PWMgrad << 8) | PWMampl); // TMC LJ -> For better readability changed to 0x00 and added PWMautoScale and PWMfreq
}

void tmc2130_wr_TPWMTHRS(uint8_t cs, uint32_t val32)
{
	tmc2130_wr(cs, TMC2130_REG_TPWMTHRS, val32);
}

void tmc2130_wr_THIGH(uint8_t cs, uint32_t val32)
{
	tmc2130_wr(cs, TMC2130_REG_THIGH, val32);
}


uint8_t tmc2130_axis_by_cs(uint8_t cs)
{
	switch (cs)
	{
	case X_TMC2130_CS: return 0;
	case Y_TMC2130_CS: return 1;
	case Z_TMC2130_CS: return 2;
	case E0_TMC2130_CS: return 3;
	}
	return -1;
}

uint8_t tmc2130_wr(uint8_t cs, uint8_t addr, uint32_t wval)
{
	uint8_t stat = tmc2130_txrx(cs, addr | 0x80, wval, 0);
#ifdef TMC2130_DEBUG_WR
	MYSERIAL.print("tmc2130_wr(");
	MYSERIAL.print((unsigned char)tmc2130_axis_by_cs(cs), DEC);
	MYSERIAL.print(", 0x");
	MYSERIAL.print((unsigned char)addr, HEX);
	MYSERIAL.print(", 0x");
	MYSERIAL.print((unsigned long)wval, HEX);
	MYSERIAL.print(")=0x");
	MYSERIAL.println((unsigned char)stat, HEX);
#endif //TMC2130_DEBUG_WR
	return stat;
}

uint8_t tmc2130_rd(uint8_t cs, uint8_t addr, uint32_t* rval)
{
	uint32_t val32 = 0;
	uint8_t stat = tmc2130_txrx(cs, addr, 0x00000000, &val32);
	if (rval != 0) *rval = val32;
#ifdef TMC2130_DEBUG_RD
	MYSERIAL.print("tmc2130_rd(");
	MYSERIAL.print((unsigned char)tmc2130_axis_by_cs(cs), DEC);
	MYSERIAL.print(", 0x");
	MYSERIAL.print((unsigned char)addr, HEX);
	MYSERIAL.print(", 0x");
	MYSERIAL.print((unsigned long)val32, HEX);
	MYSERIAL.print(")=0x");
	MYSERIAL.println((unsigned char)stat, HEX);
#endif //TMC2130_DEBUG_RD
	return stat;
}

uint8_t tmc2130_txrx(uint8_t cs, uint8_t addr, uint32_t wval, uint32_t* rval)
{
	//datagram1 - request
	SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE3));
	digitalWrite(cs, LOW);
	SPI.transfer(addr); // address
	SPI.transfer((wval >> 24) & 0xff); // MSB
	SPI.transfer((wval >> 16) & 0xff);
	SPI.transfer((wval >> 8) & 0xff);
	SPI.transfer(wval & 0xff); // LSB
	digitalWrite(cs, HIGH);
	SPI.endTransaction();
	//datagram2 - response
	SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE3));
	digitalWrite(cs, LOW);
	uint8_t stat = SPI.transfer(0); // status
	uint32_t val32 = 0;
	val32 = SPI.transfer(0); // MSB
	val32 = (val32 << 8) | SPI.transfer(0);
	val32 = (val32 << 8) | SPI.transfer(0);
	val32 = (val32 << 8) | SPI.transfer(0); // LSB
	digitalWrite(cs, HIGH);
	SPI.endTransaction();
	if (rval != 0) *rval = val32;
	return stat;
}

#endif //HAVE_TMC2130_DRIVERS
