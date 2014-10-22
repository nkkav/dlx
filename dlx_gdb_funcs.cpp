/***************************************************************/
/*  File       : dlx_gdb_funcs.cpp
 *  Description: GDB support for the DLX simulator.
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

#include "dlx.H"

// 'using namespace' statement to allow access to all
// dlx-specific datatypes.
using namespace dlx_parms;

int dlx::nRegs(void) {
   return 33;
}

ac_word dlx::reg_read( int reg ) 
{
  /* general purpose registers */
  if ( ( reg >= 0 ) && ( reg < 32 ) )
    return RB.read( reg );
  else if ( reg == 32 ) // 37
    return ac_pc;

  return 0;
}

void dlx::reg_write( int reg, ac_word value )
{
  /* general purpose registers */
  if ( ( reg >= 0 ) && ( reg < 32 ) )
    RB.write( reg, value );
  else if ( reg == 32 ) // 37
    ac_pc = value;
}

unsigned char dlx::mem_read( unsigned int address )
{
  return IM->read_byte( address );
}

void dlx::mem_write( unsigned int address, unsigned char byte )
{
  IM->write_byte( address, byte );
}
