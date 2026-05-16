#include "fireworks.h"

#include "color.h"
#include "explosion.h"
#include "firework_pattern.h"
#include "scene.h"
#include "spark.h"
#include "utils.h"
#include "vec3.h"

#include <GL/glew.h>

#include <stdbool.h>
#include <stdlib.h>

void launch_firework(Scene* scene, FireworkPattern pattern) {
    for (int i = 0; i < MAX_FIREWORKS; i++) {
        Firework* firework = &scene->fireworks[i];

        if (firework->state == FIREWORK_READY) {

            firework->state = FIREWORK_RISING;
            firework->pattern = pattern;
            firework->generation = 0;

            set_vec3_scalar(&firework->position, 0.0f);

            set_vec3(&firework->speed, 
                rand_range(-0.1f, 0.1f),
                rand_range(-0.1f, 0.1f),
                rand_range(0.6f, 0.9f)
            );

            break;
        }
    }
}

void launch_mini_explosion(Scene* scene, vec3 position, Color color, FireworkPattern pattern, int current_generation) {
    if (current_generation >= 1) {
        return; 
    }

    for (int i = 0; i < MAX_FIREWORKS; i++) {
        Firework* mini = &scene->fireworks[i];

        if (mini->state == FIREWORK_READY) {

            mini->state = FIREWORK_EXPLODED;
            mini->pattern = pattern;
            mini->position = position;
            mini->generation = current_generation + 1;

            init_explosion(mini, 15, color);
            
            break;
        }
    }
}

void update_rising_firework(Firework* firework, float delta_time) {
    firework->position = add_vec3(firework->position, scale_vec3(firework->speed, delta_time));
    firework->speed.z -= 0.4f * delta_time;

    if (firework->speed.z <= 0.0f) {
        int spark_count = get_physics_config(firework->pattern).spark_count;
        Color random_color = get_color((ColorName)(rand() % COLOR_COUNT));

        init_explosion(firework, spark_count, random_color);
        firework->state = FIREWORK_EXPLODED;
    }
}

void update_exploded_firework(Scene* scene, Firework* firework, float delta_time) {
    const PhysicsConfig config = get_physics_config(firework->pattern);
    bool any_alive = false;

    float timer = firework->sparks[0].current_life;

    for (int i = 0; i < MAX_SPARKS; i++) {
        Spark* spark = &firework->sparks[i];

        if (spark->current_life <= 0) {
            continue;
        }
        any_alive = true;

        update_spark_trail(spark, delta_time);

        if (firework->pattern == PATTERN_CROSSETTE && get_spark_progress(spark) > 0.9f) {
            launch_mini_explosion(scene, spark->position, spark->color, PATTERN_CROSSETTE, firework->generation);
            spark->current_life = 0;

            continue;
        }

        apply_spark_physics(spark, config, delta_time);
        apply_pattern_behavior(firework, spark, config, delta_time);

        spark->current_life -= delta_time;
    }

    if (firework->pattern == PATTERN_CHRYSANTHEMUM) {
        if (timer > 0.0f && firework->sparks[0].current_life <= 0.0f) {
            Color parent_color = firework->sparks[1].color;

            for (int k = 0; k < MAX_FIREWORKS; k++) {
                vec3 offset = {
                    rand_symmetric(0.4f),
                    rand_symmetric(0.4f),
                    rand_symmetric(0.4f)
                };

                launch_mini_explosion(scene, add_vec3(firework->sparks[0].position, offset), parent_color, PATTERN_PEONY, firework->generation);
            }
        }
    }

    if (!any_alive) {
        firework->state = FIREWORK_READY;
    }
}

void render_fireworks(const Scene* scene) {
/*
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, scene->texture_id);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glEnable(GL_POINT_SPRITE);
    glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
*/
    for (int i = 0; i < MAX_FIREWORKS; i++) {
        const Firework* firework = &(scene->fireworks[i]);

        if (firework->state == FIREWORK_RISING) {
            glPointSize(5.0f);
            glBegin(GL_POINTS);
                glColor3f(1.0f, 1.0f, 1.0f);
                glVertex3f(firework->position.x, firework->position.y, firework->position.z);
            glEnd();
        }
        else if (firework->state == FIREWORK_EXPLODED) {
            for (int j = 0; j < MAX_SPARKS; j++) {
                if (firework->sparks[j].current_life > 0) {
                    render_spark(firework, &(firework->sparks[j]), scene->particle_intensity);
                }
            }
        }
    }
/*
    glDisable(GL_POINT_SPRITE);
    glDisable(GL_TEXTURE_2D);
*/
}
