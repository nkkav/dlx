/***************************************************************/
/*  File       : dlx-isa.cpp
 *  Description: Instruction encodings and assembly formats.
 *  Author     : Nikolaos Kavvadias <nikos@nkavvadias.com>
 *
 *  Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010,
 *                2011, 2012, 2013, 2014 Nikolaos Kavvadias
 *
 *  Author affiliation:
 *    Dr. Nikolaos Kavvadias
 *    Independent Consultant -- Research Scientist
 *    Kornarou 12 Rd., Nea Ampliani,
 *    35100 Lamia, Greece
 *                                                             
 *  Contact information for the ArchC team:                     
 *    The ArchC Team                                              
 *    Computer Systems Laboratory (LSC)                           
 *    IC-UNICAMP                                                  
 *    http://www.lsc.ic.unicamp.br                                
 */
/***************************************************************/

#include  "dlx_isa.H"
#include  "dlx_isa_init.cpp"
#include  "dlx_bhv_macros.H"

//If you want debug information for this model, uncomment next line
//#define DEBUG_MODEL
#include "ac_debug_model.H"

//!User defined macros to reference registers.
#define Sp 29
#define Ra 31

// 'using namespace' statement to allow access to all
// dlx-specific datatypes.
using namespace dlx_parms;


//!Generic instruction behavior method.
void ac_behavior( instruction )
{
  dbg_printf("----- PC=%#x ----- %lld\n", (int) ac_pc, ac_instr_counter);
#ifndef NO_NEED_PC_UPDATE
  ac_pc = ac_pc +4;
#endif
};

//! Instruction Format behavior methods.
void ac_behavior( Type_R ){}
void ac_behavior( Type_I ){}
void ac_behavior( Type_J ){}

//!Behavior called before starting simulation
void ac_behavior(begin)
{
  unsigned int i;

  dbg_printf("@@@ begin behavior @@@\n");
  RB.write(0, 0);

  // Initialize all performance-counter registers
  for (i=0; i<1024; i++)
    PRB[i] = 0;

  // Initialize BBNUM register
  BBNUM = 0;
}

//!Behavior called after finishing simulation
void ac_behavior(end)
{
  FILE *prof_file;
  unsigned int i;

  dbg_printf("@@@ end behavior @@@\n");

//  dbg_printf("@@@ Printing general-purpose integer registers (RB) @@@\n");
  //
//  for (i=0; i<32; i++)
//  {
//    dbg_printf("RB[%#x] = %#x\n", i, RB[i]);
//    printf("RB[%#x] = %#x\n", i, RB.read(i));
//  }

  prof_file = fopen("run.prof","w");

  for (i=0; i<=BBNUM.read(); i++)
    fprintf(prof_file,"%d\n", PRB[i]);

//  for (i=0; i<=BBNUM.read(); i++)
//    fprintf(prof_file,"%d\n", PM.read(4*i));

  fclose(prof_file);  
}


//!Instruction lb behavior method.
void ac_behavior( lb )
{
  char byte;
  dbg_printf("lb r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
  byte = DM.read_byte(RB[rs] + imm);
  RB.write(rt, (ac_Sword)byte);
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction lbu behavior method.
void ac_behavior( lbu )
{
  unsigned char byte;
  dbg_printf("lbu r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
  byte = DM.read_byte(RB[rs]+ imm);
  RB.write(rt, byte);
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction lh behavior method.
void ac_behavior( lh )
{
  short int half;
  dbg_printf("lh r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
  half = DM.read_half(RB[rs]+ imm);
  RB.write(rt, (ac_Sword)half);
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction lhu behavior method.
void ac_behavior( lhu )
{
  unsigned short int  half;
  dbg_printf("lhu r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
  half = DM.read_half(RB[rs]+ imm);
  RB.write(rt, half);
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction lw behavior method.
void ac_behavior( lw )
{
  dbg_printf("lw r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
  RB[rt] = DM.read(RB[rs]+ imm);
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction sb behavior method.
void ac_behavior( sb )
{
  unsigned char byte;
  dbg_printf("sb %d(r%d), r%d\n", imm & 0xFFFF, rs, rt);
  byte = RB.read(rt) & 0xFF;
  DM.write_byte(RB[rs] + imm, byte);
  dbg_printf("Result = %#x\n", (int) byte);
};

//!Instruction sh behavior method.
void ac_behavior( sh )
{
  unsigned short int half;
  dbg_printf("sh %d(r%d), r%d\n", imm & 0xFFFF, rs, rt);
  half = RB.read(rt) & 0xFFFF;
  DM.write_half(RB[rs] + imm, half);
  dbg_printf("Result = %#x\n", (int) half);
};

//!Instruction sw behavior method.
void ac_behavior( sw )
{
  dbg_printf("sw %d(r%d), r%d\n", imm & 0xFFFF, rs, rt);
  DM.write(RB[rs] + imm, RB[rt]);
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction addi behavior method.
void ac_behavior( addi )
{
  dbg_printf("addi r%d, r%d, %#x\n", rt, rs, imm & 0xFFFF);
  RB[rt] = RB[rs] + imm;
  dbg_printf("Result = %#x\n", RB[rt]);
/*
  //Test overflow
  if ( ((RB[rs] & 0x80000000) == (imm & 0x80000000)) &&
       ((imm & 0x80000000) != (RB[rt] & 0x80000000)) ) 
  {
    fprintf(stderr, "EXCEPTION(addi): integer overflow.\n"); 
    exit(EXIT_FAILURE);
  }
*/
};

//!Instruction addui behavior method.
void ac_behavior( addui )
{
  dbg_printf("addui r%d, r%d, %#x\n", rt, rs, imm & 0xFFFF);
  RB[rt] = RB[rs] + (0xFFFF & imm);
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction subi behavior method.
void ac_behavior( subi )
{
  dbg_printf("subi r%d, r%d, %#x\n", rt, rs, imm & 0xFFFF);
  RB[rt] = RB[rs] - imm;
  dbg_printf("Result = %#x\n", RB[rt]);
/*
  //Test overflow
  if ( ((RB[rs] & 0x80000000) == (imm & 0x80000000)) &&
       ((imm & 0x80000000) != (RB[rt] & 0x80000000)) ) 
  {
    fprintf(stderr, "EXCEPTION(subi): integer overflow.\n"); 
    exit(EXIT_FAILURE);
  }
*/
};

//!Instruction subui behavior method.
void ac_behavior( subui )
{
  dbg_printf("subui r%d, r%d, %#x\n", rt, rs, imm & 0xFFFF);
  RB[rt] = RB[rs] - (0xFFFF & imm);
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction seqi behavior method.
void ac_behavior( seqi )
{
  dbg_printf("seqi r%d, r%d, %#x\n", rt, rs, imm & 0xFFFF);
  // Set the RD if RS==IMM
  if( (ac_Sword) RB[rs] == (ac_Sword) imm )
    RB[rt] = 1;
  // Else reset RD
  else
    RB[rt] = 0;
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction sequi behavior method.
void ac_behavior( sequi )
{
  dbg_printf("sequi r%d, r%d, %#x\n", rt, rs, imm & 0xFFFF);
  // Set the RD if RS==IMM
  if( (ac_Uword) RB[rs] == (ac_Uword) imm )
    RB[rt] = 1;
  // Else reset RD
  else
    RB[rt] = 0;
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction snei behavior method.
void ac_behavior( snei )
{
  dbg_printf("snei r%d, r%d, %#x\n", rt, rs, imm & 0xFFFF);
  // Set the RD if RS!=IMM
  if( (ac_Sword) RB[rs] != (ac_Sword) imm )
    RB[rt] = 1;
  // Else reset RD
  else
    RB[rt] = 0;
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction sneui behavior method.
void ac_behavior( sneui )
{
  dbg_printf("sneui r%d, r%d, %#x\n", rt, rs, imm & 0xFFFF);
  // Set the RD if RS!=IMM
  if( (ac_Uword) RB[rs] != (ac_Uword) imm )
    RB[rt] = 1;
  // Else reset RD
  else
    RB[rt] = 0;
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction slti behavior method.
void ac_behavior( slti )
{
  dbg_printf("slti r%d, r%d, %#x\n", rt, rs, imm & 0xFFFF);
  // Set the RD if RS< IMM
  if( (ac_Sword) RB[rs] < (ac_Sword) imm )
    RB[rt] = 1;
  // Else reset RD
  else
    RB[rt] = 0;
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction sltui behavior method.
void ac_behavior( sltui )
{
  dbg_printf("sltui r%d, r%d, %#x\n", rt, rs, imm & 0xFFFF);
  // Set the RD if RS< IMM
  if( (ac_Uword) RB[rs] < (ac_Uword) imm )
    RB[rt] = 1;
  // Else reset RD
  else
    RB[rt] = 0;
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction sgti behavior method.
void ac_behavior( sgti )
{
  dbg_printf("sgti r%d, r%d, %#x\n", rt, rs, imm & 0xFFFF);
  // Set the RD if RS>IMM
  if( (ac_Sword) RB[rs] > (ac_Sword) imm )
    RB[rt] = 1;
  // Else reset RD
  else
    RB[rt] = 0;
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction sgtui behavior method.
void ac_behavior( sgtui )
{
  dbg_printf("sgtui r%d, r%d, %#x\n", rt, rs, imm & 0xFFFF);
  // Set the RD if RS>IMM
  if( (ac_Uword) RB[rs] > (ac_Uword) imm )
    RB[rt] = 1;
  // Else reset RD
  else
    RB[rt] = 0;
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction slei behavior method.
void ac_behavior( slei )
{
  dbg_printf("slei r%d, r%d, %#x\n", rt, rs, imm & 0xFFFF);
  // Set the RD if RS<=IMM
  if( (ac_Sword) RB[rs] <= (ac_Sword) imm )
    RB[rt] = 1;
  // Else reset RD
  else
    RB[rt] = 0;
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction sleui behavior method.
void ac_behavior( sleui )
{
  dbg_printf("sleui r%d, r%d, %#x\n", rt, rs, imm & 0xFFFF);
  // Set the RD if RS<=IMM
  if( (ac_Uword) RB[rs] <= (ac_Uword) imm )
    RB[rt] = 1;
  // Else reset RD
  else
    RB[rt] = 0;
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction sgei behavior method.
void ac_behavior( sgei )
{
  dbg_printf("sgei r%d, r%d, %#x\n", rt, rs, imm & 0xFFFF);
  // Set the RD if RS>=IMM
  if( (ac_Sword) RB[rs] >= (ac_Sword) imm )
    RB[rt] = 1;
  // Else reset RD
  else
    RB[rt] = 0;
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction sgeui behavior method.
void ac_behavior( sgeui )
{
  dbg_printf("sgeui r%d, r%d, %#x\n", rt, rs, imm & 0xFFFF);
  // Set the RD if RS>=IMM
  if( (ac_Uword) RB[rs] >= (ac_Uword) imm )
    RB[rt] = 1;
  // Else reset RD
  else
    RB[rt] = 0;
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction andi behavior method.
void ac_behavior( andi )
{
  dbg_printf("andi r%d, r%d, %#x\n", rt, rs, imm & 0xFFFF);
  RB[rt] = RB[rs] & (imm & 0xFFFF);
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction ori behavior method.
void ac_behavior( ori )
{
  dbg_printf("ori r%d, r%d, %#x\n", rt, rs, imm & 0xFFFF);
  RB[rt] = RB[rs] | (imm & 0xFFFF);
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction xori behavior method.
void ac_behavior( xori )
{
  dbg_printf("xori r%d, r%d, %#x\n", rt, rs, imm & 0xFFFF);
  RB[rt] = RB[rs] ^ (imm & 0xFFFF);
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction lhi behavior method.
void ac_behavior( lhi )
{
  dbg_printf("lhi r%d, %#x\n", rt, imm & 0xFFFF);
  // Load a constant in the upper 16 bits of a register
  // To achieve the desired behaviour, the constant was shifted 16 bits left
  // and moved to the target register ( rt )
  RB[rt] = imm << 16;
//  RB[rt] = ((imm & 0xFFFF) << 16) | 0x0000;
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction add behavior method.
void ac_behavior( add )
{
  dbg_printf("add r%d, r%d, r%d\n", rd, rs, rt);
  RB[rd] = RB[rs] + RB[rt];
  dbg_printf("Result = %#x\n", RB[rd]);
/*
  //Test overflow
  if ( ((RB[rs] & 0x80000000) == (RB[rd] & 0x80000000)) &&
       ((RB[rd] & 0x80000000) != (RB[rt] & 0x80000000)) ) 
  {
    fprintf(stderr, "EXCEPTION(add): integer overflow.\n"); 
    exit(EXIT_FAILURE);
  }
*/
};

//!Instruction addu behavior method.
void ac_behavior( addu )
{
  dbg_printf("addu r%d, r%d, r%d\n", rd, rs, rt);
  RB[rd] = RB[rs] + RB[rt];
  dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction sub behavior method.
void ac_behavior( sub )
{
  dbg_printf("sub r%d, r%d, r%d\n", rd, rs, rt);
  RB[rd] = (ac_Sword)RB[rs] - (ac_Sword)RB[rt];
  dbg_printf("Result = %#x\n", RB[rd]);
  //TODO: test integer overflow exception for sub
};

//!Instruction subu behavior method.
void ac_behavior( subu )
{
  dbg_printf("subu r%d, r%d, r%d\n", rd, rs, rt);
  RB[rd] = RB[rs] - RB[rt];
  dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction seq behavior method.
void ac_behavior( seq )
{
  dbg_printf("seq r%d, r%d, r%d\n", rd, rs, rt);
  // Set the RD if RS==RT
  if( (ac_Sword) RB[rs] == (ac_Sword) RB[rt] )
    RB[rd] = 1;
  // Else reset RD
  else
    RB[rd] = 0;
  dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction sne behavior method.
void ac_behavior( sne )
{
  dbg_printf("sne r%d, r%d, r%d\n", rd, rs, rt);
  // Set the RD if RS!=RT
  if( (ac_Sword) RB[rs] != (ac_Sword) RB[rt] )
    RB[rd] = 1;
  // Else reset RD
  else
    RB[rd] = 0;
  dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction slt behavior method.
void ac_behavior( slt )
{
  dbg_printf("slt r%d, r%d, r%d\n", rd, rs, rt);
  // Set the RD if RS< RT
  if( (ac_Sword) RB[rs] < (ac_Sword) RB[rt] )
    RB[rd] = 1;
  // Else reset RD
  else
    RB[rd] = 0;
  dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction sltu behavior method.
void ac_behavior( sltu )
{
  dbg_printf("sltu r%d, r%d, r%d\n", rd, rs, rt);
  // Set the RD if RS < RT
  if( RB[rs] < RB[rt] )
    RB[rd] = 1;
  // Else reset RD
  else
    RB[rd] = 0;
  dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction sgt behavior method.
void ac_behavior( sgt )
{
  dbg_printf("sgt r%d, r%d, r%d\n", rd, rs, rt);
  // Set the RD if RS>RT
  if( (ac_Sword) RB[rs] > (ac_Sword) RB[rt] )
    RB[rd] = 1;
  // Else reset RD
  else
    RB[rd] = 0;
  dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction sgtu behavior method.
void ac_behavior( sgtu )
{
  dbg_printf("sgtu r%d, r%d, r%d\n", rd, rs, rt);
  // Set the RD if RS > RT
  if( RB[rs] > RB[rt] )
    RB[rd] = 1;
  // Else reset RD
  else
    RB[rd] = 0;
  dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction sle behavior method.
void ac_behavior( sle )
{
  dbg_printf("sle r%d, r%d, r%d\n", rd, rs, rt);
  // Set the RD if RS<=RT
  if( (ac_Sword) RB[rs] <= (ac_Sword) RB[rt] )
    RB[rd] = 1;
  // Else reset RD
  else
    RB[rd] = 0;
  dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction sleu behavior method.
void ac_behavior( sleu )
{
  dbg_printf("sleu r%d, r%d, r%d\n", rd, rs, rt);
  // Set the RD if RS <= RT
  if( RB[rs] <= RB[rt] )
    RB[rd] = 1;
  // Else reset RD
  else
    RB[rd] = 0;
  dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction sge behavior method.
void ac_behavior( sge )
{
  dbg_printf("sge r%d, r%d, r%d\n", rd, rs, rt);
  // Set the RD if RS>=RT
  if( (ac_Sword) RB[rs] >= (ac_Sword) RB[rt] )
    RB[rd] = 1;
  // Else reset RD
  else
    RB[rd] = 0;
  dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction sgeu behavior method.
void ac_behavior( sgeu )
{
  dbg_printf("sgeu r%d, r%d, r%d\n", rd, rs, rt);
  // Set the RD if RS >= RT
  if( RB[rs] >= RB[rt] )
    RB[rd] = 1;
  // Else reset RD
  else
    RB[rd] = 0;
  dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction instr_and behavior method.
void ac_behavior( instr_and )
{
  dbg_printf("and r%d, r%d, r%d\n", rd, rs, rt);
  RB[rd] = RB[rs] & RB[rt];
  dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction instr_or behavior method.
void ac_behavior( instr_or )
{
  dbg_printf("or r%d, r%d, r%d\n", rd, rs, rt);
  RB[rd] = RB[rs] | RB[rt];
  dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction instr_xor behavior method.
void ac_behavior( instr_xor )
{
  dbg_printf("xor r%d, r%d, r%d\n", rd, rs, rt);
  RB[rd] = RB[rs] ^ RB[rt];
  dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction nop behavior method.
void ac_behavior( nop )
{
  dbg_printf("nop\n");
};

//!Instruction sll behavior method.
void ac_behavior( sll )
{
  dbg_printf("sll r%d, r%d, r%d\n", rd, rt, rs);
  RB[rd] = RB[rt] << (RB[rs] & 0x1F);
  dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction srl behavior method.
void ac_behavior( srl )
{
  dbg_printf("srl r%d, r%d, r%d\n", rd, rt, rs);
  RB[rd] = RB[rt] >> (RB[rs] & 0x1F);
  dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction sra behavior method.
void ac_behavior( sra )
{
  dbg_printf("sra r%d, r%d, r%d\n", rd, rt, rs);
  RB[rd] = (ac_Sword) RB[rt] >> (RB[rs] & 0x1F);
  dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction slli behavior method.
void ac_behavior( slli )
{
  dbg_printf("slli r%d, r%d, %#x\n", rt, rs, imm & 0x1F);
  RB[rt] = RB[rs] << (imm & 0x1F);
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction srli behavior method.
void ac_behavior( srli )
{
  dbg_printf("srli r%d, r%d, %#x\n", rt, rs, imm & 0x1F);
  RB[rt] = RB[rs] >> (imm & 0x1F);
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction srai behavior method.
void ac_behavior( srai )
{
  dbg_printf("srai r%d, r%d, %#x\n", rt, rs, imm & 0x1F);
  RB[rt] = (ac_Sword) RB[rs] >> (imm & 0x1F);
  dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction mult behavior method.
void ac_behavior( mult )
{
  dbg_printf("mult r%d, r%d, r%d\n", rd, rs, rt);

  long long result;
  int half_result;

  result = (ac_Sword) RB[rs];
  result *= (ac_Sword) RB[rt];

  half_result = (result & 0xFFFFFFFF);
  // Register rd receives 32 less significant bits
  RB[rd] = half_result;

  dbg_printf("Result = %#x\n", half_result);
};

//!Instruction multu behavior method.
void ac_behavior( multu )
{
  dbg_printf("multu r%d, r%d, r%d\n", rd, rs, rt);

  unsigned long long result;
  unsigned int half_result;

  result  = RB[rs];
  result *= RB[rt];

  half_result = (result & 0xFFFFFFFF);
  // Register rd receives 32 less significant bits
  RB[rd] = half_result;

  dbg_printf("Result = %#x\n", half_result);
};

//!Instruction div behavior method.
void ac_behavior( div )
{
  dbg_printf("div r%d, r%d, r%d\n", rd, rs, rt);

  if (RB[rt] != 0)
  {
    if (rd != 0)
    {
      // Register rd receives quotient
      RB[rd] = (ac_Sword) RB[rs] / (ac_Sword) RB[rt];

      dbg_printf("Result = %#x\n", RB[rd]);
    }
    else
    {
      // Register $t8 (=r24) receives quotient
      RB[24] = (ac_Sword) RB[rs] / (ac_Sword) RB[rt];
      // Register $t9 (=r25) receives remainder
      RB[25] = (ac_Sword) RB[rs] % (ac_Sword) RB[rt];

      dbg_printf("Result = (q:%#x,r:%#x)\n", RB[24], RB[25]);
    }
  }
  else
  {
    fprintf(stderr,"Division by zero.\n");
    exit(EXIT_FAILURE);
  }
};

//!Instruction divu behavior method.
void ac_behavior( divu )
{
  dbg_printf("divu r%d, r%d, r%d\n", rd, rs, rt);

  if (RB[rt] != 0)
  {
    if (rd != 0)
    {
      // Register rd receives quotient
      RB[rd] = RB[rs] / RB[rt];

      dbg_printf("Result = %#x\n", RB[rd]);
    }
    else
    {
      // Register $t8 (=r24) receives quotient
      RB[24] = RB[rs] / RB[rt];
      // Register $t9 (=r25) receives remainder
      RB[25] = RB[rs] % RB[rt];
  
      dbg_printf("Result = (q:%#x,r:%#x)\n", RB[24], RB[25]);
    }
  }
  else
  {
    fprintf(stderr,"Division by zero.\n");
    exit(EXIT_FAILURE);
  }
};

//!Instruction mvts behavior method.
void ac_behavior( mvts )
{
  fprintf(stderr,"mvts behavior not implemented.\n");
  exit(EXIT_FAILURE);
};

//!Instruction mvfs behavior method.
void ac_behavior( mvfs )
{
  fprintf(stderr,"mvfs behavior not implemented.\n");
  exit(EXIT_FAILURE);
};

//!Instruction bswap behavior method.
/*
void ac_behavior( bswap )
{
  fprintf(stderr,"bswap behavior not implemented.\n");
  exit(EXIT_FAILURE);
};
*/
//!Instruction lut behavior method.
/*
void ac_behavior( lut )
{
  fprintf(stderr,"lut behavior not implemented.\n");
  exit(EXIT_FAILURE);
};
*/

//!Instruction j behavior method.
void ac_behavior( j )
{
  dbg_printf("j %#x\n", addr);
//  addr = addr << 2;
#ifndef NO_NEED_PC_UPDATE
//  ac_pc = ( (ac_pc & 0xF0000000) | addr);
  ac_pc = ac_pc + addr;
#endif
//  dbg_printf("Target = %#x\n", (ac_pc & 0xF0000000) | addr );
  dbg_printf("Target = %#x\n", (unsigned int)ac_pc);
};

//!Instruction jal behavior method.
void ac_behavior( jal )
{
  dbg_printf("jal %#x\n", addr);
  // Save the value of PC + 8 (return address) in $ra ($31) and
  // jump to the address given by PC(31...28)||(addr<<2)
  // It must also flush the instructions that were loaded into the pipeline
  RB[Ra] = ac_pc; //ac_pc is pc+4, we need pc+8

#ifndef NO_NEED_PC_UPDATE
  ac_pc = ac_pc + addr;
#endif

  dbg_printf("Target = %#x\n", (unsigned int)ac_pc);
  dbg_printf("Return = %#x\n", RB[Ra]);
};

//!Instruction jr behavior method.
void ac_behavior( jr )
{
  dbg_printf("jr r%d\n", rs);
  // Jump to the address stored on the register reg[RS]
  // It must also flush the instructions that were loaded into the pipeline
#ifndef NO_NEED_PC_UPDATE
  ac_pc = RB[rs];
#endif
  dbg_printf("Target = %#x\n", RB[rs]);
};

//!Instruction jalr behavior method.
void ac_behavior( jalr )
{
  dbg_printf("jalr r%d, r%d\n", rd, rs);
  // Save the value of PC + 8(return address) in rd and
  // jump to the address given by [rs]

  if ( rd == 0 )  //If rd is not defined use default
    RB[Ra] = ac_pc;
  else
    RB[rd] = ac_pc;

#ifndef NO_NEED_PC_UPDATE
  ac_pc = RB[rs];
#endif
  dbg_printf("Target = %#x\n", RB[rs]);
  dbg_printf("Return = %#x\n", RB[Ra]);
};

//!Instruction beqz behavior method.
void ac_behavior( beqz )
{
  dbg_printf("beqz r%d, %#x\n", rs, imm & 0xFFFF);
  if( RB[rs] == RB[0] ){
#ifndef NO_NEED_PC_UPDATE
//    ac_pc = delay(ac_pc + (imm<<2), 1);
    ac_pc = (ac_pc + imm);
#endif
    dbg_printf("Taken to %#x\n", (unsigned int)ac_pc);
  }
};

//!Instruction bnez behavior method.
void ac_behavior( bnez )
{
  dbg_printf("bnez r%d, %#x\n", rs, imm & 0xFFFF);
  if( RB[rs] != RB[0] ){
#ifndef NO_NEED_PC_UPDATE
//    ac_pc = delay(ac_pc + (imm<<2), 1);
    ac_pc = (ac_pc + imm);
#endif
    dbg_printf("Taken to %#x\n", (unsigned int)ac_pc);
  }
};

//!Instruction select behavior method.
void ac_behavior( select )
{
  dbg_printf("select r%d, r%d, r%d, r%d\n", rd, rs, rt, shamt);
  //
  //#define _SELECT(p, s0, s1) ((p) ? s0 : s1)
  RB[rd] = ((RB[rs]) ? RB[rt] : RB[shamt]);
  dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction trap behavior method.
void ac_behavior( trap )
{
  dbg_printf("trap\n");
  stop();
}

//!Instruction rfe behavior method.
void ac_behavior( rfe )
{
  fprintf(stderr, "rfe behavior not implemented.\n");
  exit(EXIT_FAILURE);
}

//!Instruction instr_break behavior method.
void ac_behavior( instr_break )
{
  fprintf(stderr, "instr_break behavior not implemented.\n");
  exit(EXIT_FAILURE);
}

//!Instruction halt behavior method.
void ac_behavior( halt )
{
  dbg_printf("halt\n");
  stop();
}

//!Instruction pcount behavior method.
void ac_behavior( pcount )
{
  unsigned int prb_address;

  dbg_printf("pcount %d\n", imm & 0x3FF);
//  dbg_printf("pcount %d\n", imm);
  prb_address = imm & 0x3FF;
//  prb_address = 4 * imm;

  PRB[prb_address]++;
//  if (PM.read(prb_address) == 0)
//  {
//    PM.write(prb_address, 1);
//  }
//  else
//  {
//    PM.write(prb_address, PM.read(prb_address)+1);
//  }

  if (prb_address > BBNUM)
    BBNUM.write(prb_address);

  dbg_printf("Result = %#x\n", PRB[prb_address]);
};
