/*
 *  This is a test program to demonstrate the simulator exit magic address.
 *
 *  $Id$
 */

#include "avrtest_help.h"

int main()
{
  debug_puts(
    "\n"
    "This program tests the simulator magic exit port.\n"
    "There should be no more messages after this one.\n"
  );

  *(unsigned char *)0x50 = 0x01;
  sim_exit(0);

  debug_puts( "ERROR - Simulator did not exit?\n" );

  return 0;
}
