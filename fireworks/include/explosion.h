#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "fireworks.h"
#include "spark.h"

typedef void (*ExplosionLogic)(Firework* firework, Spark* spark);

void init_explosion(Firework* firework, int spark_count, Color color);
void explode_comet(Firework* firework, Spark* spark);
void explode_crossette(Firework* firework, Spark* spark);
void explode_peony(Firework* firework, Spark* spark);
void explode_ring(Firework* firework, Spark* spark);
void explode_willow(Firework* firework, Spark* spark);
void explode_palm(Firework* firework, Spark* spark);
void explode_fish(Firework* firework, Spark* spark);
void explode_strobe(Firework* firework, Spark* spark);
void explode_ghost(Firework* firework, Spark* spark);
void explode_tourbillion(Firework* firework, Spark* spark);
void explode_nishiki_kamuro(Firework* firework, Spark* spark);
void explode_chrysanthemum(Firework* firework, Spark* spark);

#endif // EXPLOSION_H
