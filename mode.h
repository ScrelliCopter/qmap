#ifndef INC_MODE_H
#define INC_MODE_H

#include "s.h"

extern void blit(char *src);
extern void set_pal(uchar *pal);
extern void setup_sdl(void);
extern void close_sdl(void);

extern int kbhit();
extern int getch();

#endif
