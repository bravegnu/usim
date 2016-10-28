#ifndef ARM_H
#define ARM_H

#include <cstdint>
#include "clock.h"
#include "sysbus.h"
#include "decoder.h"

class CortexM0: public ClockedIf
{
 private:
  static const int MAX_REGS = 16;
  static const int MAX_DECODERS = 64;
  uint32_t m_regs[MAX_REGS];
  SystemBus *m_sysbus;
  uint16_t fetch_inst();
  Decoder decoder;
 public:
  CortexM0(Clock& clock, SystemBus *sysbus);
  void execute();
  void dump_regs();
};

#endif
