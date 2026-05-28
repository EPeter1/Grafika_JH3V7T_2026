#ifndef AUDIO_H
#define AUDIO_H

#include "vec3.h"

typedef enum SoundType {
    SOUND_BANG,
    SOUND_FIZZLE,
    SOUND_WHISTLE,
    SOUND_TYPE_COUNT
} SoundType;

void init_audio_mixer();
void set_audio_volume(float volume);
void play_firework_sound(SoundType type, vec3 sound_position, vec3 camera_position);
void destroy_audio_mixer();

#endif // AUDIO_H
