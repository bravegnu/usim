#ifndef ARM_H
#define ARM_H

#include <cstdint>
#include "clock.h"
#include "sysbus.h"

struct DecodedInst {
  uint8_t op;
  uint8_t rd;
  uint8_t rn;
  uint8_t rm;
  int32_t imm;
  uint16_t specific;
};

class CortexM0: public ClockedIf
{
 private:
  static const int MAX_REGS = 16;
  uint32_t m_regs[MAX_REGS];
  SystemBus *m_sysbus;
  uint16_t fetch_inst();
  DecodedInst decode_inst(uint16_t inst);
 public:
  CortexM0(Clock& clock, SystemBus *sysbus);
  void execute();
  void dump_regs();
};

#endif
