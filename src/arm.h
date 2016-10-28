#ifndef ARM_H
#define ARM_H

#include <cstdint>
#include "clock.h"
#include "sysbus.h"
#include "decoder.h"
#include "alu.h"

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
  
  SystemBus *m_sysbus;
  uint16_t fetch_inst();
  Decoder decoder;
  ALU alu;
 public:
  CortexM0(Clock& clock, SystemBus *sysbus);
  void execute();
  void dump_regs();
};

#endif
