#include "arm.h"
#include "decoder.h"
#include <iostream>
#include <iomanip>

CortexM0::CortexM0(Clock& clock, SystemBus *sysbus)
{
  clock.add_clocked(this);
  m_sysbus = sysbus;

  for (auto i = 0; i < CortexM0::MAX_REGS; i++) {
    m_regs[i] = 0;
  }
}

uint16_t CortexM0::fetch_inst()
{
  return m_sysbus->read32(m_regs[15]) & 0xFFFF;
}

static inline bool IS_32BIT(uint32_t inst) 
{
  return (inst & 0xF800) == 0xF000;
}

void CortexM0::execute()
{
  DecodedInst di;
  uint16_t inst;
  uint32_t inst32;

  inst = fetch_inst();
  m_regs[15] += 2;

  if (!IS_32BIT(inst)) {
    Decoder decoder(inst, 0);
    di = decoder.di;
  } else {
    Decoder decoder(inst, 1);
    inst32 = (((uint32_t) inst) << 16) | fetch_inst();
    m_regs[15] += 2;
    di = decoder.di;
  }

  switch (di.op) {
  case MOV:
    m_regs[di.rd] = di.imm;
    break;
  case ADD:
    m_regs[di.rd] = m_regs[di.rm] + m_regs[di.rn];
    break;
  }
}
  
void CortexM0::dump_regs()
{
  for (auto i = 0; i < CortexM0::MAX_REGS; i++) {
    std::cout << "r" << std::dec << i << ": " << std::hex
	      << m_regs[i] << std::endl;
  }
}
