#include <iostream>
#include <string>
#include <cstdint>
#include "memory.h"
#include "sysbus.h"
#include "arm.h"
#include "clock.h"

static const uint32_t RAM_BASE = 0x20000000;

static void dump_state(Debug *debug)
{
  for (auto name: Debug::REG_NAMES)
    std::cout << name << " " << debug->get_reg(name) << std::endl;

  for (auto flag: Debug::FLAGS)
    std::cout << flag << " " << debug->get_flag(flag) << std::endl;

  for (uint32_t addr = 0; addr < 256; addr++)
    std::cout << (RAM_BASE + addr) << " " << debug->get_mem32(RAM_BASE + addr) << std::endl;
}

static bool is_flag(char c)
{
  for (auto flag: Debug::FLAGS) {
    if (c == flag) {
      return true;
    }
  }

  return false;
}

static bool is_reg(std::string name)
{
  for (auto reg_name: Debug::REG_NAMES) {
    if (reg_name == name) {
      return true;
    }
  }

  return false;  
}

void load_state(Debug *debug)
{
  std::string param;
  uint32_t value;

  while (std::cin) {
    std::cin >> param;
    std::cin >> value;

    if (param.length() == 1 and is_flag(param[0])) {
      debug->set_flag(param[0], value);
    } else if (is_reg(param)) {
      debug->set_reg(param, value);
    } else {
      /* FIXME: Implement integer error checking */
      debug->set_mem32(stoi(param), value);
    }
  }
}

int main()
{
  SystemBus bus;
  Clock clock;

  /* Initialize CPU */
  CortexM0 cpu(clock, &bus);

  /* Initialize Flash */
  Memory flash(1);
  flash.load_bin("test.bin", 0);
  bus.register_device(0x00000000, &flash);

  /* Initialize SRAM */
  Memory sram(1);
  bus.register_device(RAM_BASE, &sram);

  load_state(&cpu.debug);

  while (!cpu.is_halted()) {
    clock.tick();
  }

  dump_state(&cpu.debug);
}
