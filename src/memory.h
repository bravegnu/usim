#ifndef MEMORY_H
#define MEMORY_H

#include "device.h"
#include <string>
#include <cstdint>

class Memory: public DeviceIf
{
 private:
  uint8_t *m_memory;
  unsigned int m_size_kb;
  uint32_t m_base_addr;
 public:
  Memory(unsigned int size_kb);
  void load_bin(std::string filename, uint32_t offset);
  uint32_t read32(uint32_t addr);
  void write32(uint32_t addr, uint32_t data);
  uint32_t get_size();
};

#endif
