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
#include<assert.h>

#define NR_WP 32

//监视点结构
typedef struct watchpoint {
  //监视点编号
  int NO;

  //指向链表中下一个对象
  struct watchpoint *next;
  
  //监视点的表达式，当表达式的值发生变化时停止
  char expr[128];

  //表达式的旧值，如果变化则更新并停止
  word_t value;

} WP;

//初始化链表
static WP wp_pool[NR_WP] = {};

//链表头，可用空间指针
static WP *head = NULL, *free_ = NULL;
//监视点数量
static int wpnum = 0;

//初始化监视点(进行链表的初始化)
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

//新建一个监视点但不初始化，返回新建监视点的指针
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

//打印所有监视点的信息
void print_wp()
{
  if(wpnum == 0){
    printf("No watchpoint\n");
    return ;
  }

  printf("Num   Expression          value\n");
  WP* iter = head;
  while(iter != free_)
  {
    assert(iter != NULL);
    printf("%-3d   %-6s              %-u\n",iter -> NO, iter -> expr , iter -> value);
    iter = iter -> next;
  }
}

//检查监视点的值是否被改变，返回监视点是否被触发
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

//删除编号为id的监视点
void free_wp(int id)
{
  if(wpnum == 0)
  {
    printf("there is no watchpoint\n");
    return ;
  }
  WP* iter = head;
  while(iter -> NO != id)
  {
    iter = iter -> next;
    if(iter == NULL || iter == free_)
    {
      printf("Watchpoint notfound\n");
      return ;
    }
  }
  WP* wp = iter;
  int delid = wp -> NO;
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
  printf("Deleted watchpoint : %d\n", delid);
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
