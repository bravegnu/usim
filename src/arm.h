#ifndef ARM_H
#define ARM_H

#include <cstdint>
#include "clock.h"
#include "sysbus.h"
#include "decoder.h"
#include "alu.h"
#include "debug.h"

class CortexM0: public ClockedIf
{
 private:
  static const int MAX_REGS = 16;
  uint32_t m_regs[MAX_REGS];
  bool m_c;
  bool m_o;
  bool m_n;
  bool m_z;
  uint32_t m_addr;
  uint32_t m_data;
  bool m_halted;
  
  SystemBus *m_sysbus;
  Decoder decoder;
  ALU alu;

  uint16_t fetch_inst();
 public:
  Debug debug;
  
  CortexM0(Clock& clock, SystemBus *sysbus);
  void execute();
  bool is_halted();
};

#endif
