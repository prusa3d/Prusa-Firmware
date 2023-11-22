#include "Configuration_adv.h"
#include "host.h"
#include "Timer.h"

static LongTimer M79_timer;

void M79_timer_restart() {
    M79_timer.start();
}

bool M79_timer_get_status() {
    return M79_timer.running();
}

void M79_timer_update_status() {
    M79_timer.expired(M79_TIMEOUT);
}
