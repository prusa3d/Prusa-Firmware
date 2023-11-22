#pragma once

/// Restart the M79 timer
void M79_timer_restart();

/// Get the current M79 timer status
/// @returns true if running, false otherwise
bool M79_timer_get_status();

/// Checks if the timer period has expired. If the timer
/// has expired, the timer is stopped
void M79_timer_update_status();
