CC = gcc

CFLAGS = \
	-Iinclude/ \
	-DGLEW_STATIC \
	-D_USE_MATH_DEFINES

LDFLAGS = \
	-Llib/ \
	-lglew32 \
	-lmingw32 \
	-lSDL2_image \
	-lSDL2_mixer \
	-lSDL2_ttf \
	-lSDL2main \
	-lSDL2 \
	-lobj \
	-lglu32 \
	-lopengl32 \
	-lm

WFLAGS = \
	-std=c11 \
	-Wall \
	-Wextra \
	-Wpedantic

SRC = \
	src/app.c \
	src/audio.c \
	src/camera.c \
	src/color.c \
	src/event.c \
	src/explosion.c \
	src/firework_renderer.c \
	src/fireworks.c \
	src/gl_state.c \
	src/main.c \
	src/scene.c \
	src/shader.c \
	src/spark.c \
	src/texture.c \
	src/ui.c \
	src/utils.c \
	src/water.c

OUT = fireworks

all:
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) $(WFLAGS) -o $(OUT)
