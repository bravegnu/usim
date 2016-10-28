#ifndef DEBUG_H
#define DEBUG_H

class Debug {
 private:
  uint32_t *m_regs;
  bool *m_c;
  bool *m_v;
  bool *m_n;
  bool *m_z;
 public:
  void set_reg(int index, uint32_t value);
  void get_reg(int index, uint32_t value);
  bool get_flag(char ch);
  bool set_flag(char ch, bool val);
}

#endif
