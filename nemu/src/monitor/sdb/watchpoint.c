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

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  // struct watchpoint *prev;
  char* expr;
  word_t value;
  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
static int wpnum = 0;
void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
    // wp_pool[i].prev = (i == 0 ? NULL : &wp_pool[i - 1]);
    //最后一个节点的next是NULL,其余是下一个
  }
  free_ = wp_pool;
}
WP* new_wp()//完成对链表的操作，其中的值和expr不改变
{
  if(wpnum == NR_WP)//满了
  {
    printf("watchpoint pool is full\n");
    return NULL;
  }
    
  else//改变head，free
  {
    if(wpnum == 0)//一开始head = null
    {
      head = free_;
    }
    wpnum++;//wp计数器
    WP* newwp = free_;
    free_ = free_ -> next;//更新free
    return newwp;
  }
  
}
void print_wp()
{
  if(wpnum == 0)
    return ;
  printf("Num   Expression          value");
}
bool check_wp()
{
  if(wpnum == 0)
    return 1;

  WP* i = head;
  bool state = 1;
  word_t value;
  while(i != free_){
    state = 1;
    value = expr(i->expr , &state);
    if(state == 0){
      printf("calculating expression failed while checking watchpoint\n");
      return 0;
    }
    if(i -> value != value)
    {
      printf("watchpoint %d  %s\n",i -> NO, i -> expr);
      printf("Old value %u\n",i -> value);
      printf("New value %u\n",value);
      i -> value = value;
      return 0;
    }
    i = i -> next;
  }
  return 1;
  
}
void free_wp(int id)
{
  if(wpnum == 0)
  {
    printf("no watchpoint\n");
    return ;
  }
  WP* iter = head;
  while(iter -> NO != id)
  {
    iter = iter -> next;
    if(iter == NULL)
    {
      printf("Watchpoint notfound\n");
      return ;
    }
  }
  WP* wp = iter;
  iter = head;
  if(wp == head)
  {
    if(wpnum == 1)
    {
      free_ = head;
      head = NULL;
      
    }
    else
    {
      WP* nexthead = head -> next;
      head -> next = free_ -> next;
      free_ -> next = head;
      head = nexthead;
    }
  }
  else
  {
    while(iter -> next != wp)
      iter = iter->next;
    iter -> next = wp -> next;
    wp -> next = free_ -> next;
    free_ -> next = wp;
  }
  wpnum--;
  printf("Deleted\n");
}
// extern void add_wp(char* expression){
  
//   bool success = 1;
//   word_t res = expr(expression,&success);
  
//   if(success == 0){
//     printf("failed expression analysis\n");
//   }
//   if(free_ -> next == NULL){
//     printf("error no space for watchpoint \n");
//   }
//   if(head == NULL){
//     head = free_;
    
//   }
 

//   free_ -> expr = expression;
//   free_ -> value = res;
//   free_ = free_-> next;
// }
// /* TODO: Implement the functionality of watchpoint */
// extern void del_wp(int idx){
//   if( wp_pool[idx - 1].prev){
//     wp_pool[idx - 1].prev -> next = wp_pool[idx - 1].next;
//     wp_pool[idx - 1].next = free_;
//     free_->prev -> next = &wp_pool[idx - 1];
//     free_ = & wp_pool[idx - 1];//没有加aasert
//   }
//   else{
    
//   }
  
//   return ;
// }
