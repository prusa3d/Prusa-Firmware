#pragma once

/// Assigns host name with up to two characters which will be shown on
/// the UI when printing. The function forces the third byte to be null delimiter.
void SetHostStatusScreenName(const char * name);

/// Returns a pointer to the host name
char * GetHostStatusScreenName();

/// Reset the memory to NULL when the host name should not be used
void ResetHostStatusScreenName();

/// Restart the M79 timer
void M79_timer_restart();

/// Get the current M79 timer status
/// @returns true if running, false otherwise
bool M79_timer_get_status();

/// Checks if the timer period has expired. If the timer
/// has expired, the timer is stopped
void M79_timer_update_status();
