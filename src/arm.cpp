#include "arm.h"
#include "decoder.h"
#include "alu.h"
#include <iostream>
#include <iomanip>

CortexM0::CortexM0(Clock& clock, SystemBus *sysbus)
    : alu(m_regs, &m_c, &m_o, &m_n, &m_z)
{
  clock.add_clocked(this);
  m_sysbus = sysbus;

  for (auto i = 0; i < CortexM0::MAX_REGS; i++) {
    m_regs[i] = 0;
  }

  m_c = 0;
  m_o = 0;
  m_n = 0;
  m_z = 0;
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
    di = decoder.decode(inst, 0);
  } else {
    inst32 = (((uint32_t) inst) << 16) | fetch_inst();
    m_regs[15] += 2;
    di = decoder.decode(inst, 1);
  }

  alu.execute(&di);
}
  
void CortexM0::dump_regs()
{
  for (auto i = 0; i < CortexM0::MAX_REGS; i++) {
    std::cout << "r" << std::dec << i << ": " << std::hex
	      << m_regs[i] << std::endl;
  }
}
