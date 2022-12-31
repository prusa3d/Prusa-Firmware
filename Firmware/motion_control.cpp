/*
  motion_control.c - high level interface for issuing motion commands
  Part of Grbl

  Copyright (c) 2009-2011 Simen Svale Skogsrud
  Copyright (c) 2011 Sungeun K. Jeon
  Copyright (c) 2020 Brad Hochgesang

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Marlin.h"
#include "stepper.h"
#include "planner.h"

// The arc is approximated by generating a huge number of tiny, linear segments. The length of each 
// segment is configured in settings.mm_per_arc_segment.  
void mc_arc(const float* position, float* target, const float* offset, float feed_rate, float radius, bool isclockwise, uint16_t start_segment_idx)
{
    float start_position[4];
    memcpy(start_position, position, sizeof(start_position));
    
    float r_axis_x = -offset[X_AXIS];  // Radius vector from center to current location
    float r_axis_y = -offset[Y_AXIS];
    float center_axis_x = start_position[X_AXIS] - r_axis_x;
    float center_axis_y = start_position[Y_AXIS] - r_axis_y;
    float travel_z = target[Z_AXIS] - start_position[Z_AXIS];
    float rt_x = target[X_AXIS] - center_axis_x;
    float rt_y = target[Y_AXIS] - center_axis_y;
    // 20200419 - Add a variable that will be used to hold the arc segment length
    float mm_per_arc_segment = cs.mm_per_arc_segment;
    // 20210109 - Add a variable to hold the n_arc_correction value
    unsigned char n_arc_correction = cs.n_arc_correction;

    // CCW angle between start_position and target from circle center. Only one atan2() trig computation required.
    float angular_travel_total = atan2(r_axis_x * rt_y - r_axis_y * rt_x, r_axis_x * rt_x + r_axis_y * rt_y);
    if (angular_travel_total < 0) { angular_travel_total += 2 * M_PI; }

    if (cs.min_arc_segments > 0)
    {
        // 20200417 - FormerLurker - Implement MIN_ARC_SEGMENTS if it is defined - from Marlin 2.0 implementation
        // Do this before converting the angular travel for clockwise rotation
        mm_per_arc_segment = radius * ((2.0f * M_PI) / cs.min_arc_segments);
    }
    if (cs.arc_segments_per_sec > 0)
    {
        // 20200417 - FormerLurker - Implement MIN_ARC_SEGMENTS if it is defined - from Marlin 2.0 implementation
        float mm_per_arc_segment_sec = (feed_rate / 60.0f) * (1.0f / cs.arc_segments_per_sec);
        if (mm_per_arc_segment_sec < mm_per_arc_segment)
            mm_per_arc_segment = mm_per_arc_segment_sec;
    }

    // Note:  no need to check to see if min_mm_per_arc_segment is enabled or not (i.e. = 0), since mm_per_arc_segment can never be below 0.
    if (mm_per_arc_segment < cs.min_mm_per_arc_segment)
    {
        // 20200417 - FormerLurker - Implement MIN_MM_PER_ARC_SEGMENT if it is defined
        // This prevents a very high number of segments from being generated for curves of a short radius
        mm_per_arc_segment = cs.min_mm_per_arc_segment;
    }
    else if (mm_per_arc_segment > cs.mm_per_arc_segment) {
        // 20210113 - This can be implemented in an else if since  we can't be below the min AND above the max at the same time.
        // 20200417 - FormerLurker - Implement MIN_MM_PER_ARC_SEGMENT if it is defined
        mm_per_arc_segment = cs.mm_per_arc_segment;
    }

    // Adjust the angular travel if the direction is clockwise
    if (isclockwise) { angular_travel_total -= 2 * M_PI; }

    //20141002:full circle for G03 did not work, e.g. G03 X80 Y80 I20 J0 F2000 is giving an Angle of zero so head is not moving
    //to compensate when start pos = target pos && angle is zero -> angle = 2Pi
    if (start_position[X_AXIS] == target[X_AXIS] && start_position[Y_AXIS] == target[Y_AXIS] && angular_travel_total == 0)
    {
        angular_travel_total += 2 * M_PI;
    }
    //end fix G03

    // 20200417 - FormerLurker - rename millimeters_of_travel to millimeters_of_travel_arc to better describe what we are
    // calculating here
    const float millimeters_of_travel_arc = hypot(angular_travel_total * radius, fabs(travel_z));
    if (millimeters_of_travel_arc < 0.001) { return; }
    
    // Calculate the number of arc segments
    unsigned short segments = static_cast<unsigned short>(ceil(millimeters_of_travel_arc / mm_per_arc_segment));

    /* Vector rotation by transformation matrix: r is the original vector, r_T is the rotated vector,
       and phi is the angle of rotation. Based on the solution approach by Jens Geisler.
           r_T = [cos(phi) -sin(phi);
                  sin(phi)  cos(phi] * r ;

       For arc generation, the center of the circle is the axis of rotation and the radius vector is
       defined from the circle center to the initial position. Each line segment is formed by successive
       vector rotations. This requires only two cos() and sin() computations to form the rotation
       matrix for the duration of the entire arc. Error may accumulate from numerical round-off, since
       all double numbers are single precision on the Arduino. (True double precision will not have
       round off issues for CNC applications.) Single precision error can accumulate to be greater than
       tool precision in some cases. Therefore, arc path correction is implemented.

       The small angle approximation was removed because of excessive errors for small circles (perhaps unique to
       3d printing applications, causing significant path deviation and extrusion issues).
       Now there will be no corrections applied, but an accurate initial sin and cos will be calculated.
       This seems to work with a very high degree of accuracy and results in much simpler code.

       Finding a faster way to approximate sin, knowing that there can be substantial deviations from the true
       arc when using the previous approximation, would be beneficial.
    */

    // If there is only one segment, no need to do a bunch of work since this is a straight line!
    if (segments > 1 && start_segment_idx)
    {
        // Calculate theta per segments, and linear (z) travel per segment, e travel per segment
        // as well as the small angle approximation for sin and cos.
        const float theta_per_segment = angular_travel_total / segments,
            linear_per_segment = travel_z / (segments),
            segment_extruder_travel = (target[E_AXIS] - start_position[E_AXIS]) / (segments),
            sq_theta_per_segment = theta_per_segment * theta_per_segment,
            sin_T = theta_per_segment - sq_theta_per_segment * theta_per_segment / 6,
            cos_T = 1 - 0.5f * sq_theta_per_segment;
        // Loop through all but one of the segments.  The last one can be done simply
        // by moving to the target.
        for (uint16_t i = 1; i < segments; i++) {
            if (n_arc_correction-- == 0) {
                // Calculate the actual position for r_axis_x and r_axis_y
                const float cos_Ti = cos(i * theta_per_segment), sin_Ti = sin(i * theta_per_segment);
                r_axis_x = -offset[X_AXIS] * cos_Ti + offset[Y_AXIS] * sin_Ti;
                r_axis_y = -offset[X_AXIS] * sin_Ti - offset[Y_AXIS] * cos_Ti;
                // reset n_arc_correction
                n_arc_correction = cs.n_arc_correction;
            }
            else {
                // Calculate X and Y using the small angle approximation
                const float r_axisi = r_axis_x * sin_T + r_axis_y * cos_T;
                r_axis_x = r_axis_x * cos_T - r_axis_y * sin_T;
                r_axis_y = r_axisi;
            }

            // Update Position
            start_position[X_AXIS] = center_axis_x + r_axis_x;
            start_position[Y_AXIS] = center_axis_y + r_axis_y;
            start_position[Z_AXIS] += linear_per_segment;
            start_position[E_AXIS] += segment_extruder_travel;
            // Clamp to the calculated position.
            clamp_to_software_endstops(start_position);
            // Insert the segment into the buffer
            if (i >= start_segment_idx)
                plan_buffer_line(start_position[X_AXIS], start_position[Y_AXIS], start_position[Z_AXIS], start_position[E_AXIS], feed_rate, position, i);
            // Handle the situation where the planner is aborted hard.
            if (planner_aborted)
                return;
        }
    }
    // Clamp to the target position.
    clamp_to_software_endstops(target);
    // Ensure last segment arrives at target location.
    plan_buffer_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], target[E_AXIS], feed_rate, position, 0);
}
