#ifndef CUBOID_H
#define CUBOID_H

#include <stdbool.h>

/**
 * Data of a 3D cuboid shape.
 */
typedef struct Cuboid {

	double edge_a;
	double edge_b;
	double edge_c;
} Cuboid;

/**
 * Set the data of the cuboid.
 */
int set_cuboid_data(Cuboid* cuboid, double edge_a, double edge_b, double edge_c);

/**
 * Calculate the surface area of the cuboid.
 */
double get_cuboid_surface_area(const Cuboid* cuboid);

/**
 * Calculate the volume of the cuboid.
 */
double get_cuboid_volume(const Cuboid* cuboid);

/**
 * Check whether the cuboid has a square side.
 */
bool has_square_side(const Cuboid* cuboid);

#endif // CUBOID_H
