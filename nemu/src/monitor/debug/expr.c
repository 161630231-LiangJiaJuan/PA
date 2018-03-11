#include "nemu.h"
#include <stdlib.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,TK_MU,TK_DI,TK_PL,TK_SUB,TK_LPA,TK_RPA,TK_NUM    
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
  {"[1-9][0-9]{0,3}", TK_NUM},         // subtraction
  {"\\(", TK_LPA},         // subtraction
  {"\\)", TK_RPA},         // subtraction
  {"==", TK_EQ}         // equal
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

bool check_parentheses(int p,int q){
    if (p>q||q>nr_token){
        Log("Bad expression in check_parentheses\n");
        return false;
    }
    int i=0,success=0;
    for(i=p;i<=q;i++){
        if(tokens[i].type==TK_LPA){
            success++;
        }
        if(tokens[i].type==TK_RPA){
            success--;
        }
    }
    if(success==0)
        return true;
    else
        return false;
}

int dom_op(int p,int q){
    int i;
    int result=0,op=0;
    if(p>q)  assert(0);
    for(i=p;i<=q;i++){
        if(tokens[i].type==TK_LPA){
            while(tokens[i].type!=TK_RPA){
                i++;
            }
            continue;
        }//end if ,to skip the parentheses
        else if(tokens[i].type<TK_MU||tokens[i].type>TK_SUB){
            continue;
        } //end if, to skip the not opretor
        
        else if ( tokens[i].type>=TK_PL || result==0 ){
            result=tokens[i].type;
            op=i;
            continue;
        }
        else if(tokens[i].type<=TK_DI && result <= TK_DI ){
            result=tokens[i].type;
            op=i;
            continue;
        }
    }
    return op;
}

int eval(int p,int q){
    if(p>q){
        Log("Bad expression\n");
        assert(0);
    }
    else if(p==q){
        return atoi(tokens[p].str);
    }
   // else if(check_parentheses(p,q)== true){
        //return eval(p+1,q-1);
   // }
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
            default : assert(0);
        }
    }
    return 0;
}
uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    assert(0);
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  else {
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
