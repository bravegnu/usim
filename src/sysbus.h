#ifndef SYSBUS_H
#define SYSBUS_H

#include <vector>
#include <cstdint>
#include "device.h"

struct SysDevice {
  uint32_t base_addr;
  DeviceIf *device;
};

class SystemBus: public DeviceIf
{
 private:
  std::vector<SysDevice> m_devs;
 public:
  SystemBus();
  void register_device(uint32_t base_addr, DeviceIf *dev);
  uint32_t read32(uint32_t addr);
  void write32(uint32_t addr, uint32_t data);
  uint32_t get_size();
};


#endif
