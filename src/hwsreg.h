 /*
 ****************************************************************************
 *
 * simulavr - A simulator for the Atmel AVR family of microcontrollers.
 * Copyright (C) 2001, 2002, 2003   Klaus Rudolph		
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ****************************************************************************
 *
 *  $Id$
 */

#ifndef HWSREG
#define HWSREG
#include <string>

#include "rwmem.h"
#include "avrdevice.h"
/* maybe the faster solution... */
class HWSreg_bool {
  public:
    bool	I;
    bool	T;
    bool	H;
    bool	S;
    bool	V;
    bool	N;
    bool	Z;
    bool	C;
    operator int();
    HWSreg_bool(const int i);
    HWSreg_bool();
};

/* or is this the faster one ???? */
class HWSreg_bitarray {
  public:
    bool	I:1;
    bool	T:1;
    bool	H:1;
    bool	S:1;
    bool	V:1;
    bool	N:1;
    bool	Z:1;
    bool	C:1;
    operator int();
    HWSreg_bitarray(const int );
    HWSreg_bitarray();
};


class HWSreg: public HWSreg_bitarray {
  public:
    operator std::string();
    HWSreg operator =(const int );
};

/*! IO register mapping for the status register.
  \todo Replace the status register with an ordinary byte somewhere and simple
  inline access functions sN(), gN() to get/set flags. This should also make
  accesses faster. */
class RWSreg: public RWMemoryMember {
  public:
  RWSreg(AvrDevice *core, HWSreg *s) :RWMemoryMember(core, "CORE.SREG"), status(s) {}
  protected:
    HWSreg *status;
    unsigned char get() const;
    void set(unsigned char);
};
#endif
