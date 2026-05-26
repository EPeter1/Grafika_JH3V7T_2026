#include "explosion.h"

#include "color.h"
#include "firework_pattern.h"
#include "fireworks.h"
#include "spark.h"
#include "utils.h"
#include "vec3.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static const ExplosionLogic explosion_table[PATTERN_COUNT] = {
    [PATTERN_CHRYSANTHEMUM] = explode_chrysanthemum,
    [PATTERN_COMET] = explode_comet,
    [PATTERN_CROSSETTE] = explode_crossette,
    [PATTERN_FISH] = explode_fish,
    [PATTERN_GHOST] = explode_ghost,
    [PATTERN_NISHIKI_KAMURO] = explode_nishiki_kamuro,
    [PATTERN_PALM] = explode_palm,
    [PATTERN_PEONY] = explode_peony,
    [PATTERN_RING] = explode_ring,
    [PATTERN_STROBE] = explode_strobe,
    [PATTERN_TOURBILLION] = explode_tourbillion,
    [PATTERN_WILLOW] = explode_willow
};

void init_explosion(Firework* firework, int spark_count, Color color) {
    for (int i = 0; i < MAX_SPARKS; i++) {
        firework->sparks[i].current_life = 0.0f;
    }

    if (firework->pattern >= PATTERN_COUNT || explosion_table[firework->pattern] == NULL) {
        fprintf(stderr, "[ERROR] Explosion table is invalid!");
        exit(1);
    }

    for (int i = 0; i < spark_count && i < MAX_SPARKS; i++) {
        Spark* spark = &firework->sparks[i];
        Trail* trail = &firework->trails[i];

        spark->position = firework->position;
        set_spark_life(spark, 1.0f);

        spark->is_leader = false;
        spark->color = color;

        spark->trail_index = i;
        trail->pointer = 0;
        trail->timer = 0.0f;

        for(int j = 0; j < MAX_HISTORY; j++) {
            trail->history[j] = spark->position;
        }

        explosion_table[firework->pattern](firework, spark);

        if (trail->length < 1 || trail->length > MAX_HISTORY) {
            fprintf(stderr, "[ERROR] Invalid trail length (%d) for pattern %d at spark %d.\n",
                    trail->length, firework->pattern, i);
            exit(1);
        }
    }
}

void explode_chrysanthemum(Firework* firework, Spark* spark) {
    int index = get_spark_index(firework, spark);

    if (index == 0) {
        spark->position = firework->position;
        spark->speed = (vec3){0, 0, 0};

        set_spark_life(spark, 2.0f);

        firework->trails[index].length = 1;
        spark->is_leader = false;
    }
    else {
        vec3 direction = get_spherical_direction();
        float speed = 0.7f;
        spark->speed = scale_vec3(direction, speed);

        set_spark_life(spark, 1.5f);
        firework->trails[index].length = MAX_HISTORY;
        spark->is_leader = true;
    }
}

void explode_comet(Firework* firework, Spark* spark) {
    int index = get_spark_index(firework, spark);
    int branch_count = 8;
    int branch_id = index % branch_count;

    int sparks_per_branch = get_physics_config(firework->pattern).spark_count / branch_count;
    spark->is_leader = (index % sparks_per_branch == 0);

    float angle = (float)branch_id * (2.0f * M_PI / (float)branch_count);   
    float spread = 0.5f;

    vec3 branch_direction = {
        cosf(angle) * spread,
        sinf(angle) * spread,
        1.0f
    };

    vec3 direction = get_cone_direction(branch_direction, 0.02f);

    float speed = rand_range(0.6f, 1.0f);
    spark->speed = scale_vec3(add_vec3(firework->speed, direction), speed);

    firework->trails[index].length = MAX_HISTORY;

    float life = spark->is_leader ? 2.0f : rand_range(1.0f, 1.5f);
    set_spark_life(spark, life);
}

void explode_crossette(Firework* firework, Spark* spark) {
    int index = get_spark_index(firework, spark);

    srand((unsigned int)(firework->position.x * 1000));

    vec3 v1 = get_spherical_direction();

    vec3 temp = {0, 1, 0};
    if (fabsf(v1.y) > 0.9f) {
        temp = (vec3){1, 0, 0};
    }

    vec3 v2 = normalize_vec3(cross_vec3(v1, temp));
    vec3 v3 = cross_vec3(v1, v2);

    vec3 directions[14];

    directions[0] = v1;  directions[1] = scale_vec3(v1, -1.0f);
    directions[2] = v2;  directions[3] = scale_vec3(v2, -1.0f);
    directions[4] = v3;  directions[5] = scale_vec3(v3, -1.0f);

    directions[6]  = normalize_vec3(add_vec3(add_vec3(v1, v2), v3));
    directions[7]  = normalize_vec3(add_vec3(add_vec3(v1, v2), scale_vec3(v3, -1.0f)));
    directions[8]  = normalize_vec3(add_vec3(add_vec3(v1, scale_vec3(v2, -1.0f)), v3));
    directions[9]  = normalize_vec3(add_vec3(add_vec3(v1, scale_vec3(v2, -1.0f)), scale_vec3(v3, -1.0f)));
    directions[10] = normalize_vec3(add_vec3(add_vec3(scale_vec3(v1, -1.0f), v2), v3));
    directions[11] = normalize_vec3(add_vec3(add_vec3(scale_vec3(v1, -1.0f), v2), scale_vec3(v3, -1.0f)));
    directions[12] = normalize_vec3(add_vec3(add_vec3(scale_vec3(v1, -1.0f), scale_vec3(v2, -1.0f)), v3));
    directions[13] = normalize_vec3(add_vec3(add_vec3(scale_vec3(v1, -1.0f), scale_vec3(v2, -1.0f)), scale_vec3(v3, -1.0f)));

    int branch_count = 8 + (rand() % 7);
    int direction_index = index % branch_count;

    vec3 target_axis = directions[direction_index];
    vec3 direction = get_cone_direction(target_axis, 0.04f);

    float speed = 0.75f * rand_range(0.9f, 1.1f);
    spark->speed = scale_vec3(direction, speed);

    set_spark_life(spark, 0.6f);

    firework->trails[index].length = 12;
    spark->is_leader = true;
}

void explode_fish(Firework* firework, Spark* spark) {
    vec3 direction = get_spherical_direction();

    float speed = rand_range(0.2f, 0.6f);
    spark->speed = scale_vec3(direction, speed);

    set_spark_life(spark, rand_range(0.6f, 1.0f));

    int index = get_spark_index(firework, spark);
    firework->trails[index].length = 4;
    spark->is_leader = true;
}

void explode_ghost(Firework* firework, Spark* spark) {
    int index = get_spark_index(firework, spark);
    vec3 direction = get_spherical_direction();

    if (index % 4 == 0) {
        float speed = 0.15f;
        spark->speed = scale_vec3(direction, speed);

        set_spark_life(spark, 3.0f);
        spark->is_leader = true;
        firework->trails[index].length = 1;
    }
    else {
        float speed = 0.5f;
        spark->speed = scale_vec3(direction, speed);

        set_spark_life(spark, 4.0f);
        spark->is_leader = false;
        firework->trails[index].length = 2;
    }
}

void explode_nishiki_kamuro(Firework* firework, Spark* spark) {
    vec3 direction = get_spherical_direction();
    direction.z -= 0.3f;
    direction = normalize_vec3(direction);

    float speed = rand_range(0.2f, 0.4f);
    spark->speed = scale_vec3(direction, speed);

    set_spark_life(spark, rand_range(4.0f, 7.0f));

    int index = get_spark_index(firework, spark);
    firework->trails[index].length = MAX_HISTORY;
    spark->is_leader = true;
}

void explode_palm(Firework* firework, Spark* spark) {
    int index = get_spark_index(firework, spark);
    int branch_count = 12; 
    int branch_id = index % branch_count;

    float random_seed = sinf(firework->position.x * 12.9898f + firework->position.y * 78.233f) * 43758.5453f;
    float offset = random_seed - floorf(random_seed);
    float angle_offset = offset * 2.0f * M_PI;

    float ratio = (float)branch_id / (float)branch_count;
    float phi = acosf(2.0f * ratio - 1.0f);
    float theta = (2.0f * M_PI * ratio) + angle_offset;

    vec3 branch_axis = {
        sinf(phi) * cosf(theta),
        sinf(phi) * sinf(theta),
        cosf(phi)
    };

    vec3 direction = get_cone_direction(branch_axis, 0.02f);

    float speed = rand_range(0.3f, 0.7f);
    spark->speed = scale_vec3(direction, speed);

    set_spark_life(spark, rand_range(1.5f, 2.5f));

    firework->trails[index].length = MAX_HISTORY;
    spark->is_leader = true;
}

void explode_peony(Firework* firework, Spark* spark) {
    vec3 direction = get_spherical_direction();

    float base_speed = 0.3f;
    float speed = base_speed * rand_range(0.98f, 1.02f);
    spark->speed = scale_vec3(direction, speed);

    set_spark_life(spark, 3.0f);

    int index = get_spark_index(firework, spark);
    firework->trails[index].length = 2;
}

void explode_ring(Firework* firework, Spark* spark) {
    int index = get_spark_index(firework, spark);

    if (index % 3 == 0) {
        vec3 direction = get_spherical_direction();

        float speed = rand_range(0.01f, 0.2f);
        spark->speed = scale_vec3(direction, speed);

        set_spark_life(spark, rand_range(0.8f, 1.2f));

        firework->trails[index].length = 2;
        spark->is_leader = false;
    }
    else {
        float angle = rand_range(0.0f, 2.0f * M_PI);

        vec3 direction = {
            cosf(angle),
            sinf(angle),
            0.0f
        };

        direction.x += rand_symmetric(0.02f);
        direction.y += rand_symmetric(0.02f);
        direction.z += rand_symmetric(0.05f);

        direction = normalize_vec3(direction);

        float speed = 0.5f; 
        spark->speed = scale_vec3(direction, speed);

        set_spark_life(spark, 2.0f);

        firework->trails[index].length = 10;
        spark->is_leader = true;
    }
}

void explode_strobe(Firework* firework, Spark* spark) {
    vec3 direction = get_spherical_direction();
    
    float speed = rand_range(0.2f, 0.6f);
    spark->speed = scale_vec3(direction, speed);

    set_spark_life(spark, rand_range(1.5f, 2.5f));

    int index = get_spark_index(firework, spark);
    firework->trails[index].length = 2;
    spark->is_leader = true;
}

void explode_tourbillion(Firework* firework, Spark* spark) {
    int index = get_spark_index(firework, spark);

    vec3 direction = get_spherical_direction();
    float speed = rand_range(0.1f, 0.3f);
    spark->speed = scale_vec3(direction, speed);

    float angle = index * 1.0f;
    float spin_frequency = 1.0f;

    vec3 start_movement = {
        sinf(angle) * spin_frequency,
        cosf(angle) * spin_frequency,
        0.2f
    };

    spark->speed = add_vec3(spark->speed, start_movement);

    set_spark_life(spark, rand_range(3.0f, 4.0f));

    firework->trails[index].length = MAX_HISTORY;
    spark->is_leader = true;
}

void explode_willow(Firework* firework, Spark* spark) {
    vec3 up_axis = { 0.0f, 0.0f, 1.0f };
    float spread = 0.6f;
    vec3 direction = get_cone_direction(up_axis, spread);

    float speed = rand_range(0.8f, 1.5f);
    spark->speed = scale_vec3(direction, speed);

    set_spark_life(spark, rand_range(5.0f, 8.0f));

    int index = get_spark_index(firework, spark);
    firework->trails[index].length = MAX_HISTORY;
    spark->is_leader = true;
}
