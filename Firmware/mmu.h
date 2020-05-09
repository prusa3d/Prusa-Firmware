//! @file

#ifndef MMU_H
#define MMU_H

#include <inttypes.h>


extern bool mmu_enabled;
extern bool mmu_fil_loaded;

extern uint8_t mmu_extruder;
extern uint8_t mmu_filament_count;

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
    T0,
    T1,
    T2,
    T3,
    T4,
    T5,
    T6,
    T7,
    T8,
    T9,
    T10,
    T11,
    T12,
    T13,
    T14,
    T15,
    T16,
    T17,
    T18,
    T19,
    T20,
    T21,
    T22,
    T23,
    T24,
    T25,
    T26,
    T27,
    T28,
    T29,
    T30,
    T31,
    L0,
    L1,
    L2,
    L3,
    L4,
    L5,
    L6,
    L7,
    L8,
    L9,
    L10,
    L11,
    L12,
    L13,
    L14,
    L15,
    L16,
    L17,
    L18,
    L19,
    L20,
    L21,
    L22,
    L23,
    L24,
    L25,
    L26,
    L27,
    L28,
    L29,
    L30,
    L31,
    C0,
    U0,
    E0,
    E1,
    E2,
    E3,
    E4,
    E5,
    E6,
    E7,
    E8,
    E9,
    E10,
    E11,
    E12,
    E13,
    E14,
    E15,
    E16,
    E17,
    E18,
    E19,
    E20,
    E21,
    E22,
    E23,
    E24,
    E25,
    E26,
    E27,
    E28,
    E29,
    E30,
    E31,
    K0,
    K1,
    K2,
    K3,
    K4,
    K5,
    K6,
    K7,
    K8,
    K9,
    K10,
    K11,
    K12,
    K13,
    K14,
    K15,
    K16,
    K17,
    K18,
    K19,
    K20,
    K21,
    K22,
    K23,
    K24,
    K25,
    K26,
    K27,
    K28,
    K29,
    K30,
    K31,
    R0,
    S3,
    W0, //!< Wait and signal load error
};

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
