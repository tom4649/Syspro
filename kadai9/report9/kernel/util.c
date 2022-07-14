#include "util.h"
#include "hardware.h"
extern unsigned char font[128][FONT_HEIGHT];
static struct FrameBuffer *FB;
static unsigned int fb_y;
static unsigned int fb_x;

void init_frame_buffer(struct FrameBuffer *fb){
    FB=fb;
    for (unsigned int i = 0; i < FB->height; i++) {
    for (unsigned int j = 0; j < FB->width; j++) {
      struct Pixel *pixel = FB->base + FB->width * i + j;
      pixel->r = 0;
      pixel->g = 0;
      pixel->b = 0;
    }
  }
  fb_x=0;
  fb_y=0;
}

void puts(char* str){
    int x =0;
    while(str[x]!='\0'){
        char pos =str[x];
        if(pos=='\n'){
            fb_x=0;
            fb_y+=8;
            if(fb_y+8>FB->height){
                init_frame_buffer(FB);
            }
            x++;
            continue;
        }
        if(fb_x+8>FB->width){
            fb_x=0;
            fb_y+=8;
        }
        int posx=fb_x,posy=fb_y;
        for(int k=0;k<8;k++){
            for(int l=7;l>=0;l--){
                if(((font[pos][k]>>l)&1)==0){
                    posx++;
                    continue;
                }
                struct Pixel *pixel = FB->base + FB->width * posy+ posx;
                pixel->r = 255;
                pixel->g = 255;
                pixel->b = 255;
                posx++;
            }
            posx=fb_x;
            posy++;
        }
        x++;
        fb_x+=8;
    }
}

void puth(unsigned long long value,unsigned char digits_len){
    if(digits_len==0)return;
    unsigned long long num;
    unsigned long long id=digits_len;
    char pos;
    while(id>0){
        num=value;
        for(unsigned int i=0;i<id-1;i++)num/=16;
        num%=16;
        if(num>9){
            pos = (char)num+65-10;
        }
        else pos = (char) num+48;
        if(fb_x+8>FB->width){
            fb_x=0;
            fb_y+=8;
            if(fb_y+8>FB->height){
                init_frame_buffer(FB);
            }
        }
        int posx=fb_x,posy=fb_y;
        for(int k=0;k<8;k++){
            for(int l=7;l>=0;l--){
                if(((font[pos][k]>>l)&1)==0){
                    posx++;
                    continue;
                }
                struct Pixel *pixel = FB->base + FB->width * posy+ posx;
                pixel->r = 255;
                pixel->g = 255;
                pixel->b = 255;
                posx++;
            }
            posx=fb_x;
            posy++;
        }
        fb_x+=8;
        id--;
    }
}

unsigned int compare(const char *first, const char *second, unsigned int bytes) {
  const char *f = first, *s = second;
  for (unsigned int i = 0; i < bytes; i++) {
    if (f[i] != s[i]) return 0;
  }
  return 1;
}
