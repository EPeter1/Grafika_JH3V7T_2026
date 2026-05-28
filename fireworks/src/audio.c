#include "audio.h"

#include "utils.h"
#include "vec3.h"

#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_stdinc.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_VARIATIONS 10

typedef struct SoundGroup {
    Mix_Chunk* chunks[MAX_VARIATIONS];
    int count;
} SoundGroup;

static SoundGroup sound_registry[SOUND_TYPE_COUNT];

static void load_sound_group(SoundGroup* group, const char* paths[]) {
    group->count = 0;

    for (int i = 0; paths[i] != NULL && i < MAX_VARIATIONS; i++) {
        group->chunks[i] = Mix_LoadWAV(paths[i]);

        if (group->chunks[i] == NULL) {
            fprintf(stderr, "[ERROR] Failed to load sound '%s': %s\n", paths[i], Mix_GetError());
        }
        else {
            group->count++;
        }
    }
}

void init_audio_mixer() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "[ERROR] SDL Mixer initialization error: %s\n", Mix_GetError());
        return;
    }

    const char* bang_sounds[] = {
        "assets/sounds/bang_1.wav",
        "assets/sounds/bang_2.wav",
        "assets/sounds/bang_3.wav",
        "assets/sounds/bang_4.wav",
        "assets/sounds/bang_5.wav",
        "assets/sounds/bang_6.wav",
        "assets/sounds/bang_7.wav",
        "assets/sounds/bang_8.wav",
        "assets/sounds/bang_9.wav",
        "assets/sounds/bang_10.wav",
        NULL
    };

    const char* fizzle_sounds[] = {
        "assets/sounds/fizzle_1.wav",
        "assets/sounds/fizzle_2.wav",
        "assets/sounds/fizzle_3.wav",
        "assets/sounds/fizzle_4.wav",
        "assets/sounds/fizzle_5.wav",
        "assets/sounds/fizzle_6.wav",
        NULL
    };

    const char* whistle_sounds[] = {
        "assets/sounds/whistle_1.wav",
        "assets/sounds/whistle_2.wav",
        "assets/sounds/whistle_3.wav",
        NULL
    };

    load_sound_group(&sound_registry[SOUND_BANG], bang_sounds);
    load_sound_group(&sound_registry[SOUND_FIZZLE], fizzle_sounds);
    load_sound_group(&sound_registry[SOUND_WHISTLE], whistle_sounds);
}

void set_audio_volume(float volume) {
    int sdl_volume = (int)(volume * MIX_MAX_VOLUME);
    Mix_Volume(-1, sdl_volume);
}

void play_firework_sound(SoundType type, vec3 sound_position, vec3 camera_position) {
    if (type >= SOUND_TYPE_COUNT) {
        return;
    }

    SoundGroup* group = &sound_registry[type];
    if (group->count == 0) {
        return;
    }

    int index = rand() % group->count;
    Mix_Chunk* random_chunk = group->chunks[index];

    int channel = Mix_PlayChannel(-1, random_chunk, 0);
    if (channel == -1) {
        return;
    }

    vec3 difference = subtract_vec3(sound_position, camera_position);
    float distance = get_vec3_length(difference);

    float max_distance = 6.0f;
    int sdl_distance = (int)clamp((distance / max_distance) * 255.0f, 0.0f, 255.0f);

    float angle_radian = atan2f(difference.y, difference.x);
    int angle_degree = (int)radian_to_degree(angle_radian);
    int sdl_angle = (90 - angle_degree + 360) % 360;

    Mix_SetPosition(channel, (Uint16)sdl_angle, (Uint8)sdl_distance);
}

void destroy_audio_mixer() {
    for (int i = 0; i < SOUND_TYPE_COUNT; i++) {
        for (int j = 0; j < sound_registry[i].count; j++) {
            Mix_FreeChunk(sound_registry[i].chunks[j]);
        }
    }

    Mix_CloseAudio();
}
