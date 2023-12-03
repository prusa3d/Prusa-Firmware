#include <string.h>
#include "Configuration_adv.h"
#include "host.h"
#include "Timer.h"

static LongTimer M79_timer;
static char host_status_screen_name[3];

void SetHostStatusScreenName(const char * name) {
    strncpy(host_status_screen_name, name, 2);
    host_status_screen_name[2] = '\0';
}

char * GetHostStatusScreenName() {
    return host_status_screen_name;
}

void ResetHostStatusScreenName() {
    memset(host_status_screen_name, 0, sizeof(host_status_screen_name));
}

void M79_timer_restart() {
    M79_timer.start();
}

bool M79_timer_get_status() {
    return M79_timer.running();
}

void M79_timer_update_status() {
    M79_timer.expired(M79_TIMEOUT);
}

bool M79_is_host_name_pl() {
    if(strcmp(GetHostStatusScreenName(), "PL") == 0) {
        return true;
    }
    return false;
}
