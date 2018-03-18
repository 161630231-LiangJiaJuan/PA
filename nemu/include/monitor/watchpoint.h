#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  int old;
  struct watchpoint *next;
  char expr[32];
  /* TODO: Add more members if necessary */


} WP;

 void set_wp(char *arg);
 void del_wp(int num);
extern void list_wp();
#endif
