#ifndef DECODER_H
#define DECODER_H

#include <cstdint>

enum Cond {
  COND_EQ,
  COND_NE,
  COND_CS,
  COND_CC,
  COND_MI,
  COND_PL,
  COND_VS,
  COND_VC,
  COND_HI,
  COND_LS,
  COND_GE,
  COND_LT,
  COND_GT,
  COND_LE,
  COND_AL,
};

enum ShiftType {
  SHIFT_LSL,
  SHIFT_LSR,
  SHIFT_ASR,
  SHIFT_RRX,
  SHIFT_ROR,
};

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

  /* Undefined */
  UDF,
};

struct DecodedInst {
  uint8_t op;
  uint8_t rd;
  uint8_t rn;
  uint8_t rm;
  int32_t imm;
  uint16_t specific;
};

class Decoder {
private:
  uint32_t m_inst;
  bool m_is32;

  void decode_00xxxx();
  void decode_010000();
  void decode_010001();
  void decode_01001x();
  void decode_0101xx_011xxx_100xxx();
  void decode_10100x();
  void decode_10101x();
  void decode_1011xx();
  void decode_11000x();
  void decode_11001x();
  void decode_1101xx();
  void decode_11100x();
  void decode_inst();

  void decode_branch();
  void decode_branch_misc();
  void decode_inst32();

  uint8_t REG(unsigned offset)
  {
    return (m_inst >> offset) & 0x7;
  }

  uint8_t REG4(unsigned offset)
  {
    return (m_inst >> offset) & 0xF;
  }

  uint8_t REG4M()
  {
    return (m_inst >> 3) & 0xF;
  }

  uint8_t REG4N()
  {
    return ((m_inst >> 4) & 0x8) | ((m_inst >> 0) & 0x7);
  }

  uint32_t IMM3()
  {
    return (m_inst >> 6) & 0x7;
  }

  uint32_t IMM5()
  {
    return (m_inst >> 6) & 0x1F;
  }

  uint32_t IMM7()
  {
    return (m_inst >> 0) & 0x7F;
  }

  uint32_t IMM8()
  {
    return (m_inst >> 0) & 0xFF;
  }

  uint32_t SIMM8()
  {
    return (int32_t (IMM8() << 24)) >> 24;
  }

  uint32_t SIMM11()
  {
    return (int32_t ((m_inst & 0x7FF) << 21)) >> 21;
  }

  uint16_t HI()
  {
    return (m_inst >> 16) & 0xFF;
  }

  uint16_t LO()
  {
    return (m_inst & 0xFF);
  }

public:
  DecodedInst di;
  Decoder(uint32_t inst, bool is32);
};

#endif
