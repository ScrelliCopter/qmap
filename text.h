#ifndef INC_TEXT_H
#define INC_TEXT_H

#define ISO_CHAR_MIN    0x00
#define ISO_CHAR_MAX    0xFF
#define ISO_CHAR_WIDTH  8
#define ISO_CHAR_HEIGHT 16
extern const unsigned char iso_font[256*16];

#define TEXT_SCALE 1

extern void draw_text(int x, int y, const char *text);

#endif
