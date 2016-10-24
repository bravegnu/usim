#include "arm.h"
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

void CortexM0::execute()
{
  uint16_t inst;

  inst = fetch_inst();

  if ((inst & 0xF800) == 0x2000) {
    int reg = (inst >> 8) & 0x07;
    int imm = inst & 0xFF;

    m_regs[reg] = imm;
  } else if ((inst & 0xFE00) == 0x1800) {
    int rd = inst & 0x07;
    int rn = (inst >> 3) & 0x07;
    int rm = (inst >> 6) & 0x07;

    m_regs[rd] = m_regs[rm] + m_regs[rn];
  }
  
  m_regs[15] += 2;
}
  
void CortexM0::dump_regs()
{
  for (auto i = 0; i < CortexM0::MAX_REGS; i++) {
    std::cout << "r" << std::dec << i << ": " << std::hex << m_regs[i] << std::endl;
  }
}
