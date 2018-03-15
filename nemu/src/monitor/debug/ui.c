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
static int cmd_x(char *args);
static int cmd_p(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si","Continue the program with N commands,si N",cmd_si},
  {"info","Print the program status，r:register,w:watchpoint",cmd_info} , 
  {"x","Scan the memory,x N address length(default as 4)",cmd_x },
  {"p","evaluate the expression,p expr",cmd_p}
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
	if (arg ==NULL||atoi(arg)==0) {
    cpu_exec(1);
    }
    else if(atoi(arg)== -1){
        cpu_exec(-1);
        return 0;
    }
    else if(atoi(arg)<-1){
        printf("Unkonwn command %s",arg);
        return 0;
    }
	else {
    int N=1;
    N=atoi(arg);
    cpu_exec(N);
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
                int j;
                for(j=0;j<8;j++){
                    printf("%s: 0x%x  %d\n",regsl[j],reg_l(j),reg_l(j));
                }
                printf("eip: 0x%x %d\n",cpu.eip,cpu.eip);
                return 0;
            }
            default:{
                printf("No argument given , use r or w\n");
                return 0;
            }
        }
    }
    return 0;
}

static int cmd_x(char *args){

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
    int len=4,one=1;
    char *arg3=strtok(NULL," ");
    if(arg3!=NULL){      //use arg3 to control the len
        len=atoi(arg3);
        one=0;
    }
        if(len>4){
            printf("Too long size\n");
            return 0;
        }
        int N=atoi(arg);
        int adr[N];
        arg2=arg2+2;   //skip the ch "0x"

        int *expr = (int *)malloc(sizeof(int));
        sscanf(arg2,"%x",expr);//argument save
        
        int i=0,j=0;
        for (i=0;i<N;i++){
            adr[i]=paddr_read(*expr+i*len,len);
            printf("%x   0x%0*x   ",*expr+i*len,len*2,adr[i]);  //four byte print
            for(j=0;j<len;j++){
                if (one == 0) break;
                printf("0x%0*x ",one*2,paddr_read(*expr+i*len+j,one)); // one byte print
            }
            printf("\n");
        }
        return 0;
    
return 0;
}

//static bool make_token(char *e);
static int cmd_p(char *args){
   // char *arg=strtok(NULL," ");
    if(args==NULL){

        printf("No expression given\n");
        return 0;
    }
    else{
        bool succeed;
        printf("%d\n",expr(args,&succeed));
        
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
