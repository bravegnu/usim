#include "arm.h"
#include <iostream>
#include <iomanip>

enum Op {
  /* Data Processing */
  LSLI, LSRI, ASRI, ADD, SUB, ADDI, SUBI, MOV, CMPI,
  AND, EOR, LSL, LSR, ASR, ADC, SBC, ROR, TST, RSB, CMP, CMN,
  ORR, MUL, BIC, MVN,

  /* Relative Addressing */
  ADR,

  /* Branching */
  BX, BLX, BCOND, SVC,

  /* Load Store */
  STR, STRH, STRB, LDRSB, LDR, LDRH, LDRB, LDRSH, STRI, LDRI, STRBI,
  LDRBI, STRHI, LDRHI, LDM, STM,

  /* Misc. */
  SXTH, SXTB, UXTH, UXTB, PUSH, CPS, REV, REV16, REVSH, POP, BKPT,

  /* Hint */
  NOP, YIELD, WFE, WFI, SEV,

  /* Branch and Control */
  MSR, MRS, BL, DSB, DMB, ISB,
};

CortexM0::CortexM0(Clock& clock, SystemBus *sysbus)
{
  clock.add_clocked(this);
  m_sysbus = sysbus;

  for (auto i = 0; i < CortexM0::MAX_REGS; i++) {
    m_regs[i] = 0;
  }
}

uint16_t CortexM0::fetch_inst()
{
  return m_sysbus->read32(m_regs[15]) & 0xFFFF;
}

static inline uint8_t REG(uint16_t inst, unsigned offset)
{
  return (inst >> offset) & 0x7;
}

static inline uint8_t REG4M(uint16_t inst)
{
  return (inst >> 3) & 0xF;
}

static inline uint8_t REG4N(uint16_t inst)
{
  return ((inst >> 4) & 0x8) | ((inst >> 0) & 0x7);
}

static inline uint32_t IMM3(uint16_t inst)
{
  return (inst >> 6) & 0x7;
}

static inline uint32_t IMM5(uint16_t inst)
{
  return (inst >> 6) & 0x1F;
}

static inline uint32_t IMM7(uint16_t inst)
{
  return (inst >> 0) & 0x7F;
}

static inline uint32_t IMM8(uint16_t inst)
{
  return (inst >> 0) & 0xFF;
}

static inline uint32_t decode_imm_shift(uint32_t imm)
{
}

static DecodedInst decode_00xxxx(uint16_t inst)
{
  DecodedInst d;

  switch ((inst >> 9) & 0x1F) {
  case 0b00000 ... 0b00011:
    d.op = LSLI;
    d.rd = REG(inst, 0);
    d.rm = REG(inst, 3);
    d.imm = decode_imm_shift(IMM5(inst));
    break;

  case 0b00100 ... 0b00111:
    d.op = LSRI;
    d.rd = REG(inst, 0);
    d.rm = REG(inst, 3);
    d.imm = decode_imm_shift(IMM5(inst));
    break;

  case 0b01000 ... 0b01011:
    d.op = ASRI;
    d.rd = REG(inst, 0);
    d.rm = REG(inst, 3);
    d.imm = decode_imm_shift(IMM5(inst));
    break;

  case 0b01100:
    d.op = ADD;
    d.rm = REG(inst, 6);
    d.rn = REG(inst, 3);
    d.rd = REG(inst, 0);
    break;

  case 0b01101:
    d.op = SUB;
    d.rm = REG(inst, 6);
    d.rn = REG(inst, 3);
    d.rd = REG(inst, 0);
    break;

  case 0b01110:
    d.op = ADDI;
    d.imm = IMM3(inst);
    d.rn = REG(inst, 3);
    d.rd = REG(inst, 0);
    break;

  case 0b01111:
    d.op = SUBI;
    d.imm = IMM3(inst);
    d.rn = REG(inst, 3);
    d.rd = REG(inst, 0);
    break;

  case 0b10000 ... 0b10011:
    d.op = MOV;
    d.rd = REG(inst, 8);
    d.imm = IMM8(inst);
    break;

  case 0b10100 ... 0b10111:
    d.op = CMP;
    d.rn = REG(inst, 8);
    d.imm = IMM8(inst);
    break;

  case 0b11000 ... 0b11011:
    d.op = ADDI;
    d.rd = REG(inst, 8);
    d.rn = d.rd;
    d.imm = IMM8(0xFF);
    break;

  case 0b11100 ... 0b11100:
    d.op = SUBI;
    d.rd = REG(inst, 8);
    d.rn = d.rd;
    d.imm = IMM8(0xFF);
    break;
  }

  return d;
}

static DecodedInst decode_010000(uint16_t inst)
{
  DecodedInst d;

  /* Data Processing */
  d.rm = REG(inst, 3);
  d.rd = REG(inst, 0);
  d.rn = d.rd;

  switch ((inst >> 6) & 0xF) {
  case 0b0000:
    d.op = AND;
    break;

  case 0b0001:
    d.op = EOR;
    break;

  case 0b0010:
    d.op = LSL;
    break;

  case 0b0011:
    d.op = LSR;
    break;

  case 0b0100:
    d.op = ASR;
    break;

  case 0b0101:
    d.op = ADC;
    break;

  case 0b0110:
    d.op = SBC;
    break;

  case 0b0111:
    d.op = ROR;
    break;

  case 0b1000:
    d.op = TST;
    break;

  case 0b1001:
    d.op = RSB;
    break;

  case 0b1010:
    d.op = CMP;
    break;

  case 0b1011:
    d.op = CMN;
    break;

  case 0b1100:
    d.op = ORR;
    break;

  case 0b1101:
    d.op = MUL;
    break;

  case 0b1110:
    d.op = BIC;
    break;

  case 0b1111:
    d.op = MVN;
    break;
  }

  return d;
}

static DecodedInst decode_010001(uint16_t inst)
{
  DecodedInst d;

  switch ((inst >> 6) & 0xF) {
  case 0b0000 ... 0b0011:
    d.op = ADD;
    d.rn = REG4N(inst);
    d.rd = d.rn;
    d.rm = REG4M(inst);
    break;

  case 0b0100:
    break;

  case 0b0101:
  case 0b0110:
  case 0b0111:
    d.op = CMP;
    d.rn = REG4N(inst);
    d.rm = REG4M(inst);
    break;

  case 0b1000 ... 0b1011:
    d.op = MOV;
    d.rd = REG4N(inst);
    d.rm = REG4M(inst);
    break;

  case 0b1100:
  case 0b1101:
    d.op = BX;
    d.rm = REG4M(inst);
    break;

  case 0b1110:
  case 0b1111:
    d.op = BLX;
    d.rm = REG4M(inst);
    break;
  }

  return d;
}

DecodedInst decode_01001x(uint16_t inst)
{
  DecodedInst d;

  d.rd = REG(inst, 8);
  d.imm = IMM8(inst);

  return d;
}

DecodedInst decode_0101xx_011xxx_100xxx(uint16_t inst)
{
  DecodedInst d;

  uint8_t op_a = (inst >> 12);
  uint8_t op_b = (inst >> 9) & 0x7;

  switch (op_a) {
  case 0b0101:
    d.rm = REG(inst, 6);
    d.rn = REG(inst, 3);
    d.rd = REG(inst, 0);

    switch (op_b) {
    case 0b000:
      d.op = STR;
      break;
    case 0b001:
      d.op = STRH;
      break;
    case 0b010:
      d.op = STRB;
      break;
    case 0b011:
      d.op = LDRSB;
      break;
    case 0b100:
      d.op = LDRH;
      break;
    case 0b110:
      d.op = LDRB;
      break;
    case 0b111:
      d.op = LDRSH;
      break;
    }

    break;

  case 0b0110:
    d.imm = IMM5(inst);
    d.rn = REG(inst, 3);
    d.rd = REG(inst, 0);

    if (op_b & 0x4)
      d.op = LDRI;
    else
      d.op = STRI;
    break;

  case 0b0111:
    d.imm = IMM5(inst);
    d.rn = REG(inst, 3);
    d.rd = REG(inst, 0);

    if (op_b & 0x4)
      d.op = LDRBI;
    else
      d.op = STRBI;
    break;

  case 0b1000:
    d.imm = IMM5(inst);
    d.rn = REG(inst, 3);
    d.rd = REG(inst, 0);

    if (op_b & 0x4)
      d.op = LDRHI;
    else
      d.op = STRHI;
    break;

  case 0b1001:
    d.imm = IMM5(inst);
    d.rn = REG(inst, 3);
    d.rd = REG(inst, 0);

    if (op_b & 0x4)
      d.op = LDRI;
    else
      d.op = STRI;
    break;
  }

  return d;
}

DecodedInst decode_10100x(uint16_t inst)
{
  DecodedInst d;

  d.op = ADR;
  d.rd = REG(inst, 8);
  d.imm = IMM8(inst);

  return d;
}

DecodedInst decode_10101x(uint16_t inst)
{
  DecodedInst d;

  d.op = ADD;
  d.rd = REG(inst, 8);
  d.imm = IMM8(inst);

  return d;
}

DecodedInst decode_1011xx(uint16_t inst)
{
  DecodedInst d;
  unsigned int m;
  unsigned int p;

  switch ((inst >> 5) & 0x7F) {
  case 0b0000000 ... 0b0000011:
    d.op = ADDI;
    d.rd = 13;
    d.imm = IMM7(inst);
    break;
  case 0b0000100 ... 0b0000111:
    d.op = SUB;
    d.rd = 13;
    d.imm = IMM7(inst);
    break;
  case 0b0010000:
  case 0b0010001:
    d.op = SXTH;
    d.rd = REG(inst, 0);
    d.rm = REG(inst, 3);
    break;
  case 0b0010010:
  case 0b0010011:
    d.op = SXTB;
    d.rd = REG(inst, 0);
    d.rm = REG(inst, 3);
    break;
  case 0b0010100:
  case 0b0010101:
    d.op = UXTH;
    d.rd = REG(inst, 0);
    d.rm = REG(inst, 3);
    break;
  case 0b0010110:
  case 0b0010111:
    d.op = UXTB;
    d.rd = REG(inst, 0);
    d.rm = REG(inst, 3);
    break;
  case 0b0100000 ... 0b0101111:
    d.op = PUSH;
    m = (inst >> 8) & 0x1;
    d.specific = (inst & 0xFF) | m << 14;
    break;
  case 0b0110011:
    d.op = CPS;
    d.specific = (inst >> 4) & 0x1;
    break;
  case 0b1010000:
  case 0b1010001:
    d.op = REV;
    d.rm = REG(inst, 3);
    d.rd = REG(inst, 0);
    break;
  case 0b1010010:
  case 0b1010011:
    d.op = REV16;
    d.rm = REG(inst, 3);
    d.rd = REG(inst, 0);
    break;
  case 0b1010110:
  case 0b1010111:
    d.op = REVSH;
    d.rm = REG(inst, 3);
    d.rd = REG(inst, 0);
    break;
  case 0b1100000 ... 0b1101111:
    d.op = POP;
    p = (inst >> 8) & 0x1;
    d.specific = (inst & 0xFF) | (p << 15);
    break;
  case 0b1110000 ... 0b1110111:
    d.op = BKPT;
    d.imm = IMM8(inst);
    break;
  case 0b1111000 ... 0b1111111:
    switch (inst & 0xFF) {
    case 0x00:
      d.op = NOP;
      break;
    case 0x10:
      d.op = YIELD;
      break;
    case 0x20:
      d.op = WFE;
      break;
    case 0x30:
      d.op = WFI;
      break;
    case 0x40:
      d.op = SEV;
      break;
    }
    break;
  }

  return d;
}

DecodedInst decode_11000x(uint16_t inst)
{
  DecodedInst d;

  d.op = STM;
  d.rn = REG(inst, 8);
  d.specific = inst & 0xFF;

  return d;
}

DecodedInst decode_11001x(uint16_t inst)
{
  DecodedInst d;

  d.op = LDM;
  d.rn = REG(inst, 8);
  d.specific = inst & 0xFF;

  return d;
}

DecodedInst decode_1101xx(uint16_t inst)
{
  
}

DecodedInst decode_11100x(uint16_t inst)
{
}

DecodedInst CortexM0::decode_inst(uint16_t inst)
{
  switch (inst >> 10) {
  case 0b000000 ... 0b001111:
    return decode_00xxxx(inst);

  case 0b010000:
    return decode_010000(inst);

  case 0b010001:
    return decode_010001(inst);

  case 0b010010 ... 0b010011:
    return decode_01001x(inst);

  case 0b010100 ... 0b010111:
  case 0b011000 ... 0b011111:
  case 0b100000 ... 0b100111:
    return decode_0101xx_011xxx_100xxx(inst);

  case 0b101000 ... 0b101001:
    return decode_10100x(inst);
    
  case 0b101010 ... 0b101011:
    return decode_10101x(inst);

  case 0b101100 ... 0b101111:
    return decode_1011xx(inst);

  case 0b110000 ... 0b110001:
    return decode_11000x(inst);

  case 0b110010 ... 0b110011:
    return decode_11001x(inst);

  case 0b110100 ... 0b110111:
    return decode_1101xx(inst);

  case 0b111000 ... 0b111001:
    return decode_11100x(inst);
  }
}

void CortexM0::execute()
{
  uint16_t inst = fetch_inst();
  DecodedInst di = decode_inst(inst);

  switch (di.op) {
  case MOV:
    m_regs[di.rd] = di.imm;
    break;
  case ADD:
    m_regs[di.rd] = m_regs[di.rm] + m_regs[di.rn];
    break;
  }
  
  m_regs[15] += 2;
}
  
void CortexM0::dump_regs()
{
  for (auto i = 0; i < CortexM0::MAX_REGS; i++) {
    std::cout << "r" << std::dec << i << ": " << std::hex
	      << m_regs[i] << std::endl;
  }
}
