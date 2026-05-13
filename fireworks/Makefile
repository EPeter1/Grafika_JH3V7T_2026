CC = gcc

CFLAGS = \
	-Iinclude/ \
	-DGLEW_STATIC

LDFLAGS = \
	-Llib/ \
	-lglew32 \
	-lmingw32 \
	-lSDL2_image \
	-lSDL2main \
	-lSDL2 \
	-lobj \
	-lglu32 \
	-lopengl32 \
	-lm

WFLAGS = \
	-Wall \
	-Wextra \
	-Wpedantic

SRC = \
	src/app.c \
	src/camera.c \
	src/color.c \
	src/explosion.c \
	src/fireworks.c \
	src/main.c \
	src/scene.c \
	src/spark.c \
	src/texture.c \
	src/utils.c

OUT = fireworks

all:
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) $(WFLAGS) -o $(OUT)
