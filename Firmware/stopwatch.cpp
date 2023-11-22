/**
 *
 * NOTE: this file has been changed in order to compile & run on Prusa-Firmware https://github.com/prusa3d/Prusa-Firmware
 *
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "stopwatch.h"
#include "system_timer.h"

Stopwatch print_job_timer;

#define MS_TO_SEC(N) uint32_t((N)/1000UL)

Stopwatch::State Stopwatch::state;
uint32_t Stopwatch::accumulator;
uint32_t Stopwatch::startTimestamp;
uint32_t Stopwatch::stopTimestamp;

bool Stopwatch::stop() {
  if (isRunning() || isPaused()) {
    state = STOPPED;
    stopTimestamp = _millis();
    return true;
  }
  else return false;
}

bool Stopwatch::pause() {
  if (isRunning()) {
    state = PAUSED;
    stopTimestamp = _millis();
    return true;
  }
  else return false;
}

bool Stopwatch::start() {
  if (!isRunning()) {
    if (isPaused()) accumulator = duration();
    else reset();

    state = RUNNING;
    startTimestamp = _millis();
    return true;
  }
  else return false;
}

void Stopwatch::resume(const uint32_t with_time) {
  reset();
  if ((accumulator = with_time)) state = RUNNING;
}

void Stopwatch::reset() {
  state = STOPPED;
  startTimestamp = 0;
  stopTimestamp = 0;
  accumulator = 0;
}

uint32_t Stopwatch::duration() {
  return accumulator + MS_TO_SEC((isRunning() ? _millis() : stopTimestamp) - startTimestamp);
}

