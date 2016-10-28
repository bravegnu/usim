#include "debug.h"

void Debug::set_reg(int index, uint32_t value)
{
  m_regs[index] = value;
}

uint32_t Debug::get_reg(int index)
{
  return m_regs[index];
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
      break
  }
}
