#include "cuboid.h"

#include <stdbool.h>
#include <stdio.h>

int set_cuboid_data(Cuboid* cuboid, double edge_a, double edge_b, double edge_c) {

    if (edge_a <= 0 || edge_b <= 0 || edge_c <= 0) {
        printf("Hiba! Az elhossz csak pozitiv szam lehet!");
        return 1;
    }
    else {
        cuboid->edge_a = edge_a;
        cuboid->edge_b = edge_b;
        cuboid->edge_c = edge_c;
        return 0;
    }
}

double get_cuboid_surface_area(const Cuboid* cuboid) {

    return 2 * (cuboid->edge_a * cuboid->edge_b + cuboid->edge_a * cuboid->edge_c + cuboid->edge_b * cuboid->edge_c);
}

double get_cuboid_volume(const Cuboid* cuboid) {

    return cuboid->edge_a * cuboid->edge_b * cuboid->edge_c;
}

bool has_square_side(const Cuboid* cuboid) {

    if (cuboid->edge_a == cuboid->edge_b || cuboid->edge_a == cuboid->edge_c || cuboid->edge_b == cuboid->edge_c) {
        return true;
    }
    else {
        return false;
    }
}
