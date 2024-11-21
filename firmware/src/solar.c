#include "solar.h"

#include "HAL_Debug.h"

#include "SolTrack.h"
#include "sensors/barometer.h"
#include "sensors/gps.h"

#include <math.h>

// Define constants for pi and degree-to-radian conversion
#define DEG_TO_RAD(angle) ((angle) * M_PI / 180.0)

static const double WINDSHIELD_WIDTH = 4.0;
static const double WINDSHIELD_HEIGHT = 2.0;

// Function to convert spherical coordinates (altitude, azimuth) to a direction vector
static Vector3D sphericalToCartesian(double altitude, double azimuth) {
    Vector3D direction;
    direction.x = cos(DEG_TO_RAD(altitude)) * cos(DEG_TO_RAD(azimuth));
    direction.y = cos(DEG_TO_RAD(altitude)) * sin(DEG_TO_RAD(azimuth));
    direction.z = sin(DEG_TO_RAD(altitude));
    return direction;
}

// Function to calculate intersection point of line and plane
// Returns 1 if intersection exists, 0 otherwise
static int linePlaneIntersection(Vector3D origin, Vector3D direction,
                                 Vector3D planePoint, Vector3D planeNormal,
                                 Vector3D *intersection) {
    // Calculate dot product of direction and normal
    double denom = direction.x * planeNormal.x + direction.y * planeNormal.y + direction.z * planeNormal.z;
    
    // Check if line is parallel to plane (denom ~ 0)
    if (fabs(denom) < 1e-6) return 0; // No intersection
    
    // Calculate distance t from origin to intersection
    Vector3D diff = {planePoint.x - origin.x, planePoint.y - origin.y, planePoint.z - origin.z};
    double t = (diff.x * planeNormal.x + diff.y * planeNormal.y + diff.z * planeNormal.z) / denom;
    
    // Calculate intersection point
    intersection->x = origin.x + t * direction.x;
    intersection->y = origin.y + t * direction.y;
    intersection->z = origin.z + t * direction.z;
    return 1; // Intersection found
}

// Function to check if intersection is within rectangle bounds and get relative x, y
// rectangleCenter is the rectangle's center, u and v are the side vectors of the rectangle,
// width and height are the rectangle's dimensions in meters.
static int pointInRectangle(Vector3D intersection, Vector3D rectangleCenter,
                            Vector3D u, Vector3D v, double width, double height,
                            double *rel_x, double *rel_y) {
    Vector3D d = {
        intersection.x - rectangleCenter.x,
        intersection.y - rectangleCenter.y,
        intersection.z - rectangleCenter.z
    };
    
    // Project d onto u and v to get local coordinates
    double local_x = d.x * u.x + d.y * u.y + d.z * u.z;
    double local_y = d.x * v.x + d.y * v.y + d.z * v.z;
    
    // Check if the point is within rectangle bounds
    if (fabs(local_x) <= width / 2 && fabs(local_y) <= height / 2) {
        *rel_x = local_x / (width / 2);
        *rel_y = local_y / (height / 2);
        return 1; // Point is within rectangle
    }
    return 0; // Point is outside rectangle
}

static void CalculateSunPosition(double *relativeAltitude, double *relativeAzimuth) {
    struct Time time;
    struct Position pos;
    struct Location loc;

    int useDegrees = 1;             // Input (geographic position) and output are in degrees
    int useNorthEqualsZero = 1;     // Azimuth: 0 = South, pi/2 (90deg) = West  ->  0 = North, pi/2 (90deg) = East
    int computeRefrEquatorial = 0;  // Compure refraction-corrected equatorial coordinates (Hour angle, declination): 0-no, 1-yes
    int computeDistance = 1;        // Compute the distance to the Sun in AU: 0-no, 1-yes

    GPSData gpsData = GPS_GetData();

    // Set (UT!) date and time manually - use the first date from SolTrack_positions.dat:
    time.year = gpsData.year;
    time.month = gpsData.month;
    time.day = gpsData.day;
    time.hour = gpsData.hour;
    time.minute = gpsData.minute;
    time.second = gpsData.second;

    // Latitude and longitude from GPS are reported in 10^-7 degrees
    loc.longitude = gpsData.longitude / (double) pow(10, 7);
    loc.latitude  = gpsData.latitude / (double) pow(10, 7);
    loc.pressure = BarometerGetData() * 10;     // Convert hectopascal to kilopascals
    loc.temperature = 283.0;  // TODO: Atmospheric temperature in K

    // Compute positions:
    SolTrack(time, loc, &pos, useDegrees, useNorthEqualsZero, computeRefrEquatorial, computeDistance);

    HAL_Debug_Printf("[Solar]: Ecliptic longitude, latitude:        %10.6lf° %10.6lf°\n", pos.longitude, 0.0);
    HAL_Debug_Printf("[Solar]: Right ascension, declination:        %10.6lf° %10.6lf°\n", pos.rightAscension, pos.declination);
    HAL_Debug_Printf("[Solar]: Uncorrected altitude:                            %10.6lf°\n\n", pos.altitude);
    HAL_Debug_Printf("[Solar]: Corrected azimuth, altitude:         %10.6lf° %10.6lf°\n", pos.azimuthRefract, pos.altitudeRefract);
    HAL_Debug_Printf("[Solar]: Corected hour angle, declination:    %10.6lf° %10.6lf°\n\n", pos.hourAngleRefract, pos.declinationRefract);

    // Azimuth and Altitude of sun relative to user instead of north
    double usrAzimuth = gpsData.yaw; // The 'Azimuth' of the user's view relative to north. (Yaw)
    double usrAltitude = gpsData.pitch; // The 'Altitude' of the user's view relative to the horizon. (Pitch)

    *relativeAzimuth = pos.azimuthRefract - usrAzimuth;
    *relativeAltitude = pos.altitudeRefract - usrAltitude; 
}

Vector2D Solar_GetWindshieldRelativeIntersectionPoint(void) {
    Vector2D relativeCoords;
    double relativeAltitude;
    double relativeAzimuth;

    // Define the origin point (0, 0, 0) and direction vector from spherical coordinates
    Vector3D origin = {0.0, 0.0, 0.0}; // Origin in meters
    Vector3D direction = sphericalToCartesian(relativeAltitude, relativeAzimuth);

    // Define the rectangle plane
    Vector3D rectangleCenter = {5.0, 5.0, 5.0};     // A point on the rectangle in meters
    Vector3D planeNormal = {0.0, 0.0, 1.0};         // Rectangle normal (assumed perpendicular to z-axis)
    Vector3D u = {1.0, 0.0, 0.0};                   // Vector representing width direction in meters
    Vector3D v = {0.0, 1.0, 0.0};                   // Vector representing height direction in meters

    // Run SolTrack
    CalculateSunPosition(&relativeAltitude, &relativeAzimuth);

    // Find intersection point
    Vector3D intersection;
    if (linePlaneIntersection(origin, direction, rectangleCenter, planeNormal,
                              &intersection)) {
        if (pointInRectangle(intersection, rectangleCenter, u, v,
                             WINDSHIELD_WIDTH, WINDSHIELD_HEIGHT,
                             &relativeCoords.x, &relativeCoords.y)) {
            HAL_Debug_Printf("[Solar]: Intersection at rectangle relative coordinates: (%.2f, %.2f)\n",
                             relativeCoords.x, relativeCoords.y);
        } else {
            HAL_Debug_Printf("[Solar]: Intersection point is outside rectangle bounds.\n");
        }
    } else {
        HAL_Debug_Printf("[Solar]: No intersection with the rectangle plane.\n");
    }

    return relativeCoords;
}

