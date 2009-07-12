/*
 ****************************************************************************
 *
 * simulavr - A simulator for the Atmel AVR family of microcontrollers.
 * Copyright (C) 2009 Onno Kortmann
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
#ifndef traceval_h
#define traceval_h

#include "avrdevice.h"
#include <iostream>
#include <map>
#include <vector>

class Dumper;

/* TODO, notes:

   ===========================================================   
   Goals for tracing functionality for values in the simulator
   ===========================================================
   
   - being dumped into a file every time they changed (classical hardware
   register change use)

   - being dumped into a file every time they have been write-accessed
   (register with 'write-will-cause-something' functionality

   - enable statistics counters for accesses (such as EEPROM)
   
   - for eeproms, it should also be possible in particular to have an easy
   tracing for whole blocks of memory
   
   - statistics on the number of accesses on the FLASH can then be used for
   profiling and on the RAM for stack size checking etc.

      ... this leads to ...   
   ============================
   List of tracers to implement
   ============================
   - VCD dumping on changing a value. VCD dumping of the writes AND the READS
   itself as some kind of visible strobes may also be of interest!
   
   - Human-readable dumping of a simulavrxx trace, with register changes in a
   column next to the current opcode line etc. (much like the current tracing,
   but much cleaner in the code). Human-readable dumping should also say
   whether there has been a read-access or a write-access and whether the
   value changed during write!
   
   Also the interface should be able to warn when there is a read-access
   before a write access (e.g. read access of SRAM in unknown state).

   =====================   
   Ideas for further use
   =====================

   - memory array access profiling

   - 'wear map' for eeproms

   - For RAM, map with addresses that have been written at all
   
   - For flash, give out a map of read access counters (write-acesses later on
   for boot loaders??) for each flash address.  A separate tool could then be
   used to merge the output of avr-objdump -S and this to produce nice
   per-line profiling statistics. */

/*! Abstract interface for traceable values.
  Traced values can be written (marking it with a WRITE flag
  and if the value changed also a CHANGE flag. If the traced value
  has been written once, it is marked 'written()' for the whole simulation.
  They can also be read, marking the READ flag.

  For values where no accessors for read and write can be intercepted,
  it is also possible to use the cycle() method (activated when the
  traceval is initialized with a shadow ptr !=0), which will then simply
  update the state of the value during the cycle() method by comparing
  it with the internal state. This does not allow to trace read and write
  accesses, but all state changes will still be represented in the output file.
  This is helpful for e.g. tracing the hidden shadow states in various
  parts of the AVR hardware, such as the timer double buffers.
  */
class TraceValue {
 public:
    //! Generate a new unitialized trace value of width bits
    TraceValue(size_t bits,
               const std::string &_name,
               const int __index=-1,
               void* shadow=0);

    //! Give number of bits for this value. Max 32.
    size_t bits() const;

    //! Gives the saved shadow value for this trace value.
    /*! Note that the shadow value does not necessarily reflect
      the *current* value of the traced variable.
    */
    unsigned value() const;
    
    //! Give name (fully qualified), including the index appended if it is >=0
    std::string name() const;

    //! Gives the name without the index
    std::string barename() const;

    //! Gives the index of this member in a memory field (or -1)
    int index() const;
    
    enum Atype {
	READ=1, // true if a READ access has been logged
	WRITE=2,
	CHANGE=4
    };

    /*! Enabled? All operations should be skipped if a trace value is not
      enabled. */
    bool enabled() const;

    //! Enable tracing
    void enable();
    
    
    //! Log a write access on this value
    void write(unsigned val);
    //! Log a read access
    void read();
    

    /*! Gives true if this value has been written at one point during the
      simulation. */
    bool written() const;

    /*! Just set the written flag for tracevalues which are automatically
      initialized (IO registers etc.) */
    void set_written();
    
    //! Gives the current set of flag readings
    Atype flags() const;
	
    //! Called for each cycle if this trace value is activated
    /*! This may check for updates to an underlying referenced value etc.
      and update the flags accordingly. */
    virtual void cycle();
    
    /*! Dump the state or state change somewhere. This also resets the current
      flags. */
    virtual void dump(Dumper &d);
 protected:
    //! Clear all access flags
    void clear_flags();
    friend class TraceKeeper; 
 private:
    std::string _name;

    int _index;

    //! number of bits
    const unsigned b;

    //! shadow reg, if used
    void *shadow;

    //! The value itself
    unsigned v;
    //! accesses since last dump/clearflagsd
    int f;
    /*! Initialized to zero upon creation and any logged write will make this
      true. */
    bool _written;

    //! Tracing of this value enabled at all?
    /*! Note that it must additionally be enabled in the particular
      Dumper. */
    bool _enabled;

};

typedef std::vector<TraceValue*> TraceSet;

/*! Generic interface for a trace value processor */
class Dumper {
public:
    /*! Called with the set of all active signals,
      after they've been specified. */
    virtual void setActiveSignals(const TraceSet &act) {}

    //! Called before start of tracing
    virtual void start() {}
    //! Called after stopping tracing
    virtual void stop() {}

    //! Called for each cycle before dumping the values
    virtual void cycle() {}
    
    /*! Called when a traced value has been read (as long as it supports read
      logging!) */
    virtual void markRead(const TraceValue *t) {}
    /*! Called for all values which are read before they have been written. */
    virtual void markReadUnknown(const TraceValue *t) {}
    
    /*! Called when a traced value has been written (as long as it supports
      write logging!) */
    virtual void markWrite(const TraceValue *t) {}
    /*! Called when the value has changed. This is mainly used for values which
      do not have READ/WRITE notification by checking for changes after
      each clock cycle. All writes changing something also appear as a change.*/
    virtual void markChange(const TraceValue *t) {}

    //! Destructor, called for all dumpers at the very end of the run
    /*! Should close files etc. */
    virtual ~Dumper() {}

    //! Returns true iff tracing a particular value is enabled
    /*! FIXME: For a lot of values to trace,
      checking enabled() each time by doing find on a map()
      could be slow. Here is potential for more optimization! */
    virtual bool enabled(const TraceValue *t) const=0;
};

/*! Very simple dumper which will simply warn on unknown read
  accesses on stderr. */
class WarnUnknown : public Dumper {
 public:
    WarnUnknown(AvrDevice *core);
    void markReadUnknown(const TraceValue *t);
    bool enabled(const TraceValue *t) const;
  private:
    AvrDevice *core;
};

/*! Produces value change dump files. */
class DumpVCD : public Dumper {
 public:
    //! Create tracer with time scale tscale and output os
    DumpVCD(std::ostream *os, const std::string &tscale="ns",
	    const bool rstrobes=false, const bool wstrobes=false);
    
    void setActiveSignals(const TraceSet &act);

    //! Writes header stuff and the initial state
    void start();

    //! Writes next clock cycle and resets all RS and WS states
    void cycle();

    /*! Iff rstrobes is true, this will mark reads on a special
      R-strobe signal line. */
    void markRead(const TraceValue *t);

    /*! Iff wstrobes is true, this will mark writes on a special
      W-strobe signal line .*/
    void markWrite(const TraceValue *t);
    
    /*! This will produce a change in the value CHANGE dump file :-) */
    void markChange(const TraceValue *t);

    bool enabled(const TraceValue *t) const;
    ~DumpVCD();
 private:
    TraceSet tv;
    std::map<const TraceValue*, size_t> id2num;
    const std::string tscale;
    const bool rs, ws;

    // list of signals marked last cycle
    std::vector<int> marked;
    std::ostream *os;

    void valout(const TraceValue *v);
};


class AvrDevice;

/*! Manages all active Dumper instances for a given AvrDevice.
  It also manages all trace values and sets them active as necessary.
  */
class DumpManager {
 public:
    DumpManager(AvrDevice *core);
    /*! Registers a value of being traceable. Does NOT register
      the value as an ACTIVE trace value!

      \todo Maybe implement a more efficient memory management than a
      large mapping of custom strings to RWMemoryMember s...
      */
    void regTrace(TraceValue *tv);
		  
    /*! Add a dumper to the list. The vector vals
      contains all the values this dumper should trace. */
    void addDumper(Dumper *dump, const TraceSet &vals);

    /*! Start all dumpers. They will be stopped when the dump manager
      gets destroyed. */
    void start();

    /*! Process one AVR clock cycle. Must be done after the AVR did all
      processing so that changed values etc. can be collected. */
    void cycle();

    //! Shut down all dumpers
    ~DumpManager();

    /*! Write a list of tracing value names into the given
      output stream. */
    void save(std::ostream &os, const TraceSet &s) const;
    
    /*! Load a list of tracing values from the given input stream.
      Checks whether the values are part of the set of traceable
      values. */
    TraceSet load(std::istream &is);

    /*! Gives all available tracers as a set. */
    const TraceSet& all() const;
 private:
    //! Set of active tracing values
    TraceSet active;
    //! Set of all traceable values
    TraceSet _all;
    //! Maps all names of traceable values to the values themselves
    std::map<std::string, TraceValue*> all_map;
	
    //! All dumpers too use
    std::vector<Dumper*> dumps;
    AvrDevice *core;
};

/*! Sets a group for all next direct tracing values. Used to avoid
  repeated typing of the hardware group a direct traced value belongs to. */
void set_trace_group_s(const std::string &grp);

//! Register a directly traced bool value
void trace_direct(AvrDevice *c, const std::string &name, bool *val);
//! Register a directly traced byte value
void trace_direct(AvrDevice *c, const std::string &name, uint8_t *val);
//! Register a directly traced 16bit word value
void trace_direct(AvrDevice *c, const std::string &name, uint16_t *val);
//! Register a directly traced 32bit word value
void trace_direct(AvrDevice *c, const std::string &name, uint32_t *val);
//-----------------------------------------------------------------------------
#endif
