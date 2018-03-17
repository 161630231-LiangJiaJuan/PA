#include "nemu.h"
#include <stdlib.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>
int hex_str(char *hex);
enum {
  TK_EQ=0,TK_NOEQ,TK_AND,TK_OR,TK_MU,TK_DI,TK_PL,TK_SUB,TK_LPA,TK_RPA,TK_NUM,TK_HEX_NUM,TK_VAL,TK_REG,TK_NEG,TK_DER,TK_NOTYPE    
  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\*",TK_MU},        //multiplication
  {"\\/",TK_DI},         // division
  {"\\+",TK_PL},         // plus
  {"\\-",TK_SUB},         // subtraction
  {"[0][x][1-9][0-9]*", TK_HEX_NUM},         //HEX number 
  {"[1-9][0-9]*", TK_NUM},         //number 
  {"[a-zA-Z0-9]+",TK_VAL},    //value
  {"[$][e][a-zA-Z]+",TK_REG},    //register
  {"\\(", TK_LPA},         // left parentheses
  {"\\)", TK_RPA},         // right parentheses
  {"==", TK_EQ},         // equal
  {"!=", TK_NOEQ},         //no  equal
  {"&&", TK_AND},         //&&
  {"||", TK_OR}         //||
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE:{
              continue;
          }
          case TK_HEX_NUM:{
              tokens[nr_token].type =  TK_NUM;
              int dec=hex_str(substr_start);
              sprintf(tokens[nr_token].str,"%d",dec);
                
              nr_token++;
              break;
          }
          default:{
                tokens[nr_token].type=rules[i].token_type;
                int j;
                for(j=0;j<substr_len;j++){
                    tokens[nr_token].str[j]=*(substr_start+j);
                }
                nr_token++;
          } ;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  return true;
}

int hex_str(char *hex ){
    int dec=0;
    sscanf(hex+2,"%x",&dec);
    return dec;
}
int der_reg(char *name){
    int i;
    for (i=0;i<7;i++){
        if (strcmp(name+1,regsl[i])==0){
            Log("compare register %s\n",regsl[i]);
            return reg_l(i);
        }
    }
    return 0;
}

int EQ(int val1,int val2 ){
    if (val1==val2){
        return 1;
    }
    else
        return 0;
}
int NOEQ(int val1,int val2){
    if(val1!=val2)
        return 1;
    else
        return 0;
}

bool check_parentheses(int p,int q){
    if (p>q||q>nr_token){
        Log("Bad expression in check_parentheses\n");
        return false;
    }
    int i=0,success=0;
    if (tokens[p].type == TK_LPA){
    for(i=p;i<=q;i++){
        if(tokens[i].type==TK_LPA){
            success++;
        }
        if(tokens[i].type==TK_RPA){
            success--;
        }
        if(success == 0 && i!=q){
            return false;
        }
    }
    }
    else return false;

    if(success==0 && tokens[q].type == TK_RPA){
        Log("True match parentheses in %d\n",q);
        return true;
    }
    else
        return false;
}

int dom_op(int p,int q){
    int i,j;
    int op=0;
    if(p>q)  assert(0);
    for(i=p;i<=q;i++){
        if(tokens[i].type==TK_LPA){
            j=0;
            j++;
            while(j!=0){
                i++;
                if(tokens[i].type == TK_LPA) j++;
                if(tokens[i].type == TK_RPA) j--;
            }
            continue;
        }//end if ,to skip the parentheses

        
        else if(tokens[i].type<TK_EQ||tokens[i].type>TK_SUB){
            continue;
        } //end if, to skip the not opretor
        else if(tokens[i].type >=TK_AND && tokens[i].type <=TK_OR ){
            op=i;
            continue;
        }
        else if(tokens[i].type>=TK_EQ && tokens[i].type<=TK_NOEQ ){
            op=i;
            continue;
        }
        else if ( (tokens[i].type>=TK_PL && tokens[i].type <=TK_SUB )  ){
            op=i;
//        Log("operator : %s\n",tokens[op].str);
            continue;
        }
        else if((tokens[i].type<=TK_DI && tokens[i].type>=TK_MU)  ){
            op=i;
//        Log("operator : %s\n",tokens[op].str);
            continue;
        }
    }
    return op;
}

int eval(int p,int q){
    if (tokens[p].type>=TK_NEG){
        p++;
    }
    if(p>q){
        Log("Bad expression\n");
        assert(0);
    }
    
    else if(p==q){
        if (tokens[p-1].type==TK_NEG)
            return (0-atoi(tokens[p].str));  //return negative
        if (tokens[p-1].type == TK_DER ){
            if (tokens[p].type!=TK_NUM){   
                return vaddr_read(der_reg(tokens[p].str),4); // return dereference register and value
            }
            else 
                return vaddr_read(atoi(tokens[p].str),4); // return dereference  number
        }

        return atoi(tokens[p].str);
    }
   else if(check_parentheses(p,q)== true){
        return eval(p+1,q-1);
    }
    else {
        int op =dom_op(p,q);
        Log("operator : %s\n",tokens[op].str);
        int val1=eval(p,op-1);
        int val2=eval(op+1,q);
        switch(tokens[op].type){
            case TK_MU : return val1*val2;
            case TK_DI : return val1/val2;
            case TK_PL :return val1+val2;
            case TK_SUB : return val1-val2;
            case TK_EQ: return EQ(val1,val2);
            case TK_NOEQ: return NOEQ(val1,val2);
            case TK_AND: return val1*val2;
            case TK_OR: return val1||val2;
            default : assert(0);
        }
    }
    return 0;
}

void init_tokens(){
    int i,j;
    for(i=0;i<32;i++){
        tokens[i].type=0;
        for(j=0;j<32;j++){
            tokens[i].str[j]='\0';
        }
    }
    return ;
}
void negative(){
    int i;
    for (i=0;i<nr_token;i++){
        if (tokens[i].type == TK_SUB && tokens[i-1].type < TK_NUM ){
            tokens[i].type=TK_NEG;
            Log("position : %d  type : %d\n",i,tokens[i].type);
        }
    }
}

void dereference(){
    int i;
    for (i=0;i<nr_token;i++){
        if (tokens[i].type == TK_MU  && tokens[i-1].type < TK_NUM ){
            tokens[i].type=TK_DER;
            Log("position : %d  type : %d\n",i,tokens[i].type);
        }
    }
}

uint32_t expr(char *e, bool *success) {
    init_tokens();// 

  if (!make_token(e)) {
    *success = false;
    assert(0);
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  else {
      negative();
      dereference();
      int i;
      printf("nr_token: %d\n",nr_token);
      for (i=0;i<nr_token;i++){
          printf("%s",tokens[i].str);
      }
      printf("=");
      return eval(0,nr_token-1);
  }

  return 0;
}
