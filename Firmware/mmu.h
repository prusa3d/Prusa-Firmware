//! @file

#ifndef MMU_H
#define MMU_H

#include <inttypes.h>


extern bool mmu_enabled;
extern bool mmu_fil_loaded;

extern uint8_t mmu_extruder;

extern uint8_t tmp_extruder;

extern int8_t mmu_finda;
extern uint32_t mmu_last_finda_response;
extern bool ir_sensor_detected;

extern int16_t mmu_version;
extern int16_t mmu_buildnr;

extern uint16_t mmu_power_failures;

#define MMU_FILAMENT_UNKNOWN 255

#define MMU_NO_MOVE 0
#define MMU_UNLOAD_MOVE 1
#define MMU_LOAD_MOVE 2
#define MMU_TCODE_MOVE 3

#define MMU_LOAD_FEEDRATE 19.02f //mm/s
#define MMU_LOAD_TIME_MS 2000 //should be fine tuned to load time for shortest allowed PTFE tubing and maximum loading speed

enum class MmuCmd : uint_least8_t
{
    None,
    T0, //!< T0 change to filament 0
    T1, //!< T1 change to filament 1
    T2, //!< T2 change to filament 2
    T3, //!< T3 change to filament 3
    T4, //!< T4 change to filament 4
    L0, //!< Load filament 0
    L1, //!< Load filament 1
    L2, //!< Load filament 2
    L3, //!< Load filament 3
    L4, //!< Load filament 4
    C0, //!< Continue loading current filament (used after T-code)
    U0, //!< Unload filament
    E0, //!< Eject filament 0
    E1, //!< Eject filament 1
    E2, //!< Eject filament 2
    E3, //!< Eject filament 3
    E4, //!< Eject filament 4
    K0, //!< Cut filament 0
    K1, //!< Cut filament 1
    K2, //!< Cut filament 2
    K3, //!< Cut filament 3
    K4, //!< Cut filament 4
    R0, //!< Recover after eject filament
    S3, //!< Power failures request
    W0, //!< Wait for user click
};

// Other MMU commands not included in the above enum:
// X0 - MMU reset
// P0 - Read finda
// M0 - Set MMU to normal mode
// M1 - Set MMU to stealth mode
// S0 - return 'ok'
// S1 - Read firmware version
// S2 - Read firmware build number
// F<nr.> \<type\> filament type. <nr.> filament number, \<type\> 0, 1 or 2. Does nothing.

inline MmuCmd operator+ (MmuCmd cmd, uint8_t filament)
{
    return static_cast<MmuCmd>(static_cast<uint8_t>(cmd) + filament );
}

inline uint8_t operator- (MmuCmd cmda, MmuCmd cmdb)
{
    return (static_cast<uint8_t>(cmda) - static_cast<uint8_t>(cmdb));
}

extern int mmu_puts_P(const char* str);

extern int mmu_printf_P(const char* format, ...);

extern int8_t mmu_rx_ok(void);

extern bool check_for_ir_sensor();

extern void mmu_init(void);

extern void mmu_loop(void);


extern void mmu_reset(void);

extern int8_t mmu_set_filament_type(uint8_t extruder, uint8_t filament);

extern void mmu_command(MmuCmd cmd);

extern bool mmu_get_response(uint8_t move = 0);

extern void manage_response(bool move_axes, bool turn_off_nozzle, uint8_t move = MMU_NO_MOVE);

extern void mmu_load_to_nozzle();

extern void mmu_M600_load_filament(bool automatic, float nozzle_temp);
extern void mmu_M600_wait_and_beep();

extern void extr_mov(float shift, float feed_rate);
extern void change_extr(int extr);
extern int get_ext_nr();
extern void display_loading();
extern void extr_adj(uint8_t extruder);
extern void extr_unload();

extern void extr_adj_0();
extern void extr_adj_1();
extern void extr_adj_2();
extern void extr_adj_3();
extern void extr_adj_4();
extern void load_all();
extern void extr_change_0();
extern void extr_change_1();
extern void extr_change_2();
extern void extr_change_3();
#ifdef SNMM
extern void extr_unload_all();
extern void extr_unload_used();
#endif //SNMM
extern void extr_unload_0();
extern void extr_unload_1();
extern void extr_unload_2();
extern void extr_unload_3();
extern void extr_unload_4();

extern bool mmu_check_version();
extern void mmu_show_warning();
extern void lcd_mmu_load_to_nozzle(uint8_t filament_nr);
extern void mmu_eject_filament(uint8_t filament, bool recover);
#ifdef MMU_HAS_CUTTER
extern void mmu_cut_filament(uint8_t filament_nr);
#endif //MMU_HAS_CUTTER
extern void mmu_continue_loading(bool blocking);
extern void mmu_filament_ramming();
extern void mmu_wait_for_heater_blocking();
extern void mmu_load_step(bool synchronize = true);

#endif //MMU_H
