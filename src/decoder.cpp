#include "decoder.h"

Decoder::Decoder(uint32_t inst, bool is32)
{
  m_inst = inst;
  m_is32 = is32;

  if (is32)
    decode_inst32();
  else
    decode_inst();
}

void Decoder::decode_00xxxx()
{
  switch ((m_inst >> 9) & 0x1F) {
  case 0b00000 ... 0b00011:
    di.op = LSLI;
    di.rd = REG(0);
    di.rm = REG(3);
    di.imm = IMM5();
    break;

  case 0b00100 ... 0b00111:
    di.op = LSRI;
    di.rd = REG(0);
    di.rm = REG(3);
    if (IMM5() == 0)
      di.imm = 32;
    else
      di.imm = IMM5();
    break;

  case 0b01000 ... 0b01011:
    di.op = ASRI;
    di.rd = REG(0);
    di.rm = REG(3);
    if (IMM5() == 0)
      di.imm = 32;
    else
      di.imm = IMM5();
    break;

  case 0b01100:
    di.op = ADD;
    di.rm = REG(6);
    di.rn = REG(3);
    di.rd = REG(0);
    break;

  case 0b01101:
    di.op = SUB;
    di.rm = REG(6);
    di.rn = REG(3);
    di.rd = REG(0);
    break;

  case 0b01110:
    di.op = ADDI;
    di.imm = IMM3();
    di.rn = REG(3);
    di.rd = REG(0);
    break;

  case 0b01111:
    di.op = SUBI;
    di.imm = IMM3();
    di.rn = REG(3);
    di.rd = REG(0);
    break;

  case 0b10000 ... 0b10011:
    di.op = MOV;
    di.rd = REG(8);
    di.imm = IMM8();
    break;

  case 0b10100 ... 0b10111:
    di.op = CMP;
    di.rn = REG(8);
    di.imm = IMM8();
    break;

  case 0b11000 ... 0b11011:
    di.op = ADDI;
    di.rd = REG(8);
    di.rn = di.rd;
    di.imm = IMM8();
    break;

  case 0b11100 ... 0b11100:
    di.op = SUBI;
    di.rd = REG(8);
    di.rn = di.rd;
    di.imm = IMM8();
    break;
  }
}

void Decoder::decode_010000()
{
  /* Data Processing */
  di.rm = REG(3);
  di.rd = REG(0);
  di.rn = di.rd;

  switch ((m_inst >> 6) & 0xF) {
  case 0b0000:
    di.op = AND;
    break;

  case 0b0001:
    di.op = EOR;
    break;

  case 0b0010:
    di.op = LSL;
    break;

  case 0b0011:
    di.op = LSR;
    break;

  case 0b0100:
    di.op = ASR;
    break;

  case 0b0101:
    di.op = ADC;
    break;

  case 0b0110:
    di.op = SBC;
    break;

  case 0b0111:
    di.op = ROR;
    break;

  case 0b1000:
    di.op = TST;
    break;

  case 0b1001:
    di.op = RSB;
    break;

  case 0b1010:
    di.op = CMP;
    break;

  case 0b1011:
    di.op = CMN;
    break;

  case 0b1100:
    di.op = ORR;
    break;

  case 0b1101:
    di.op = MUL;
    break;

  case 0b1110:
    di.op = BIC;
    break;

  case 0b1111:
    di.op = MVN;
    break;
  }
}

void Decoder::decode_010001()
{
  switch ((m_inst >> 6) & 0xF) {
  case 0b0000 ... 0b0011:
    di.op = ADD;
    di.rn = REG4N();
    di.rd = di.rn;
    di.rm = REG4M();
    break;

  case 0b0100:
    break;

  case 0b0101:
  case 0b0110:
  case 0b0111:
    di.op = CMP;
    di.rn = REG4N();
    di.rm = REG4M();
    break;

  case 0b1000 ... 0b1011:
    di.op = MOV;
    di.rd = REG4N();
    di.rm = REG4M();
    break;

  case 0b1100:
  case 0b1101:
    di.op = BX;
    di.rm = REG4M();
    break;

  case 0b1110:
  case 0b1111:
    di.op = BLX;
    di.rm = REG4M();
    break;
  }
}

void Decoder::decode_01001x()
{
  di.rd = REG(8);
  di.imm = IMM8();
}

void Decoder::decode_0101xx_011xxx_100xxx()
{
  uint8_t op_a = (m_inst >> 12);
  uint8_t op_b = (m_inst >> 9) & 0x7;

  switch (op_a) {
  case 0b0101:
    di.rm = REG(6);
    di.rn = REG(3);
    di.rd = REG(0);

    switch (op_b) {
    case 0b000:
      di.op = STR;
      break;
    case 0b001:
      di.op = STRH;
      break;
    case 0b010:
      di.op = STRB;
      break;
    case 0b011:
      di.op = LDRSB;
      break;
    case 0b100:
      di.op = LDRH;
      break;
    case 0b110:
      di.op = LDRB;
      break;
    case 0b111:
      di.op = LDRSH;
      break;
    }

    break;

  case 0b0110:
    di.imm = IMM5();
    di.rn = REG(3);
    di.rd = REG(0);

    if (op_b & 0x4)
      di.op = LDRI;
    else
      di.op = STRI;
    break;

  case 0b0111:
    di.imm = IMM5();
    di.rn = REG(3);
    di.rd = REG(0);

    if (op_b & 0x4)
      di.op = LDRBI;
    else
      di.op = STRBI;
    break;

  case 0b1000:
    di.imm = IMM5();
    di.rn = REG(3);
    di.rd = REG(0);

    if (op_b & 0x4)
      di.op = LDRHI;
    else
      di.op = STRHI;
    break;

  case 0b1001:
    di.imm = IMM5();
    di.rn = REG(3);
    di.rd = REG(0);

    if (op_b & 0x4)
      di.op = LDRI;
    else
      di.op = STRI;
    break;
  }
}

void Decoder::decode_10100x()
{
  di.op = ADR;
  di.rd = REG(8);
  di.imm = IMM8();
}

void Decoder::decode_10101x()
{
  di.op = ADD;
  di.rd = REG(8);
  di.imm = IMM8();
}

void Decoder::decode_1011xx()
{
  unsigned int m;
  unsigned int p;

  switch ((m_inst >> 5) & 0x7F) {
  case 0b0000000 ... 0b0000011:
    di.op = ADDI;
    di.rd = 13;
    di.imm = IMM7();
    break;
  case 0b0000100 ... 0b0000111:
    di.op = SUB;
    di.rd = 13;
    di.imm = IMM7();
    break;
  case 0b0010000:
  case 0b0010001:
    di.op = SXTH;
    di.rd = REG(0);
    di.rm = REG(3);
    break;
  case 0b0010010:
  case 0b0010011:
    di.op = SXTB;
    di.rd = REG(0);
    di.rm = REG(3);
    break;
  case 0b0010100:
  case 0b0010101:
    di.op = UXTH;
    di.rd = REG(0);
    di.rm = REG(3);
    break;
  case 0b0010110:
  case 0b0010111:
    di.op = UXTB;
    di.rd = REG(0);
    di.rm = REG(3);
    break;
  case 0b0100000 ... 0b0101111:
    di.op = PUSH;
    m = (m_inst >> 8) & 0x1;
    di.specific = (m_inst & 0xFF) | m << 14;
    break;
  case 0b0110011:
    di.op = CPS;
    di.specific = (m_inst >> 4) & 0x1;
    break;
  case 0b1010000:
  case 0b1010001:
    di.op = REV;
    di.rm = REG(3);
    di.rd = REG(0);
    break;
  case 0b1010010:
  case 0b1010011:
    di.op = REV16;
    di.rm = REG(3);
    di.rd = REG(0);
    break;
  case 0b1010110:
  case 0b1010111:
    di.op = REVSH;
    di.rm = REG(3);
    di.rd = REG(0);
    break;
  case 0b1100000 ... 0b1101111:
    di.op = POP;
    p = (m_inst >> 8) & 0x1;
    di.specific = (m_inst & 0xFF) | (p << 15);
    break;
  case 0b1110000 ... 0b1110111:
    di.op = BKPT;
    di.imm = IMM8();
    break;
  case 0b1111000 ... 0b1111111:
    switch (m_inst & 0xFF) {
    case 0x00:
      di.op = NOP;
      break;
    case 0x10:
      di.op = YIELD;
      break;
    case 0x20:
      di.op = WFE;
      break;
    case 0x30:
      di.op = WFI;
      break;
    case 0x40:
      di.op = SEV;
      break;
    }
    break;
  }
}

void Decoder::decode_11000x()
{
  di.op = STM;
  di.rn = REG(8);
  di.specific = m_inst & 0xFF;
}

void Decoder::decode_11001x()
{
  di.op = LDM;
  di.rn = REG(8);
  di.specific = m_inst & 0xFF;
}

void Decoder::decode_1101xx()
{
  switch ((m_inst >> 8) & 0xF) {
  case 0b1110:
    di.op = UDF;
    break;
  case 0b1111:
    di.op = SVC;
    di.imm = IMM8();
    break;
  default:
    di.op = BCOND;
    di.imm = SIMM8() << 1;
    di.specific = (m_inst >> 8) & 0xF;
    break;
  }
}

void Decoder::decode_11100x()
{
  di.op = BCOND;
  di.imm = SIMM11() << 1;
  di.specific = COND_AL;
}

void Decoder::decode_branch()
{
  unsigned int s = (HI() >> 10) & 0x1;
  unsigned int j1 = (LO() >> 13) & 0x1;
  unsigned int j2 = (LO() >> 13) & 0x1;
  unsigned int i1 = !(j1 ^ s);
  unsigned int i2 = !(j2 ^ s);
  uint32_t imm11 = (LO() & 0x7FF);
  uint32_t imm10 = (HI() & 0x3FF);

  di.imm |= imm11 << 1;
  di.imm |= imm10 << 12;
  di.imm |= i2 << 22;
  di.imm |= i1 << 23;
  di.imm |= s << 24;
  di.imm = ((int32_t)(di.imm << 7)) >> 7;
  di.op = BL;
}

void Decoder::decode_branch_misc()
{
  unsigned int op1 = (HI() >> 4) & 0x7F;
  unsigned int op2 = (LO() >> 12) & 0x7;
  unsigned int op3 = (LO() >> 4) & 0xF;

 switch (op2) {
 case 0b000:
 case 0b010:
   switch (op1) {
   case 0b0111000:
   case 0b0111001:
     di.op = MSR;
     di.rn = REG4(16);
     di.specific = LO() & 0xFF;
     break;
   case 0b0111011:
     if (op3 == 0b0100)
       di.op = DSB;
     if (op3 == 0b0101)
       di.op = DMB;
     if (op3 == 0b0110)
       di.op = ISB;
     break;
   case 0b0111110:
   case 0b0111111:
     di.op = MRS;
     di.rd = REG4(16);
     di.specific = LO() & 0xFF;
     break;
   case 0b1111111:
     di.op = UDF;
     break;
   }
   break;
 case 0b101:
 case 0b111:
   decode_branch();
   break;
 }
}

void Decoder::decode_inst32()
{
  unsigned int op1 = (HI() >> 11) & 0x3;
  unsigned int op = (LO() >> 15) & 0x1;

  if (op1 == 0b10) {
    decode_branch_misc();
  } else {
    di.op = UDF;
  }
}

void Decoder::decode_inst()
{
  switch (m_inst >> 10) {
  case 0b000000 ... 0b001111:
    decode_00xxxx();
    break;

  case 0b010000:
    decode_010000();
    break;

  case 0b010001:
    decode_010001();
    break;

  case 0b010010 ... 0b010011:
    decode_01001x();
    break;

  case 0b010100 ... 0b010111:
  case 0b011000 ... 0b011111:
  case 0b100000 ... 0b100111:
    decode_0101xx_011xxx_100xxx();
    break;

  case 0b101000 ... 0b101001:
    decode_10100x();
    break;
    
  case 0b101010 ... 0b101011:
    decode_10101x();
    break;

  case 0b101100 ... 0b101111:
    decode_1011xx();
    break;

  case 0b110000 ... 0b110001:
    decode_11000x();
    break;

  case 0b110010 ... 0b110011:
    decode_11001x();
    break;

  case 0b110100 ... 0b110111:
    decode_1101xx();
    break;

  case 0b111000 ... 0b111001:
    decode_11100x();
    break;
  }
}

