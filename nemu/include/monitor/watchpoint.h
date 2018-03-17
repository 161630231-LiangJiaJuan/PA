#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char expr[32];
  /* TODO: Add more members if necessary */


} WP;

extern void set_wp(char *arg);
extern void list_wp();
#endif
