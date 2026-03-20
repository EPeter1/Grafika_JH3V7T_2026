#include "cuboid.h"

#include <stdbool.h>
#include <stdio.h>

int main() {

    Cuboid cuboid;

    if (set_cuboid_data(&cuboid, 2, 3, 4) == 0) {
        printf("Teglatest felszine: %.2lf\n", get_cuboid_surface_area(&cuboid));
        printf("Teglatest terfogata: %.2lf\n\n", get_cuboid_volume(&cuboid));
    
        if (has_square_side(&cuboid) == true) {
            printf("A teglatestnek van negyzetes oldala.");
        }
        else {
            printf("A teglatestnek nincs negyzetes oldala.");
        }
    }

    return 0;
}
