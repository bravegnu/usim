#ifndef DEVICE_H
#define DEVICE_H

#include <cstdint>

class DeviceIf
{
 public:
  virtual uint32_t read32(uint32_t addr) = 0;
  virtual void write32(uint32_t addr, uint32_t data) = 0;
  virtual uint32_t get_size() = 0;
};

#endif
