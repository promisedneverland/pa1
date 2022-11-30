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

#include "local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>

#define R(i) gpr(i)
#define Mr vaddr_read
#define Mw vaddr_write

bool is_func_start(word_t w);

enum {
  TYPE_I, TYPE_U, TYPE_S,
  TYPE_N,  TYPE_J, TYPE_R, TYPE_B// none
};

int used_rs1 = -1;
int used_rs2 = -1;
int used_rd = -1;
#define src1R() do { *src1 = R(rs1); used_rs1 = rs1; } while (0)
#define src2R() do { *src2 = R(rs2); used_rs2 = rs2;} while (0)
#define immI() do { *imm = SEXT(BITS(i, 31, 20), 12); } while(0)
#define immU() do { *imm = SEXT(BITS(i, 31, 12), 20) << 12; } while(0)
#define immS() do { *imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7); } while(0)
#define immJ() do { *imm = SEXT(BITS(i, 31, 31) << 20 | BITS(i, 30, 21) << 1 | BITS(i, 20, 20) << 11 | BITS(i, 19, 12) << 12, 21); } while(0)
#define immB() do { *imm = SEXT(BITS(i, 31, 31) << 12 | BITS(i, 30, 25) << 5 | BITS(i, 11, 8) << 1 | BITS(i, 7, 7) << 11, 13); } while(0)



static void decode_operand(Decode *s, int *dest, word_t *src1,
 word_t *src2, word_t *imm, int type) {
  //根据type进行操作数解析
  //dest代表目的操作数
  uint32_t i = s->isa.inst.val;
  int rd  = BITS(i, 11, 7);
  int rs1 = BITS(i, 19, 15);
  int rs2 = BITS(i, 24, 20);

  used_rs1 = -1;
  used_rs2 = -1;
  used_rd = rd;

  *dest = rd;
  switch (type) {
    case TYPE_I: src1R();          immI(); break;
    case TYPE_U:                   immU(); break;
    case TYPE_S: src1R(); src2R(); immS(); break;
    case TYPE_J:                   immJ(); break;
    case TYPE_R: src1R(); src2R()        ; break;
    case TYPE_B: src1R(); src2R(); immB(); break;
  }
}
  //指令名称在代码中仅当注释使用, 不参与宏展开
  // 指令类型用于后续译码过程
  //指令执行操作则是通过C代码来模拟指令执行的真正行为
  //const void ** __instpat_end = &&__instpat_end_;
#define JUMP_BUF_SIZE 65536 

enum {CALL,RET};

static word_t jumpTo[JUMP_BUF_SIZE];  
static word_t jumpFrom[JUMP_BUF_SIZE];  
static bool jumpType[JUMP_BUF_SIZE];  
static int jumpid = 0;
// static void inst_csrrs(word_t sr , word_t src1, word_t dest);
static word_t sr_value(word_t imm);
static void sr_set(word_t imm,word_t value);
static void jump_inst_record(const Decode *s)
{
  // jumpFrom[jumpid] = s -> pc;
  // jumpTo[jumpid] = s -> dnpc;
  // //printf("jumpid = %d, from 0x%08x to 0x%08x\n",jumpid,jumpFrom[jumpid],jumpTo[jumpid]);
  // jumpid++;
  // assert(jumpid < JUMP_BUF_SIZE);
  ; 
}
static int decode_exec(Decode *s) {
  int dest = 0;
  word_t src1 = 0, src2 = 0, imm = 0;

  s->dnpc = s->snpc;//初始化dnpc 为 snpc = pc + 4

#define INSTPAT_INST(s) ((s)->isa.inst.val)
#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
  decode_operand(s, &dest, &src1, &src2, &imm, concat(TYPE_, type)); \
  __VA_ARGS__ ; \
}

  bool isjump = 0; 
  INSTPAT_START();
  //INSTPAT(pattern,name,type,执行的指令)
  INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, R(dest) = src1 + imm);
  INSTPAT("??????? ????? ????? ??? ????? 01101 11", lui    , U, R(dest) = imm);
  INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, s->dnpc = s->pc + imm,R(dest) = s->pc + 4, isjump = true);
  INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, s->dnpc = (imm + src1)&(~1), R(dest) = s->pc + 4, isjump = true);
  INSTPAT("0000000 ????? ????? 000 ????? 01100 11", add    , R, R(dest) = src1 + src2);
  INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw     , S, Mw(src1 + imm, 4, src2));//4 byte
  INSTPAT("??????? ????? ????? 001 ????? 01000 11", sh     , S, Mw(src1 + imm, 2, src2));//2 byte
  INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, Mw(src1 + imm, 1, src2));//1 byte
  INSTPAT("??????? ????? ????? 010 ????? 00000 11", lw     , I, R(dest) = Mr(src1 + imm, 4));
  INSTPAT("??????? ????? ????? 001 ????? 00000 11", lh     , I, R(dest) = SEXT(BITS(Mr(src1 + imm, 2), 15, 0), 16));//需要符号扩展
  INSTPAT("??????? ????? ????? 000 ????? 00000 11", lb     , I, R(dest) = SEXT(BITS(Mr(src1 + imm, 1), 7, 0), 8));
  INSTPAT("??????? ????? ????? 101 ????? 00000 11", lhu    , I, R(dest) = Mr(src1 + imm, 2),assert(BITS(R(dest),31,16) == 0));
  INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu    , I, R(dest) = Mr(src1 + imm, 1),assert(BITS(R(dest),31,8) == 0));
  INSTPAT("0100000 ????? ????? 000 ????? 01100 11", sub    , R, R(dest) = src1 - src2);
  INSTPAT("??????? ????? ????? 011 ????? 00100 11", sltiu  , I, R(dest) = (src1 < imm));
  INSTPAT("??????? ????? ????? 010 ????? 00100 11", slti   , I, R(dest) = ((int)src1 < (int)imm));
  INSTPAT("??????? ????? ????? 000 ????? 11000 11", beq    , B, s->dnpc = (src1 == src2) ? s->pc + imm : s->dnpc);
  INSTPAT("??????? ????? ????? 001 ????? 11000 11", bne    , B, s->dnpc = (src1 != src2) ? s->pc + imm : s->dnpc);
  INSTPAT("0000000 ????? ????? 011 ????? 01100 11", sltu   , R, R(dest) = (src1 < src2));
  INSTPAT("0000000 ????? ????? 010 ????? 01100 11", slt    , R, R(dest) = ((int)src1 < (int)src2));

  INSTPAT("??????? ????? ????? 100 ????? 00100 11", xori   , I, R(dest) = src1 ^ imm);
  INSTPAT("??????? ????? ????? 110 ????? 00100 11", ori    , I, R(dest) = src1 | imm);
  INSTPAT("??????? ????? ????? 111 ????? 00100 11", andi   , I, R(dest) = src1 & imm);
  INSTPAT("0000000 ????? ????? 001 ????? 00100 11", slli   , I, R(dest) = src1 << imm);
  INSTPAT("0000000 ????? ????? 101 ????? 00100 11", srli   , I, R(dest) = src1 >> imm);
  INSTPAT("0100000 ????? ????? 101 ????? 00100 11", srai   , I, R(dest) = (int)src1 >> (imm % (32)));//010000需要处理吗？
  INSTPAT("0000000 ????? ????? 100 ????? 01100 11", xor    , R, R(dest) = src1 ^ src2);
  INSTPAT("0000000 ????? ????? 110 ????? 01100 11", or     , R, R(dest) = src1 | src2);
  INSTPAT("0000000 ????? ????? 111 ????? 01100 11", and    , R, R(dest) = src1 & src2);
  INSTPAT("0000000 ????? ????? 001 ????? 01100 11", sll    , R, R(dest) = src1 << BITS(src2,4,0));
  INSTPAT("0000000 ????? ????? 101 ????? 01100 11", srl    , R, R(dest) = src1 >> BITS(src2,4,0));
  INSTPAT("0100000 ????? ????? 101 ????? 01100 11", sra    , R, R(dest) = (int)src1 >> BITS(src2,4,0));
  INSTPAT("??????? ????? ????? 100 ????? 11000 11", blt    , B, s->dnpc = ((int)src1 < (int)src2)  ? s->pc + imm : s->dnpc);
  INSTPAT("??????? ????? ????? 110 ????? 11000 11", bltu   , B, s->dnpc = ((word_t)src1 < (word_t)src2)            ? s->pc + imm : s->dnpc);
  INSTPAT("??????? ????? ????? 101 ????? 11000 11", bge    , B, s->dnpc = ((int)src1 >= (int)src2) ? s->pc + imm : s->dnpc);
  INSTPAT("??????? ????? ????? 111 ????? 11000 11", bgeu   , B, s->dnpc = ((word_t)src1 >= (word_t)src2)           ? s->pc + imm : s->dnpc);
  INSTPAT("0000001 ????? ????? 000 ????? 01100 11", mul    , R, R(dest) = src1 * src2);
  INSTPAT("0000001 ????? ????? 100 ????? 01100 11", div    , R, R(dest) = (int)src1 / (int)src2);
  INSTPAT("0000001 ????? ????? 101 ????? 01100 11", divu   , R, R(dest) = src1 / src2);
  INSTPAT("0000001 ????? ????? 001 ????? 01100 11", mulh   , R, R(dest) = BITS(((int64_t)((int)src1)  * (int64_t)((int)src2) ),63,32));
  INSTPAT("0000001 ????? ????? 010 ????? 01100 11", mulhsu , R, R(dest) = ((int64_t)((int)src1)  * (u_int64_t)src2) >> 32);
  INSTPAT("0000001 ????? ????? 011 ????? 01100 11", mulhu  , R, R(dest) = ((u_int64_t)src1 * (u_int64_t)src2) >> 32);
  INSTPAT("0000001 ????? ????? 110 ????? 01100 11", rem    , R, R(dest) = (int)src1 % (int)src2);
  INSTPAT("0000001 ????? ????? 111 ????? 01100 11", remu   , R, R(dest) = src1 % src2);
  INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc  , U, R(dest) = s->pc + imm);
  INSTPAT("??????? ????? ????? 011 ????? 00000 11", ld     , I, R(dest) = Mr(src1 + imm, 8));
  INSTPAT("??????? ????? ????? 011 ????? 01000 11", sd     , S, Mw(src1 + imm, 8, src2));

  //将state设置为end，halt_ret设置为R(10)
  INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, NEMUTRAP(s->pc, R(10))); 

  INSTPAT("0000000 00000 00000 000 00000 11100 11", ecall  , N, s->dnpc = isa_raise_intr(11,s->pc)); 
  INSTPAT("??????? ????? ????? 001 ????? 11100 11", csrrw  , I, sr_set(imm,src1),R(dest) = sr_value(imm)); 
  INSTPAT("??????? ????? ????? 010 ????? 11100 11", csrrs  , I, sr_set(imm,sr_value(imm) | src1),R(dest) = sr_value(imm)); 
  INSTPAT("0011000 00010 00000 000 00000 11100 11", mret   , R, s->dnpc = (sr(mepc) += 4 )); 

  INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));//无效指令


  INSTPAT_END();

  if(isjump)
    jump_inst_record(s);
  
  R(0) = 0; // reset $zero to 0

  return 0;
}
static void sr_set(word_t imm,word_t value)
{
  if(imm == 0x305)
  {
    sr(mtvec) = value;
    return ;
  }
  if(imm == 0x300)
  {
    sr(mstatus) = value;
    return ;
  }
  if(imm == 0x341)
  {
    sr(mepc) = value;
    return ;
  }
  if(imm == 0x342)
  {
    sr(mcause) = value;
    return ;
  }
  printf("no sr found\n");
  return ;
}
static word_t sr_value(word_t imm)
{
  if(imm == 0x305)
    return sr(mtvec);
  if(imm == 0x300)
    return sr(mstatus);
  if(imm == 0x341)
    return sr(mepc);
  if(imm == 0x342)
    return sr(mcause);
  printf("no sr found\n");
  return 0;
}
// static void inst_csrrs(word_t sr , word_t src1, word_t dest)
// {
//   // if(sr == 0x305)
//   // {
//   //   sr(m)
//   // }
//   return ;
// }
void init_jumpType()
{
  for(int i = 0 ; i < jumpid ; i++)
  {
    if(is_func_start(jumpTo[i]))
    {
      jumpType[i] = CALL;
    }
    else
      jumpType[i] = RET;
  }
}
char* get_func_name(word_t to);

//打印
void print_Ftrace()
{
  init_jumpType();

  char s[6];
  int space = 0;
  char* name;
  
  for(int i = 0 ; i < jumpid ; i++)
  {
    if(jumpType[i] == CALL)
    {
      strcpy(s,"call");
  
    }
    else
    {
      strcpy(s,"ret ");
      space--;
    }

    name = get_func_name(jumpTo[i]);

    assert(name);
    assert(space >= 0);

    printf("%4d 0x%08x : ",i,jumpFrom[i]);
    for(int j = 0 ; j < space ; j++)
    {
      printf(" ");
    }
    printf("%s [ %-2s 0x%08x ] \n", s, name, jumpTo[i]);
    if(jumpType[i] == CALL)
    {
      space++;
    }
    
    //printf("jumpid = %3d, from 0x%08x to 0x%08x, type = %d\n",i,jumpFrom[i],jumpTo[i], jumpType[i]);
  }
}
int isa_exec_once(Decode *s)
{
  //snpc = pc(execute 传入的 参数)
  s->isa.inst.val = inst_fetch(&s->snpc, 4);//取指令,存到val中
  return decode_exec(s);
}
