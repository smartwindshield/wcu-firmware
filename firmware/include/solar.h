#ifndef SOLAR_H
#define SOLAR_H

#include "util.h"

#include <stdbool.h>

/**
 * Runs the SolTrack algorithm using latest data reported from GPS module to
 * get the coordinates of the Sun relative to the windshield
 *
 * @returns True if the Sun is present on the windshield, false if not
 */ 
bool Solar_GetWindshieldRelativeIntersectionPoint(Vector2D *intersection);

#endif // SOLAR_H
