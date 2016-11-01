#include <iostream>
#include <fstream>
#include <sstream>
#include <cerrno>
#include <cstring>
#include "memory.h"
#include "sysbus.h"
#include "utils.h"

Memory::Memory(unsigned int size_kb)
{
  m_memory = new uint8_t[size_kb * 1024];
  std::memset(m_memory, 0, size_kb * 1024);
  m_size_kb = size_kb;
}

uint32_t Memory::read32(uint32_t addr)
{
  uint32_t val;
  
  val = m_memory[addr];
  val |= m_memory[addr + 1] << 8;
  val |= m_memory[addr + 2] << 16;
  val |= m_memory[addr + 3] << 24;

  return val;
}

void Memory::write32(uint32_t addr, uint32_t data)
{
  m_memory[addr] = data & 0xFF;
  m_memory[addr + 1] = (data >> 8) & 0xFF;
  m_memory[addr + 2] = (data >> 16) & 0xFF;
  m_memory[addr + 3] = (data >> 24) & 0xFF;
}

void Memory::load_bin(char const *filename, uint32_t offset)
{
  std::ifstream infile;
  uint32_t size = m_size_kb * 1024;

  infile.open(filename);

  if (!infile) {
    std::ostringstream message;
    message << "error opening file '" << filename << "': ";
    throw IOError(errno, message.str());
  }

  while (!infile.eof()) {
    uint8_t byte;

    if (offset >= size)
      break;

    infile >> byte;
    m_memory[offset] = byte;
    offset++;
  }
}

uint32_t Memory::get_size()
{
  return m_size_kb * 1024;
}
