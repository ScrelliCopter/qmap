CC ?= cc

TARGET := qmap
SRCS := main.c iso_font.c mode.c clock.c cam.c bsp.c readbsp.c \
        clippoly.c render.c 3d.c tmap3d.c poly.c tm_c.c surface.c

SFLAGS  := -Wall -Wextra -flto -O3 -fno-strict-aliasing
CFLAGS  ?= -pipe $(SFLAGS) $(shell sdl2-config --cflags)
LDFLAGS ?= $(SFLAGS) $(shell sdl2-config --libs) -lm


ODIR := .obj_$(CC)_$(TARGET)
OBJS := $(patsubst %.c, $(ODIR)/%.o, $(SRCS))
DEPS := $(OBS:%.o=%.d)

.PHONY: default all clean
default: $(TARGET)
all: $(TARGET)

clean: $(TARGET) $(ODIR)
	rm -rf $^

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(ODIR)/%.o: %.c | $(ODIR)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

$(ODIR):
	mkdir -p $@

-include $(DEPS)
