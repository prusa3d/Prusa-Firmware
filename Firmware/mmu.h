//mmu.h

#include <inttypes.h>

extern bool mmu_enabled;
extern bool mmuFSensorLoading;
extern bool mmu_fil_loaded;
extern bool mmu_idl_sens;
extern uint32_t mmu_last_request;
extern uint32_t mmu_last_finda_response;
extern uint8_t mmu_filament_types[];

extern bool mmu_enabled;
extern bool ir_sensor_detected;
extern uint8_t mmu_extruder;
extern uint8_t tmp_extruder;
extern int8_t mmu_finda;
extern int16_t mmu_version;
extern int16_t mmu_buildnr;
extern uint16_t mmu_power_failures;

#define MMU_FILAMENT_UNKNOWN 255

#define MMU_LOAD_FEEDRATE 19.02f //mm/s
#define MMU_LOAD_TIME_MS 2000 //should be fine tuned to load time for shortest allowed PTFE tubing and maximum loading speed
#define isEXTLoaded (PIN_GET(IR_SENSOR_PIN) == 0)

enum class MmuCmd : uint_least8_t
{
    None,
    T0,
    T1,
    T2,
    T3,
    T4,
    L0,
    L1,
    L2,
    L3,
    L4,
    C0,
    U0,
    E0,
    E1,
    E2,
    E3,
    E4,
    F0,
    F1,
    F2,
    F3,
    F4,
    K0,
    K1,
    K2,
    K3,
    K4,
    R0,
    S3,
    W0 //!< Wait and signal load error
};

inline MmuCmd operator+ (MmuCmd cmd, uint8_t filament)
{
    return static_cast<MmuCmd>(static_cast<uint8_t>(cmd) + filament );
}

inline uint8_t operator- (MmuCmd cmda, MmuCmd cmdb)
{
    return (static_cast<uint8_t>(cmda) - static_cast<uint8_t>(cmdb));
}

extern void mmu_init(void);
extern void mmu_loop(void);
extern void mmu_reset(void);
extern bool check_for_ir_sensor();
extern void mmu_set_filament_type(uint8_t extruder, uint8_t filament);
extern void mmu_command(MmuCmd cmd);
extern bool mmu_get_response(uint8_t move);
extern void manage_response(bool move_axes, bool turn_off_nozzle); //, uint8_t move = MMU_NO_MOVE);
extern void mmu_load_to_nozzle();
extern void mmu_M600_load_filament(bool automatic, float nozzle_temp);
extern void mmu_wait_for_heater_blocking();
extern void mmu_M600_wait_and_beep();
extern void extr_mov(float shift, float feed_rate);
extern void extr_adj(uint8_t extruder);
extern void extr_unload();
extern void load_all();
extern void extr_unload_();

extern void mmu_filament_ramming();
extern void mmu_load_step(bool synchronize = true);
extern void mmu_continue_loading(void);

extern bool mmu_check_version();
extern void mmu_show_warning();
extern void lcd_mmu_load_to_nozzle(uint8_t filament_nr);
extern void mmu_eject_filament(uint8_t filament, bool recover);
