#ifndef DEBUG_H
#define DEBUG_H

#include <cstdint>
#include <string>
#include "sysbus.h"

class Debug {
 private:
  uint32_t *m_regs;
  bool *m_c;
  bool *m_v;
  bool *m_n;
  bool *m_z;
  SystemBus *m_sysbus;

  uint32_t *parse_reg(std::string reg_name);
 public:
  static const char *REG_NAMES[16];
  static const char FLAGS[4];
  Debug(uint32_t *regs, bool *c, bool *v, bool *n, bool *z, SystemBus *sysbus);
  void set_reg(std::string reg_name, uint32_t value);
  uint32_t get_reg(std::string reg_name);
  bool get_flag(char ch);
  void set_flag(char ch, bool val);
  void set_mem32(uint32_t addr, uint32_t value);
  uint32_t get_mem32(uint32_t addr);
};

#endif
