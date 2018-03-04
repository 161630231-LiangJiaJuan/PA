#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
//#include "cpu/reg.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_scan(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si","Continue the program with N commands",cmd_si},
  {"info","Print the program status",cmd_info} , 
  {"scan","Scan the memory",cmd_scan },
  /* TODO: Add more commands */
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}
void exec_wrapper(bool);

static int cmd_si(char *args){
	char *arg= strtok(NULL," " );
	int i=1;
	if (arg ==NULL) {
    bool print_flag = i<10;
    exec_wrapper(print_flag);
    }
    else if(atoi(arg)<=0){
        printf("There must be a number more than 0\n");
        return 0;
    }
	else {
    int N=1;
    N=atoi(arg);
       bool print_flag = N<11;
        for (;N>0;N--){
       exec_wrapper(print_flag);
        }
    }	
    return 0;
}  

static int cmd_info(char *args){
    char *arg= strtok(NULL," ");
    if (arg == NULL){
        printf("r , print the register status\n");
        printf("w , print the watchpoint status\n");
        return 0;
    }
    else if (strlen(arg)>1){
        printf("Unknow command %s\n",arg);
        return 0;
    }
    else {
        char key = *arg;
        switch(key){
            case 'r':{
                printf("eax: %d\n",cpu.eax);
                printf("ecx: %d\n",cpu.ecx);
                printf("edx: %d\n",cpu.edx);
                printf("ebx: %d\n",cpu.ebx);
                printf("esp: %d\n",cpu.esp);
                printf("ebp: %d\n",cpu.ebp);
                printf("esi: %d\n",cpu.esi);
                printf("edi: %d\n",cpu.edi);
                return 0;
            }
        }
    }
    return 0;
}

static int cmd_scan(char *args){

    char *arg=strtok(NULL," ");
    if (arg==NULL){
        printf("No argument given\n");
        return 0;
    }
    char *arg2=strtok(NULL," ");
    if (arg2 == NULL){
        printf("No experation given\n");
        return 0;
    }
    else {
        int N=atoi(arg);
        int expr=atoi(arg2);
        int i=0;
        unsigned char *p=(unsigned char *)&expr;
        for (i=0;i<N;i++){
        printf("%x",*(p+i));
            
        }
        return 0;
    }
return 0;
}
void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
