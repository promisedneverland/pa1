/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

#define NUM_TOKEN 65536+128
#define INT_DIGIT 50

enum {
  TK_NOTYPE = 256, TK_EQ,TK_NUM

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // minus
  {"\\*", '*'},         // minus
  {"\\/", '/'},         // minus
  {"\\(", '('},         // minus
  {"\\)", ')'},         // minus
  {"==", TK_EQ},        // equal
  {"[0-9]+",TK_NUM}
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);//把rule编译，若执行成功返回0
    //结果储存在re
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {//记录token信息
  int type;
  char str[INT_DIGIT];//token子串，需要缓冲溢出处理
} Token;

static Token tokens[NUM_TOKEN] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;//指示已经被识别出的token数目

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    if(nr_token == NUM_TOKEN)
    {
       Log("overflow too much token");
       break;
    }
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        //so是匹配结果的起始位置，eo是结束为止;1位pmatch数组长度，pmatch是struct
        //regexec 返回0是匹配成功
        char *substr_start = e + position;//子串开始的位置
        int substr_len = pmatch.rm_eo;//子串长度
        if(substr_len > INT_DIGIT)
        {
          Log("overflow string length");
          break;
        }
        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //     i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;//更新指向下一个位置

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NUM:
          {
            for(int p=0;p<substr_len;p++)
            {
              tokens[nr_token].str[p] = *(substr_start + p);
              tokens[nr_token].type = rules[i].token_type;
            }
            break;
          }
          case TK_NOTYPE:
          {
            nr_token--;
            break;
          }
          default:
          {
            tokens[nr_token].type = rules[i].token_type;
            break;
          }
        }
        nr_token++;
        
        break;
      }
    }

    if (i == NR_REGEX) {//表达式分析失败，指示位置
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      //第二个position 是width
      return false;
    }
  }

  return true;
}

int s[NUM_TOKEN];
int cur = 0;
void push(int num)
{
  s[cur] = num;
  ++cur;
}
void pop()
{
  --cur;
}
bool empty()
{
  return !cur;
}
void clear()
{
  cur = 0;
}
bool check_parentheses(int p, int q,bool* success)
{
  bool res = 1;
  for(int i=p;i<=q;i++)
  {

    if(tokens[i].type == '(')
    {
      push(1);
    }
    else if(tokens[i].type == ')')
    {
      if(empty())
      {
        printf("parentheses dismatch, ) more\n");
        *success = 0;
        return 0;
      }
      pop();
      if(empty() && i != q)
      {
        res = 0;
      }
    }
    
  }
  if(!empty())
  {
    printf("parentheses dismatch, ( more\n");
    *success = 0;
    return 0;
  }
  if(tokens[p].type == '(' && tokens[q].type == ')' ){
    // for(int i=p+1;i<q;i++){
    //   if(tokens[i].type == ')'){
    //     printf("fake match (%d,%d)\n",p,q);
    //     return 0;
    //   }
    // }
    // return 1;
    return res;
  }
    
  return 0;
}

u_int32_t eval(int p,int q,bool* success) {
  if(*success == 0)
  {
    return 0;
  }
  if (p > q) {
    *success = 0;
    return 0;
    /* Bad expression */
  }
  else if (p == q) {
    if(tokens[p].type == TK_NUM){
      return atoi(tokens[p].str);
    }
    else{
      *success = 0;
      printf("Single token %c but not num \n", tokens[p].type);
      return 0;
    }
  }
  else if (check_parentheses(p, q,success) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    if(*success == 0)
      return 0;
    
    // printf("check parentheses is TRUE (%d,%d)\n",p,q);
    return eval(p + 1, q - 1,success);
  }
  else {
    clear();
    int op = -1, act = 1;
    for(int i=q;i<=p;i--)
    {
      if(tokens[i].type == '+' || tokens[i].type == '-'){
        if(act){
          op = i;
          break;
        }
      }
      else if(tokens[i].type == '*' || tokens[i].type == '/'){
        if(act){
          op = i;
        }
      }
      else if(tokens[i].type == ')'){
        push(1);
        act = 0;
      }
      else if(tokens[i].type == '('){
        pop();
        // printf("cur = %d\n",cur);
        if(cur == 0)
        {
          act = 1;
        }
      }
    }
    if(op == -1){
      printf("op incorrect, didn't update\n");
      *success = 0;
      return 0;
    }
    u_int32_t val1 = eval(p, op - 1,success);
    u_int32_t val2 = eval(op + 1, q,success);
    // printf("op = %d\n",op); 
    switch (tokens[op].type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': 
        if(val2 == 0)
        {
          printf("divide 0\n");
          *success = 0;
          return 0;
        }
        else
          return val1 / val2;
        
      default: 
      {
        printf("type invalid\n");
        assert(0);
      }
    }
  }
  printf("no if matches\n");
  return 0;
}
word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  // printf("%u\n",eval(0,nr_token-1,success));
  word_t result = eval(0,nr_token-1,success);
  for(int i=0;i<NUM_TOKEN;i++)
  {
    for(int j=0;j<INT_DIGIT;j++)
    {
      tokens[i].str[j] = 0;
    }
    tokens[i].type = 0;
  }


  return result;
}
