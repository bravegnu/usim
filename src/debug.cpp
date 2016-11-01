#include "debug.h"
#include <assert.h>

const char *Debug::REG_NAMES[] = {
  "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
  "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
};

const char Debug::FLAGS[] = { 'c', 'v', 'n', 'z' };

Debug::Debug(uint32_t *regs, bool *c, bool *v, bool *n, bool *z, SystemBus *sysbus)
    : m_regs(regs)
    , m_c (c)
    , m_v (v)
    , m_n (n)
    , m_z (z)
    , m_sysbus (sysbus)
{
  /* pass */
}

uint32_t *Debug::parse_reg(std::string reg_name)
{
  if (reg_name[0] == 'r') {
    int index = std::stoi(reg_name.substr(1));
    return &m_regs[index];
  }

  /* FIXME: throw exception */
  return nullptr;
}

void Debug::set_reg(std::string reg_name, uint32_t value)
{
  uint32_t *regp;

  regp = parse_reg(reg_name);
  *regp = value;
}

uint32_t Debug::get_reg(std::string reg_name)
{
  uint32_t *regp;

  regp = parse_reg(reg_name);
  return *regp;
}

bool Debug::get_flag(char ch)
{
  switch (ch) {
    case 'c':
      return *m_c;
    case 'v':
      return *m_v;
    case 'n':
      return *m_n;
    case 'z':
      return *m_z;
    default:
      assert("invalid flag name");
  }
}

void Debug::set_flag(char ch, bool value)
{
  switch (ch) {
    case 'c':
      *m_c = value;
      break;
    case 'v':
      *m_v = value;
      break;
    case 'n':
      *m_n = value;
      break;
    case 'z':
      *m_z = value;
      break;
    default:
      assert("invalid flag name");
  }
}

void Debug::set_mem32(uint32_t addr, uint32_t value)
{
  m_sysbus->write32(addr, value);
}

uint32_t Debug::get_mem32(uint32_t addr)
{
  return m_sysbus->read32(addr);
}
