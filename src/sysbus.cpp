#include "sysbus.h"

SystemBus::SystemBus()
{
  /* empty */
}

void SystemBus::register_device(uint32_t base_addr, DeviceIf *dev)
{
  m_devs.push_back({base_addr, dev});
}

uint32_t SystemBus::read32(uint32_t addr)
{
  for (auto& sysdev: m_devs) {
    uint32_t base_addr = sysdev.base_addr;
    uint32_t size = sysdev.device->get_size();
    
    if (addr >= base_addr && addr < (base_addr + size)) {
      return sysdev.device->read32(addr - base_addr);
    }
  }
}

void SystemBus::write32(uint32_t addr, uint32_t data)
{
  for (auto& sysdev: m_devs) {
    uint32_t base_addr = sysdev.base_addr;
    uint32_t size = sysdev.device->get_size();

    if (addr >= base_addr && addr < (base_addr + size)) {
      sysdev.device->write32(addr - base_addr, data);
      return;
    }
  }
}

uint32_t SystemBus::get_size()
{
  throw "Not Implemented Error";
}
