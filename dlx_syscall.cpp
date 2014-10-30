/***************************************************************/
/*  File       : dlx_syscall.cpp
 *  Description: OS call emulation support for DLX.
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

#include "dlx_syscall.H"

// 'using namespace' statement to allow access to all
// dlx-specific datatypes.
using namespace dlx_parms;


void dlx_syscall::get_buffer(int argn, unsigned char* buf, unsigned int size)
{
  unsigned int addr = RB[4+argn];

  for (unsigned int i = 0; i<size; i++, addr++) {
    buf[i] = DM.read_byte(addr);
  }
}

void dlx_syscall::set_buffer(int argn, unsigned char* buf, unsigned int size)
{
  unsigned int addr = RB[4+argn];

  for (unsigned int i = 0; i<size; i++, addr++) {
    DM.write_byte(addr, buf[i]);
  }
}

void dlx_syscall::set_buffer_noinvert(int argn, unsigned char* buf, unsigned int size)
{
  unsigned int addr = RB[4+argn];

  for (unsigned int i = 0; i<size; i+=4, addr+=4) {
    DM.write(addr, *(unsigned int *) &buf[i]);
  }
}

int dlx_syscall::get_int(int argn)
{
  return RB[4+argn];
}

void dlx_syscall::set_int(int argn, int val)
{
  RB[2+argn] = val;
}

void dlx_syscall::return_from_syscall()
{
  ac_pc = RB[31];
}

void dlx_syscall::set_prog_args(int argc, char **argv)
{
  int i, j, base;

  unsigned int ac_argv[30];
  char ac_argstr[512];

  base = AC_RAM_END - 512;
  for (i=0, j=0; i<argc; i++) {
    int len = strlen(argv[i]) + 1;
    ac_argv[i] = base + j;
    memcpy(&ac_argstr[j], argv[i], len);
    j += len;
  }

  //Adjust $sp and write argument string
  RB[29] = AC_RAM_END-512;
  set_buffer(25, (unsigned char*) ac_argstr, 512);   //$25 = $29(sp) - 4 (set_buffer adds 4)

  //Adjust $sp and write string pointers
  RB[29] = AC_RAM_END-512-120;
  set_buffer_noinvert(25, (unsigned char*) ac_argv, 120);

  //Set $sp
  RB[29] = AC_RAM_END-512-128;

  //Set $a0 to the argument count
  RB[4] = argc;

  //Set $a1 to the string pointers
  RB[5] = AC_RAM_END-512-120;
}
