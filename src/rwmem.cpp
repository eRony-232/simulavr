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

/*
 * All here defined types are used to simulate the 
 * read write address space. This means also registers
 * io-data space, internal and external sram
 */

#include "avrerror.h"
#include "traceval.h"
#include "helper.h"
#include "rwmem.h"

using namespace std;

RWMemoryMember::RWMemoryMember(
    AvrDevice *_core,
    const std::string &tracename,
    const int index)
    : core(_core) {
    if (tracename.size()) {
        tv=new TraceValue(8, tracename, index);
        if (!core) {
            avr_error(("core not initialized for RWMemoryMember '"+
                       tracename+"'.").c_str());
        }
        if (!core->dump_manager) {
            avr_error(("core->dump_manager not initialized for RWMemoryMember '"+
                       tracename+"'.").c_str());
        }
        core->dump_manager->regTrace(tv);
    } else {
        tv=0;
    }
}

RWMemoryMember::operator unsigned char() const {
    if (tv)
	tv->read();
    return get();
}

unsigned char RWMemoryMember::operator=(unsigned char val) {
    set(val);
    if (tv)
	tv->write(val);
    return val;
}

unsigned char RWMemoryMember::operator=(const RWMemoryMember &mm) {
    if (mm.tv)
	mm.tv->read();
    unsigned char v=mm.get();
    set(v);
    if (tv)
	tv->write(v);
    return v;
}


RWMemoryMember::~RWMemoryMember() {
    if (tv)
	delete tv;
}

RAM::RAM(AvrDevice *core,
         const std::string &name,
         const size_t number) : RWMemoryMember(core, name, number) {}

unsigned char RAM::get() const { return value; }
void RAM::set(unsigned char v) { value=v; }

    
InvalidMem::InvalidMem(
    AvrDevice *core,
    const std::string &name,
    const size_t number) : RWMemoryMember(core, name, number) {}

RWMemoryMember& MemoryOffsets::operator[](unsigned int externOffset) const {
    return *(rwHandler[myOffset+externOffset]);
}

unsigned char InvalidMem::get() const {
    cerr << "Invalid read access to " << tv->name() << "." << endl;
}


void InvalidMem::set(unsigned char c) {
    cerr << "Invalid write access to " << tv->name()
         << ", trying to set value [0x"
         << hex << int(c) << dec << "], PC=0x"
         << hex << 2*core->PC << dec << endl;
}


