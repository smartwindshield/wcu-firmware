#include "solar.h"

#include "HAL_Debug.h"

#include "HAL_Time.h"
#include "SolTrack.h"
#include "sensors/sensors_controller.h"
#include "sensors/barometer.h"
#include "sensors/external_temp.h"
#include "comms/bluetooth.h"
#include "util.h"

#include <math.h>

// Define constants for pi and degree-to-radian conversion
#define DEG_TO_RAD(angle) ((angle) * M_PI / 180.0)

// Function to compute the cross product of two vectors
static Vector3D crossProduct(Vector3D a, Vector3D b) {
    Vector3D result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

// Function to rotate a vector around the Y-axis by a given angle (in degrees)
Vector3D rotateAroundY(Vector3D normal, float angleDegrees) {
    // Convert angle to radians
    float angleRadians = angleDegrees * (M_PI / 180.0f);
    
    // Calculate the cosine and sine of the angle
    float cosAngle = cos(angleRadians);
    float sinAngle = sin(angleRadians);
    
    // Apply the rotation matrix for rotating around the Y-axis
    Vector3D rotatedNormal;
    rotatedNormal.x = normal.x * cosAngle + normal.z * sinAngle;
    rotatedNormal.y = normal.y;  // Y-component remains the same
    rotatedNormal.z = -normal.x * sinAngle + normal.z * cosAngle;
    
    return rotatedNormal;
}

// Function to convert spherical coordinates (altitude, azimuth) to a direction vector
static Vector3D sphericalToCartesian(double altitude, double azimuth) {
    Vector3D direction;
    direction.x = cos(DEG_TO_RAD(altitude)) * cos(DEG_TO_RAD(azimuth));
    direction.y = cos(DEG_TO_RAD(altitude)) * sin(DEG_TO_RAD(azimuth));
    direction.z = sin(DEG_TO_RAD(altitude));
    return direction;
}

// Function to calculate intersection point of line and plane
static int linePlaneIntersection(Vector3D origin, Vector3D direction, Vector3D planePoint, Vector3D planeNormal, Vector3D *intersection) {
    double denom = direction.x * planeNormal.x + direction.y * planeNormal.y + direction.z * planeNormal.z;
    
    if (fabs(denom) < 1e-6) return 0; // No intersection
    
    Vector3D diff = {planePoint.x - origin.x, planePoint.y - origin.y, planePoint.z - origin.z};
    double t = (diff.x * planeNormal.x + diff.y * planeNormal.y + diff.z * planeNormal.z) / denom;
    
    intersection->x = origin.x + t * direction.x;
    intersection->y = origin.y + t * direction.y;
    intersection->z = origin.z + t * direction.z;
    return 1; // Intersection found
}

// Function to check if intersection is within rectangle bounds and get relative x, y
static int pointInRectangle(Vector3D intersection, Vector3D rectangleCenter, Vector3D u, Vector3D v, double width, double height, double *rel_x, double *rel_y) {
    Vector3D d = {intersection.x - rectangleCenter.x, intersection.y - rectangleCenter.y, intersection.z - rectangleCenter.z};
    double local_x = d.x * u.x + d.y * u.y + d.z * u.z;
    double local_y = d.x * v.x + d.y * v.y + d.z * v.z;
    
    if (fabs(local_x) <= width / 2 && fabs(local_y) <= height / 2) {
        *rel_x = local_x / (width / 2);
        *rel_y = local_y / (height / 2);
        return 1; // Point is within rectangle
    }
    return 0; // Point is outside rectangle
}


static void CalculateSunPosition(LocationData currLocation, double *relativeAltitude, double *relativeAzimuth) {
    struct Time time;
    struct Position pos;
    struct Location loc;

    int useDegrees = 1;             // Input (geographic position) and output are in degrees
    int useNorthEqualsZero = 1;     // Azimuth: 0 = South, pi/2 (90deg) = West  ->  0 = North, pi/2 (90deg) = East
    int computeRefrEquatorial = 0;  // Compure refraction-corrected equatorial coordinates (Hour angle, declination): 0-no, 1-yes
    int computeDistance = 1;        // Compute the distance to the Sun in AU: 0-no, 1-yes

    DatetimeData datetime = SensorsController_GetDatetimeData();

    time.year = datetime.year;
    time.month = datetime.month;
    time.day = datetime.day;
    time.hour = datetime.hour;
    time.minute = datetime.minute;
    time.second = datetime.second;

    loc.longitude = currLocation.longitude;
    loc.latitude  = currLocation.latitude;
    loc.pressure = SensorsController_GetPressureKpa();
    loc.temperature = SensorsController_GetTemperatureKelvin();
    
    // Compute positions:
    SolTrack(time, loc, &pos, useDegrees, useNorthEqualsZero, computeRefrEquatorial, computeDistance);
#if 0
    HAL_Debug_Printf("[Solar]: Ecliptic longitude, latitude:        %10.6lf° %10.6lf°\n", pos.longitude, 0.0);
    HAL_Debug_Printf("[Solar]: Right ascension, declination:        %10.6lf° %10.6lf°\n", pos.rightAscension, pos.declination);
    HAL_Debug_Printf("[Solar]: Uncorrected altitude:                            %10.6lf°\n\n", pos.altitude);
    HAL_Debug_Printf("[Solar]: Corrected azimuth, altitude:         %10.6lf° %10.6lf°\n", pos.azimuthRefract, pos.altitudeRefract);
    HAL_Debug_Printf("[Solar]: Corected hour angle, declination:    %10.6lf° %10.6lf°\n\n", pos.hourAngleRefract, pos.declinationRefract);
#endif
    // Azimuth and Altitude of sun relative to user instead of north
    double usrAzimuth = currLocation.yaw; // The 'Azimuth' of the user's view relative to north. (Yaw)
    double usrAltitude = currLocation.pitch; // The 'Altitude' of the user's view relative to the horizon. (Pitch)

    *relativeAzimuth = pos.azimuthRefract - usrAzimuth;
    *relativeAltitude = pos.altitudeRefract - usrAltitude;

    HAL_Debug_Printf("[Solar]: Sun azimuth Refract, altitude refract: %f %f\n",
                     pos.azimuthRefract, pos.altitudeRefract);
    HAL_Debug_Printf("[Solar]: Sun relative azimuth, altitude: %f %f\n",
                    *relativeAzimuth, *relativeAltitude);
}

bool Solar_GetWindshieldRelativeIntersectionPoint(Vector2D *intersection) {
    bool inBounds = false;
    double relativeAltitude;
    double relativeAzimuth;

    // Define the origin point (0, 0, 0) and direction vector from spherical coordinates
    Vector3D origin = {0.0, 0.0, 0.0}; // Origin in meters
    Vector3D direction;
    Vector3D planeIntersection;

    LocationData currLocation = SensorsController_GetLocationData();

    Vector3D distanceToWindshield = BluetoothComms_GetUserWindshieldDistance();
    double windshieldHeight = BluetoothComms_GetWindshieldHeight();
    double windshieldLength = BluetoothComms_GetWindshieldLength();
    double windshieldAngleRotate = -60;

    // TODO: swap X,Y due to differences with visualizer and ublox? idk yet
    
    // Define the rectanglular plane (windshield)
    // To account for the user's orientation (heading), we need to rotate the center point
    // Instead of a rotation matrix, we can represent the center in spherical coordinates
    // which would be (distanceToWindshield, 0, heading)
    // TODO: Make this work for the user not being exactly dead center of the windshield
    Vector3D rectangleCenter = sphericalToCartesian(0, currLocation.yaw);
    // Multiply in the "rho" value (distance to the windshield)
    rectangleCenter.x *= distanceToWindshield.x;
    rectangleCenter.y *= distanceToWindshield.x;
    rectangleCenter.z *= distanceToWindshield.x;

    //Vector3D rectangleCenter = {5, 5, 0.0};

    // Rectangle normal (assumed perpendicular to z-axis)
    // Same as the rectangeCenter point except it is negated to point in the opposite direction
    // (and no "rho" multiplication) to normalize it
    Vector3D planeNormal = sphericalToCartesian(0, currLocation.yaw);
    planeNormal.x *= -1;
    planeNormal.y *= -1;
    planeNormal.z *= -1;

    planeNormal = rotateAroundY(planeNormal, windshieldAngleRotate);

    /*
     * This vector determines the width direction.
     * The width direction is perpendicular to the windshield's heading,
     * favoring positive X as well
     */
    Vector3D u = sphericalToCartesian(0, (currLocation.yaw > 180 ? currLocation.yaw - 90 : currLocation.yaw + 90));

    // TODO: implement windshield angle using similar principle with this vector
    Vector3D v = crossProduct(u, planeNormal);                   // Vector representing height direction in meters
   
    HAL_Debug_Printf("[Solar]: Windshield Center is (X,Y,Z): %f %f %f, Heading %F\n",
                     rectangleCenter.x, rectangleCenter.y, rectangleCenter.z,
                     currLocation.yaw);

    // TODO: Cache SolTrack data so this doesn't have to run every time
    // Run SolTrack
    CalculateSunPosition(currLocation, &relativeAltitude, &relativeAzimuth);
    //relativeAltitude = 10;
    //relativeAzimuth = 15;

    direction = sphericalToCartesian(relativeAltitude, relativeAzimuth);
 
    // Find intersection point
    if (linePlaneIntersection(origin, direction, rectangleCenter, planeNormal,
                              &planeIntersection)) {
        if (pointInRectangle(planeIntersection, rectangleCenter, u, v,
                             windshieldLength,
                             windshieldHeight,
                             &intersection->x, &intersection->y)) {
            HAL_Debug_Printf("[Solar]: Intersection at real coordinates: %f %f %f\n", planeIntersection.x, planeIntersection.y, planeIntersection.z);
            HAL_Debug_Printf("[Solar]: Intersection at rectangle relative coordinates: (%.2f, %.2f)\n",
                             intersection->x, intersection->y);

            inBounds = true;
        } else {
            HAL_Debug_Printf("[Solar]: Intersection point is outside rectangle bounds, at %f %f %f\n",
                    planeIntersection.x, planeIntersection.y, planeIntersection.z);
        }
    } else {
        HAL_Debug_Printf("[Solar]: No intersection with the rectangle plane.\n");
    }

    return inBounds;
}

