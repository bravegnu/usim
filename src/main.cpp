#include <iostream>
#include "memory.h"
#include "sysbus.h"
#include "arm.h"
#include "clock.h"

int main()
{
  SystemBus bus;
  Clock clock;

  /* Initialize CPU */
  CortexM0 cpu(clock, &bus);

  /* Initialize Flash */
  Memory flash(1024);
  flash.load_bin("add.bin", 0);
  bus.register_device(0x00000000, &flash);

  /* Initialize SRAM */
  Memory sram(1024);
  bus.register_device(0x20000000, &sram);

  clock.tick();
  clock.tick();
  clock.tick();

  cpu.dump_regs();
}
