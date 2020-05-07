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
void mc_arc(float* position, float* target, float* offset, float feed_rate, float radius, uint8_t isclockwise, uint8_t extruder)
{
    // Extract the position to reduce indexing at the cost of a few bytes of mem
    float p_x = position[X_AXIS];
    float p_y = position[Y_AXIS];
    float p_z = position[Z_AXIS];
    float p_e = position[E_AXIS];

    float t_x = target[X_AXIS];
    float t_y = target[Y_AXIS];
    float t_z = target[Z_AXIS];
    float t_e = target[E_AXIS];

    float r_axis_x = -offset[X_AXIS];  // Radius vector from center to current location
    float r_axis_y = -offset[Y_AXIS];
    float center_axis_x = p_x - r_axis_x;
    float center_axis_y = p_y - r_axis_y;
    float travel_z = t_z - p_z;
    float extruder_travel_total = t_e - p_e;

    float rt_x = t_x - center_axis_x;
    float rt_y = t_y - center_axis_y;
    // 20200419 - Add a variable that will be used to hold the arc segment length
    float mm_per_arc_segment = cs.mm_per_arc_segment;

    // CCW angle between position and target from circle center. Only one atan2() trig computation required.
    float angular_travel_total = atan2(r_axis_x * rt_y - r_axis_y * rt_x, r_axis_x * rt_x + r_axis_y * rt_y);
    if (angular_travel_total < 0) { angular_travel_total += 2 * M_PI; }

    bool check_mm_per_arc_segment_max = false;
    if (cs.min_arc_segments > 0)
    {
        // 20200417 - FormerLurker - Implement MIN_ARC_SEGMENTS if it is defined - from Marlin 2.0 implementation
        // Do this before converting the angular travel for clockwise rotation
        mm_per_arc_segment = radius * ((2.0f * M_PI) / cs.min_arc_segments);
        check_mm_per_arc_segment_max = true;
    }

    if (cs.arc_segments_per_sec > 0)
    {
        // 20200417 - FormerLurker - Implement MIN_ARC_SEGMENTS if it is defined - from Marlin 2.0 implementation
        float mm_per_arc_segment_sec = (feed_rate / 60.0f) * (1.0f / cs.arc_segments_per_sec);
        if (mm_per_arc_segment_sec < mm_per_arc_segment)
            mm_per_arc_segment = mm_per_arc_segment_sec;
        check_mm_per_arc_segment_max = true;
    }

    if (cs.min_mm_per_arc_segment > 0)
    {
        check_mm_per_arc_segment_max = true;
        // 20200417 - FormerLurker - Implement MIN_MM_PER_ARC_SEGMENT if it is defined
        // This prevents a very high number of segments from being generated for curves of a short radius
        if (mm_per_arc_segment < cs.min_mm_per_arc_segment)  mm_per_arc_segment = cs.min_mm_per_arc_segment;
    }

    if (check_mm_per_arc_segment_max && mm_per_arc_segment > cs.mm_per_arc_segment) mm_per_arc_segment = cs.mm_per_arc_segment;



    // Adjust the angular travel if the direction is clockwise
    if (isclockwise) { angular_travel_total -= 2 * M_PI; }

    //20141002:full circle for G03 did not work, e.g. G03 X80 Y80 I20 J0 F2000 is giving an Angle of zero so head is not moving
    //to compensate when start pos = target pos && angle is zero -> angle = 2Pi
    if (p_x == t_x && p_y == t_y && angular_travel_total == 0)
    {
        angular_travel_total += 2 * M_PI;
    }
    //end fix G03

    // 20200417 - FormerLurker - rename millimeters_of_travel to millimeters_of_travel_arc to better describe what we are
    // calculating here
    float millimeters_of_travel_arc = hypot(angular_travel_total * radius, fabs(travel_z));
    if (millimeters_of_travel_arc < 0.001) { return; }
    // Calculate the total travel per segment
    // Calculate the number of arc segments
    uint16_t segments = static_cast<uint16_t>(ceil(millimeters_of_travel_arc / mm_per_arc_segment));


    // Calculate theta per segments and linear (z) travel per segment
    float theta_per_segment = angular_travel_total / segments;
    float linear_per_segment = travel_z / (segments);
    // Calculate the extrusion amount per segment
    float segment_extruder_travel = extruder_travel_total / (segments);
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

    // Don't bother calculating cot_T or sin_T if there is only 1 segment.
    if (segments > 1)
    {
        // Initialize the extruder axis

        float cos_T = cos(theta_per_segment);
        float sin_T = sin(theta_per_segment);
        float r_axisi;
        uint16_t i;

        for (i = 1; i < segments; i++) { // Increment (segments-1)
            r_axisi = r_axis_x * sin_T + r_axis_y * cos_T;
            r_axis_x = r_axis_x * cos_T - r_axis_y * sin_T;
            r_axis_y = r_axisi;

            // Update arc_target location
            p_x = center_axis_x + r_axis_x;
            p_y = center_axis_y + r_axis_y;
            p_z += linear_per_segment;
            p_e += segment_extruder_travel;
            // We can't clamp to the target because we are interpolating!  We would need to update a position, clamp to it
            // after updating from calculated values.
            //clamp_to_software_endstops(position);
            plan_buffer_line(p_x, p_y, p_z, p_e, feed_rate, extruder);
        }
    }
    // Ensure last segment arrives at target location.
    // Here we could clamp, but why bother.  We would need to update our current position, clamp to it
    //clamp_to_software_endstops(target);
    plan_buffer_line(t_x, t_y, t_z, t_e, feed_rate, extruder);
}
