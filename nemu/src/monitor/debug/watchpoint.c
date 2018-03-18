#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = (WP *)malloc(sizeof(WP));
  head->next=NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(){
    WP *cur=NULL,*last=NULL;
    cur=free_->next;
    last=free_;
    while(cur->next!=NULL){
        cur=cur->next;
        last=last->next;
    }
    if(cur!=NULL){
        last->next=NULL;
        return cur;
    }
    else{
        Log("Not enough watchpoint \n");
        assert(0);
        return free_;
    }
}

void free_wp(WP *wp){
    WP *cur=free_;
    while(cur->next!=NULL){
        cur=cur->next;
    }
    cur->next=wp;
    wp->next=NULL;
}

 void del_wp(int num){
    WP *cur=head->next,*last=head;
    if(cur==NULL){
        printf("No watchpoint using\n");
        return;
    }
    else{
        while(cur!=NULL){
            if (cur->NO==num){
                last->next=cur->next;
                free_wp(cur);
                printf("Delete NO.%d\n",num);
                return;
            }
            cur=cur->next;
            last=last->next;
        }
    }
    printf("No this watchpoint\n");
    return ;
}

 void set_wp(char *arg){
    
   WP * last = head;
    while(last->next!=NULL){
        last=last->next;
    }
    last->next=new_wp();
    WP *cur=last->next;
    strcpy(cur->expr,arg);
    cur->next=NULL;
    bool succeed;
    cur->old=expr(arg,&succeed);
    printf("Set watchpoint No.%d\n",cur->NO);
    return;
}

extern void list_wp(){
    WP *cur =head;
    while(cur->next!=NULL){
        cur=cur->next;
        printf("WP No.%d  WP expr:%s  old_val:0x%08x \n ",cur->NO,cur->expr,cur->old);
    }
    return;
}

extern int cmp_val(){
    if (head->next==NULL){
        return 0;
    }
    else{
        WP *cur=head;
        while(cur->next!=NULL){
            cur=cur->next;
            bool succeed;
            int result=expr(cur->expr,&succeed);
            if (result!=cur->old){
                Log("Touch watchpoint No.%d expr:%s old:0x%08x new:0x%08x \n",cur->NO,cur->expr,cur->old,result);
                return 1;
                break;
            }
            cur->old = result;
        }
    }
    return 0;
}
