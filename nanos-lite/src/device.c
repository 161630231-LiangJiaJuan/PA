#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

void _draw_rect(const uint32_t *pixels,int x,int y,int w,int h);
int _read_key();
unsigned long _uptime();

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
  int key= _read_key();
  if (key==_KEY_NONE){
      sprintf((char *)buf,"t %s\n",_uptime());
      return strlen((char *)buf)-1;
  }
  else{
    
    if(key>0x8000){
        key=key-0x8000;
        sprintf((char *)buf , "kd %s\n",keyname[key]);
    }
    else{
        sprintf((char *)buf, "ku %s\n",keyname[key]);
    
    }
    return strlen((char *)buf)-1;
  }
  return 0;
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
    strncpy(buf,dispinfo+offset,len);
}

void fb_write(const void *buf, off_t offset, size_t len) {
    offset=offset/4;
    int y=offset/400;
    int x=offset%(400);
    _draw_rect(buf,x,y,len,len);
}

void init_device() {
  _ioe_init();
    strcpy(dispinfo,"WIDTH : 400\nHEIGHT:300\n");
    Log("dispinfo %s",dispinfo);
  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}
