#include "util.h"

#include <math.h>

// Define constants for pi and degree-to-radian conversion
#define DEG_TO_RAD(angle) ((angle) * M_PI / 180.0)

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

#if 0 
int main() {
    // Define input values: altitude, azimuth, rectangle dimensions and position
    double altitude = 30.0;      // Altitude in degrees
    double azimuth = 45.0;       // Azimuth in degrees
    double width = 4.0;          // Width of the rectangle in meters
    double height = 2.0;         // Height of the rectangle in meters

    // Define the origin point (0, 0, 0) and direction vector from spherical coordinates
    Vector3D origin = {0.0, 0.0, 0.0}; // Origin in meters
    Vector3D direction = sphericalToCartesian(altitude, azimuth);

    // Define the rectangle plane
    Vector3D rectangleCenter = {5.0, 5.0, 5.0};     // A point on the rectangle in meters
    Vector3D planeNormal = {0.0, 0.0, 1.0};         // Rectangle normal (assumed perpendicular to z-axis)
    Vector3D u = {1.0, 0.0, 0.0};                   // Vector representing width direction in meters
    Vector3D v = {0.0, 1.0, 0.0};                   // Vector representing height direction in meters

    // Find intersection point
    Vector3D intersection;
    if (linePlaneIntersection(origin, direction, rectangleCenter, planeNormal, &intersection)) {
        double rel_x, rel_y;
        if (pointInRectangle(intersection, rectangleCenter, u, v, width, height, &rel_x, &rel_y)) {
            printf("Intersection at rectangle relative coordinates: (%.2f, %.2f)\n", rel_x, rel_y);
        } else {
            printf("Intersection point is outside rectangle bounds.\n");
        }
    } else {
        printf("No intersection with the rectangle plane.\n");
    }

    return 0;
}
#endif
