CC ?= cc

TARGET := qmap
SRCS := main.c iso_font.c mode.c clock.c cam.c bsp.c readbsp.c \
        clippoly.c render.c 3d.c tmap3d.c poly.c tm_c.c surface.c

SFLAGS  := -flto -O3 -fno-strict-aliasing
WFLAGS  := -Wall -Wextra -Wno-unused-parameter
CFLAGS  ?= -pipe
BUILD_CFLAGS  := $(WFLAGS) $(SFLAGS) $(CFLAGS) $(shell sdl2-config --cflags)
BUILD_LDFLAGS := $(SFLAGS) $(LDFLAGS) $(shell sdl2-config --libs) -lm


ODIR := .obj_$(CC)_$(TARGET)
OBJS := $(patsubst %.c, $(ODIR)/%.o, $(SRCS))
DEPS := $(OBS:%.o=%.d)

.PHONY: default all clean
default: $(TARGET)
all: $(TARGET)

clean: $(TARGET) $(ODIR)
	rm -rf $^

$(TARGET): $(OBJS)
	$(CC) $(BUILD_LDFLAGS) $^ -o $@

$(ODIR)/%.o: %.c | $(ODIR)
	$(CC) $(BUILD_CFLAGS) -MMD -c $< -o $@

$(ODIR):
	mkdir -p $@

-include $(DEPS)
