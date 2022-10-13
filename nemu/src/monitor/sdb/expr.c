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

#include <memory/paddr.h>        
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

#define NUM_TOKEN 65536+128
#define INT_DIGIT 50

enum {
  TK_NOTYPE = 256, TK_EQ,TK_NUM,DEREF,TK_NEGATIVE,TK_AND,TK_UEQ,TK_HNUM,TK_RG

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {"0x[A-Fa-f0-9]+",TK_HNUM},//TK_num不能 先于 HNUM
  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // minus
  {"\\*", '*'},         // minus
  {"\\/", '/'},         // minus
  {"\\(", '('},         // minus
  {"\\)", ')'},         // minus
  {"==", TK_EQ},    
  {"!=", TK_UEQ},
  {"&&", TK_AND},     
  {"[0-9]+",TK_NUM},
  {"\\$[\\$A-Za-z0-9]+",TK_RG}
  

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
            }
            tokens[nr_token].type = rules[i].token_type;
            break;
          }
          case TK_RG:
          {
            for(int p=1;p<substr_len;p++)
            {
              tokens[nr_token].str[p-1] = *(substr_start + p);
            }
            tokens[nr_token].type = rules[i].token_type;
            break;
          }
          case TK_HNUM:
          {
            for(int p=2;p<substr_len;p++)
            {
              tokens[nr_token].str[p-2] = *(substr_start + p);
            }
            tokens[nr_token].type = rules[i].token_type;
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
    for (i = 0; i < nr_token; i ++) {
      // if (tokens[i].type == '*' && (i == 0 || (tokens[i - 1].type != TK_NUM && tokens[i - 1].type != ')')) ) {
      //   tokens[i].type = DEREF;
      // }
      // if(idx >= 1 && tokens[idx-1].type == TK_NEGATIVE){
      //   idx--;
      //   if(idx == 1)
      //   tokens[idx].type = '+';
      // }
      if (tokens[i].type == '-' && (i == 0 || (tokens[i - 1].type != TK_NUM && tokens[i - 1].type != TK_HNUM && tokens[i - 1].type != TK_RG && tokens[i - 1].type != ')')) ) {
        tokens[i].type = TK_NEGATIVE;
      }
      if (tokens[i].type == '*' && (i == 0 || (tokens[i - 1].type != TK_NUM && tokens[i - 1].type != TK_HNUM && tokens[i - 1].type != TK_RG && tokens[i - 1].type != ')')) ) {
        tokens[i].type = DEREF;
      }
      
    }
    //untested
  }

  return true;
}

int s[NUM_TOKEN];
int cur = 0;
void push(int num)
{
  // s[cur] = num;
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
word_t singleeval(int p,int q,word_t res,bool* success){
  for(int i=q;i>=p;i--)
  {
    if(tokens[i].type == TK_NEGATIVE){
      res = -res;
    }
    else if(tokens[i].type == DEREF){
      res = paddr_read(res,4);
    }
    else{
      *success = 0;
      printf("not negative and deref detected\n");
    }
  }
  return res;
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
    if(tokens[p].type == TK_NUM ){
      return atoi(tokens[p].str);
    }
    else if(tokens[p].type == TK_HNUM){

      word_t res = 0;
      char c;
      c = tokens[p].str[0];
      if(c >= '0' && c <= '9'){
        res += c - '0';
      }
      else if(c >= 'A' && c <= 'F'){
          res += c - 'A' + 10;
      }
      else{
        res += c - 'a' + 10;
      }   

      for(int i=1;i<strlen(tokens[p].str);i++){
        c = tokens[p].str[i];
        res <<= 4;
        if(c >= '0' && c <= '9'){
          res += c - '0';
        }
        else if(c >= 'A' && c <= 'F'){
          res += c - 'A' + 10;
        }
        else{
          res += c - 'a' + 10;
        }
      }
      return res;
    }
    if(tokens[p].type == TK_RG ){
      if(strcmp(tokens[p].str,"pc") == 0)
      {
        return cpu.pc;
      }
      bool find = 1;
      word_t res = isa_reg_str2val(tokens[p].str,&find);
      
      if(find){
        return res;
      }
      else{
        printf("reg not found\n");
        *success = 0;
        return 0;
      }
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
    if(tokens[q].type == TK_NUM || tokens[q].type == TK_HNUM || tokens[q].type == TK_RG ){
      int flag = 1;
      for(int i=p;i<q;i++){
        if(tokens[i].type != TK_NEGATIVE && tokens[i].type != DEREF ){
          flag = 0;
        }
      }
      if(flag){
        word_t res = eval(q,q,success);
        return singleeval(p,q-1,res,success);
        
      }
    }
    
    int left = p;
    while(left < q - 2 && (tokens[left].type == TK_NEGATIVE || tokens[left].type == DEREF))
    {
      if(check_parentheses(left + 1, q, success) == true){
        word_t res = eval(left + 1, q, success);
        return singleeval(p, left, res, success);
      }
      left++;
    }
      
    
    int op = -1, act = 1,addsub = 0,eq = 0;
    for(int i=q;i>=p;i--)
    {
      if(tokens[i].type == TK_AND){
        if(act){
          op = i;
          break;
        }
      }
      if(tokens[i].type == TK_EQ || tokens[i].type == TK_UEQ){
        if(act && !eq){
          op = i;
          eq = 1;
        }
      }
      else if(tokens[i].type == '+' || tokens[i].type == '-'){
        if( act && !eq && !addsub){
          op = i;
          addsub = 1;
        }
      }
      else if(tokens[i].type == '*' || tokens[i].type == '/'){
        if(op == -1 && act && !addsub && !eq){
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
      case TK_EQ: return val1 == val2;
      case TK_UEQ: return val1 != val2;
      case TK_AND: return val1 && val2;
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
extern word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  clear();
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
