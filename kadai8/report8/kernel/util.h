#include "hardware.h"
#define FONT_HEIGHT 8
extern unsigned char font[128][FONT_HEIGHT];
static struct FrameBuffer *FB;
static unsigned int fb_y;
static unsigned fb_x;

void init_frame_buffer(struct FrameBuffer *fb);
void puts(char* str);
void puth(unsigned long long value,unsigned char digits_len);
unsigned int compare(const char *first, const char *second, unsigned int bytes);