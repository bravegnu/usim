#include "decoder.h"
#include <iostream>

/*
 * Calculate no. of bits in mask.
 */
constexpr int NBITS(int hi, int lo)
{
  return hi - lo + 1;
}

/*
 * Generate unshifted mask.
 */
constexpr uint16_t MASK(int hi, int lo)
{
  return (1 << NBITS(hi, lo)) - 1;
}

/*
 * Generate the bitmask for bits, between hi and lo.
 */
constexpr uint16_t BM(int hi, int lo)
{
  return MASK(hi, lo) << lo;
}

/*
 * Generate bitmask for bits between 15 and lo.
 */
constexpr uint16_t TM(int lo)
{
  return BM(15, lo);
}

constexpr uint16_t PTN(uint16_t pattern, int lo)
{
  return pattern << lo;
}

const DecoderTab Decoder::m_decode_table[] = {
  { TM( 6), PTN(0b0100000101,  6), &Decoder::decode_adc     },
  { TM( 9), PTN(0b0001110,     9), &Decoder::decode_addi_t1 },
  { TM(11), PTN(0b00110,      11), &Decoder::decode_addi_t2 },
  { TM( 8) | BM(6, 3), PTN(0b0100010001101, 3), &Decoder::decode_add_sp_t1 },
  { TM( 7), PTN(0b010001001,   7), &Decoder::decode_add_sp_t2 },
  { TM( 9), PTN(0b0001100,     9), &Decoder::decode_add_t1  },
  { TM( 8), PTN(0b01000100,    8), &Decoder::decode_add_t2  },
  { TM(11), PTN(0b10101,      11), &Decoder::decode_addi_sp_t1 },
  { TM( 7), PTN(0b101100000,   7), &Decoder::decode_addi_sp_t2 },
  { TM(11), PTN(0b10100,      11), &Decoder::decode_adr     },
  { TM( 6), PTN(0b0100000000,  6), &Decoder::decode_and     },
  { TM(11), PTN(0b00010,      11), &Decoder::decode_asri    },
  { TM( 6), PTN(0b0100000100,  6), &Decoder::decode_asr     },
  { TM( 8), PTN(0b11011111,    8), &Decoder::decode_svc     },
  { TM(12), PTN(0b1101,       12), &Decoder::decode_b_t1    },
  { TM(11), PTN(0b11100,      11), &Decoder::decode_b_t2    },
  { TM( 6), PTN(0b0100001110,  6), &Decoder::decode_bic     },
  { TM( 8), PTN(0b10111110,    8), &Decoder::decode_bkpt    },
  { TM( 7), PTN(0b010001111,   7), &Decoder::decode_blx     },
  { TM( 7), PTN(0b010001110,   7), &Decoder::decode_bx      },
  { TM( 6), PTN(0b0100001011,  6), &Decoder::decode_cmn     },
  { TM(11), PTN(0b00101,      11), &Decoder::decode_cmpi    },
  { TM( 6), PTN(0b0100001010,  6), &Decoder::decode_cmp_t1  },
  { TM( 8), PTN(0b01000101,    8), &Decoder::decode_cmp_t2  },
  { TM( 6), PTN(0b0100000001,  6), &Decoder::decode_eor     },
  { TM(11), PTN(0b11001,      11), &Decoder::decode_ldm     },
  { TM(11), PTN(0b01101,      11), &Decoder::decode_ldri_t1 },
  { TM(11), PTN(0b10011,      11), &Decoder::decode_ldri_t2 },
  { TM(11), PTN(0b01001,      11), &Decoder::decode_ldrl    },
  { TM( 9), PTN(0b0101100,     9), &Decoder::decode_ldr     },
  { TM(11), PTN(0b01111,      11), &Decoder::decode_ldrbi   },
  { TM( 9), PTN(0b0101110,     9), &Decoder::decode_ldrb    },
  { TM(11), PTN(0b10001,      11), &Decoder::decode_ldrhi   },
  { TM( 9), PTN(0b0101101,     9), &Decoder::decode_ldrh    },
  { TM( 9), PTN(0b0101011,     9), &Decoder::decode_ldrsb   },
  { TM( 9), PTN(0b0101111,     9), &Decoder::decode_ldrsh   },
  { TM(11), PTN(0b00000,      11), &Decoder::decode_lsli    },
  { TM( 6), PTN(0b0100000010,  6), &Decoder::decode_lsl     },
  { TM(11), PTN(0b00001,      11), &Decoder::decode_lsri    },
  { TM( 6), PTN(0b0100000011,  6), &Decoder::decode_lsr     },
  { TM(11), PTN(0b00100,      11), &Decoder::decode_movi    },
  { TM( 8), PTN(0b01000110,    8), &Decoder::decode_mov_t1  },
  { TM( 6), PTN(0b0000000000,  6), &Decoder::decode_mov_t2  },
  { TM( 6), PTN(0b0100001101,  6), &Decoder::decode_mul     },
  { TM( 6), PTN(0b0100001111,  6), &Decoder::decode_mvn     },
  { TM( 0), PTN(0b1011111100000000, 0), &Decoder::decode_nop },
  { TM( 6), PTN(0b0100001100,  6), &Decoder::decode_orr     },
  { TM( 9), PTN(0b1011110,     9), &Decoder::decode_pop     },
  { TM( 9), PTN(0b1011010,     9), &Decoder::decode_push    },
  { TM( 6), PTN(0b1011101000,  6), &Decoder::decode_rev     },
  { TM( 6), PTN(0b1011101001,  6), &Decoder::decode_rev16   },
  { TM( 6), PTN(0b1011101011,  6), &Decoder::decode_revsh   },
  { TM( 6), PTN(0b0100000111,  6), &Decoder::decode_ror     },
  { TM( 6), PTN(0b0100001001,  6), &Decoder::decode_rsb     },
  { TM( 6), PTN(0b0100000110,  6), &Decoder::decode_sbc     },
  { TM( 0), PTN(0b1011111101000000, 0), &Decoder::decode_sev },
  { TM(11), PTN(0b11000,      11), &Decoder::decode_stm     },
  { TM(11), PTN(0b01100,      11), &Decoder::decode_stri_t1 },
  { TM(11), PTN(0b10010,      11), &Decoder::decode_stri_t2 },  
  { TM( 9), PTN(0b0101000,     9), &Decoder::decode_str     },
  { TM(11), PTN(0b01110,      11), &Decoder::decode_strbi   },
  { TM( 9), PTN(0b0101010,     9), &Decoder::decode_strb    },
  { TM(11), PTN(0b10000,      11), &Decoder::decode_strhi   },
  { TM( 9), PTN(0b0101001,     9), &Decoder::decode_strh    },
  { TM( 9), PTN(0b0001111,     9), &Decoder::decode_subi_t1 },
  { TM(11), PTN(0b00111,      11), &Decoder::decode_subi_t2 },
  { TM( 9), PTN(0b0001101,     9), &Decoder::decode_sub     },
  { TM( 7), PTN(0b101100001,   7), &Decoder::decode_subi_sp },
  { TM( 6), PTN(0b1011001001,  6), &Decoder::decode_sxtb    },
  { TM( 6), PTN(0b1011001000,  6), &Decoder::decode_sxth    },
  { TM( 6), PTN(0b0100001000,  6), &Decoder::decode_tst     },
  { TM( 8), PTN(0b11011110,    8), &Decoder::decode_udf     },
  { TM( 6), PTN(0b1011001011,  6), &Decoder::decode_uxtb    },
  { TM( 6), PTN(0b1011001010,  6), &Decoder::decode_uxth    },
  { TM( 0), PTN(0b1011111100100000, 0), &Decoder::decode_wfe },
  { TM( 0), PTN(0b1011111100110000, 0), &Decoder::decode_wfi },
  { TM( 0), PTN(0b1011111100010000, 0), &Decoder::decode_yield },
};

Decoder::Decoder() {}

DecodedInst Decoder::decode(uint32_t inst, bool is32)
{
  m_inst = inst;
  m_is32 = is32;
  m_di.op = UDF;
  
  if (is32)
    decode_inst32();
  else
    decode_inst();

  return m_di;
}

void Decoder::decode_lsli()
{
  m_di.op = LSLI;
  m_di.rd = REG(0);
  m_di.rm = REG(3);
  m_di.imm = IMM5();
  m_di.setflags = true;
}

void Decoder::decode_lsri()
{
  m_di.op = LSRI;
  m_di.rd = REG(0);
  m_di.rm = REG(3);
  if (IMM5() == 0)
    m_di.imm = 32;
  else
    m_di.imm = IMM5();
  m_di.setflags = true;
}

void Decoder::decode_asri()
{
  m_di.op = ASRI;
  m_di.rd = REG(0);
  m_di.rm = REG(3);
  if (IMM5() == 0)
    m_di.imm = 32;
  else
    m_di.imm = IMM5();
  m_di.setflags = true;
}

void Decoder::decode_add_t1()
{
  m_di.op = ADD;
  m_di.rm = REG(6);
  m_di.rn = REG(3);
  m_di.rd = REG(0);
  m_di.setflags = true;
}

void Decoder::decode_sub()
{
  m_di.op = SUB;
  m_di.rm = REG(6);
  m_di.rn = REG(3);
  m_di.rd = REG(0);
  m_di.setflags = true;
}

void Decoder::decode_addi_t1()
{
  m_di.op = ADDI;
  m_di.imm = IMM3();
  m_di.rn = REG(3);
  m_di.rd = REG(0);
  m_di.setflags = true;
}

void Decoder::decode_subi_t1()
{
  m_di.op = SUBI;
  m_di.imm = IMM3();
  m_di.rn = REG(3);
  m_di.rd = REG(0);
  m_di.setflags = true;
}

void Decoder::decode_movi()
{
  m_di.op = MOV;
  m_di.rd = REG(8);
  m_di.imm = IMM8();
  m_di.setflags = true;
}

void Decoder::decode_cmpi()
{
  m_di.op = CMP;
  m_di.rn = REG(8);
  m_di.imm = IMM8();
  m_di.setflags = true;
}

void Decoder::decode_addi_t2()
{
  m_di.op = ADDI;
  m_di.rd = REG(8);
  m_di.rn = m_di.rd;
  m_di.imm = IMM8();
  m_di.setflags = true;
}

void Decoder::decode_subi_t2()
{
  m_di.op = SUBI;
  m_di.rd = REG(8);
  m_di.rn = m_di.rd;
  m_di.imm = IMM8();
  m_di.setflags = true;
}

void Decoder::decode_dproc()
{
  m_di.rm = REG(3);
  m_di.rd = REG(0);
  m_di.rn = m_di.rd;
}

void Decoder::decode_and()
{
  decode_dproc();
  m_di.op = AND;
  m_di.setflags = true;
}

void Decoder::decode_eor()
{
  decode_dproc();
  m_di.op = EOR;
  m_di.setflags = true;
}

void Decoder::decode_lsl()
{
  decode_dproc();
  m_di.op = LSL;
  m_di.setflags = true;
}

void Decoder::decode_lsr()
{
  decode_dproc();
  m_di.op = LSR;
  m_di.setflags = true;
}

void Decoder::decode_asr()
{
  decode_dproc();
  m_di.op = ASR;
  m_di.setflags = true;
}

void Decoder::decode_adc()
{
  decode_dproc();
  m_di.op = ADC;
  m_di.setflags = true;
}

void Decoder::decode_sbc()
{
  decode_dproc();
  m_di.op = SBC;
  m_di.setflags = true;
}

void Decoder::decode_ror()
{
  decode_dproc();
  m_di.op = ROR;
  m_di.setflags = true;
}

void Decoder::decode_tst()
{
  decode_dproc();
  m_di.op = TST;
}

void Decoder::decode_rsb()
{
  decode_dproc();
  m_di.op = RSB;
  m_di.setflags = true;
}

void Decoder::decode_cmn()
{
  decode_dproc();
  m_di.op = CMN;
  m_di.setflags = true;
}

void Decoder::decode_orr()
{
  decode_dproc();
  m_di.op = ORR;
  m_di.setflags = true;
}

void Decoder::decode_mul()
{
  decode_dproc();
  m_di.op = MUL;
  m_di.setflags = true;
}

void Decoder::decode_bic()
{
  decode_dproc();
  m_di.op = BIC;
  m_di.setflags = true;
}

void Decoder::decode_mvn()
{
  decode_dproc();
  m_di.op = MVN;
  m_di.setflags = true;
}

void Decoder::decode_add_t2()
{
  m_di.op = ADD;
  m_di.rn = REG4N();
  m_di.rd = m_di.rn;
  m_di.rm = REG4M();
  m_di.setflags = false;
}

void Decoder::decode_cmp_t1()
{
  m_di.op = CMP;
  m_di.rn = REG(0);
  m_di.rm = REG(3);
  m_di.setflags = true;
}

void Decoder::decode_cmp_t2()
{
  m_di.op = CMP;
  m_di.rn = REG4N();
  m_di.rm = REG4M();
  m_di.setflags = true;
}

void Decoder::decode_mov_t1()
{
  m_di.op = MOV;
  m_di.rd = REG4N();
  m_di.rm = REG4M();
  m_di.setflags = false;
}

void Decoder::decode_mov_t2()
{
  m_di.op = MOV;
  m_di.rm = REG(3);
  m_di.rd = REG(0);
  m_di.setflags = true;
}

void Decoder::decode_bx()
{
  m_di.op = BX;
  m_di.rm = REG4M();
}

void Decoder::decode_blx()
{
  m_di.op = BLX;
  m_di.rm = REG4M();
}

void Decoder::decode_ldrl()
{
  m_di.op = LDRL;
  m_di.rd = REG(8);
  m_di.imm = IMM8();
}

void Decoder::decode_load_store()
{
  m_di.rm = REG(6);
  m_di.rn = REG(3);
  m_di.rd = REG(0);
}

void Decoder::decode_str()
{
  decode_load_store();
  m_di.op = STR;
}

void Decoder::decode_strh()
{
  decode_load_store();
  m_di.op = STRH;
}

void Decoder::decode_strb()
{
  decode_load_store();
  m_di.op = STRB;
}

void Decoder::decode_ldrsb()
{
  decode_load_store();
  m_di.op = LDRSB;
}

void Decoder::decode_ldrh()
{
  decode_load_store();
  m_di.op = LDRH;
}

void Decoder::decode_ldrb()
{
  decode_load_store();
  m_di.op = LDRB;
}

void Decoder::decode_ldrsh()
{
  decode_load_store();
  m_di.op = LDRSH;
}

void Decoder::decode_ldr()
{
  decode_load_store();
  m_di.op = LDR;
}

void Decoder::decode_load_store_imm()
{
  m_di.imm = IMM5();
  m_di.rn = REG(3);
  m_di.rd = REG(0);
}

void Decoder::decode_ldri_t1()
{
  decode_load_store_imm();
  m_di.op = LDRI;
}

void Decoder::decode_stri_t1()
{
  decode_load_store_imm();
  m_di.op = STRI;
}

void Decoder::decode_ldrbi()
{
  decode_load_store_imm();
  m_di.op = LDRBI;
}

void Decoder::decode_strbi()
{
  decode_load_store_imm();
  m_di.op = STRBI;
}

void Decoder::decode_ldrhi()
{
  decode_load_store_imm();
  m_di.op = LDRHI;
}

void Decoder::decode_strhi()
{
  decode_load_store_imm();
  m_di.op = STRHI;
}

void Decoder::decode_ldri_t2()
{
  m_di.imm = IMM8();
  m_di.rn = 13;
  m_di.rd = REG(8);
  m_di.op = LDRI;
}

void Decoder::decode_stri_t2()
{
  m_di.imm = IMM8();
  m_di.rn = 13;
  m_di.rd = REG(8);
  m_di.op = STRI;
}

void Decoder::decode_adr()
{
  m_di.op = ADR;
  m_di.rd = REG(8);
  m_di.imm = IMM8();
}

void Decoder::decode_add_sp_t1()
{
  m_di.op = ADD;
  m_di.rd = REG4N();
  m_di.rm = m_di.rd;
  m_di.setflags = false;
}

void Decoder::decode_add_sp_t2()
{
  m_di.op = ADD;
  m_di.rd = 13;
  m_di.rm = REG4M();
  m_di.setflags = false;
}

void Decoder::decode_addi_sp_t1()
{
  m_di.op = ADDI;
  m_di.rd = REG(8);
  m_di.imm = IMM8();
  m_di.setflags = false;
}

void Decoder::decode_addi_sp_t2()
{
  m_di.op = ADDI;
  m_di.rd = 13;
  m_di.imm = IMM7();
  m_di.setflags = false;
}

void Decoder::decode_subi_sp()
{
  m_di.op = SUB;
  m_di.rd = 13;
  m_di.imm = IMM7();
  m_di.setflags = false;
}

void Decoder::decode_sxth()
{
  m_di.op = SXTH;
  m_di.rd = REG(0);
  m_di.rm = REG(3);
}

void Decoder::decode_sxtb()
{
  m_di.op = SXTB;
  m_di.rd = REG(0);
  m_di.rm = REG(3);
}

void Decoder::decode_uxth()
{
  m_di.op = UXTH;
  m_di.rd = REG(0);
  m_di.rm = REG(3);
}

void Decoder::decode_uxtb()
{
  m_di.op = UXTB;
  m_di.rd = REG(0);
  m_di.rm = REG(3);
}

void Decoder::decode_push()
{
  m_di.op = PUSH;
  unsigned int m = (m_inst >> 8) & 0x1;
  m_di.specific = (m_inst & 0xFF) | m << 14;
}

void Decoder::decode_cps()
{
  m_di.op = CPS;
  m_di.specific = (m_inst >> 4) & 0x1;
}

void Decoder::decode_rev()
{
  m_di.op = REV;
  m_di.rm = REG(3);
  m_di.rd = REG(0);
}

void Decoder::decode_rev16()
{
  m_di.op = REV16;
  m_di.rm = REG(3);
  m_di.rd = REG(0);
}

void Decoder::decode_revsh()
{
  m_di.op = REVSH;
  m_di.rm = REG(3);
  m_di.rd = REG(0);
}

void Decoder::decode_pop()
{
  m_di.op = POP;
  unsigned int p = (m_inst >> 8) & 0x1;
  m_di.specific = (m_inst & 0xFF) | (p << 15);
}

void Decoder::decode_bkpt()
{
  m_di.op = BKPT;
  m_di.imm = IMM8();
}

void Decoder::decode_nop()
{
  m_di.op = NOP;
}

void Decoder::decode_yield()
{
  m_di.op = YIELD;
}

void Decoder::decode_wfe()
{
  m_di.op = WFE;
}

void Decoder::decode_wfi()
{
  m_di.op = WFI;
}

void Decoder::decode_sev()
{
  m_di.op = SEV;
}

void Decoder::decode_stm()
{
  m_di.op = STM;
  m_di.rn = REG(8);
  m_di.specific = m_inst & 0xFF;
}

void Decoder::decode_ldm()
{
  m_di.op = LDM;
  m_di.rn = REG(8);
  m_di.specific = m_inst & 0xFF;
}

void Decoder::decode_udf()
{
  m_di.op = UDF;
}

void Decoder::decode_svc()
{
  m_di.op = SVC;
  m_di.imm = IMM8();
}

void Decoder::decode_b_t1()
{
  m_di.op = BCOND;
  m_di.imm = SIMM8() << 1;
  m_di.specific = (m_inst >> 8) & 0xF;
}

void Decoder::decode_b_t2()
{
  m_di.op = BCOND;
  m_di.imm = SIMM11() << 1;
  m_di.specific = COND_AL;
}

const Decoder32Tab Decoder::m_decode32_table[] = {
  { 0xFFFFFFF0, 0xF3DF8F50, &Decoder::decode_dmb },
  { 0xFFFFFFF0, 0xF3DF8F40, &Decoder::decode_dsb },
  { 0xFFFFFFF0, 0xF3DF8F60, &Decoder::decode_isb },
  { 0xFFFFF000, 0xF3EF8000, &Decoder::decode_mrs },
  { 0xFFF0FF00, 0xF3808800, &Decoder::decode_msr },
  { 0xFFF0F000, 0xF7F0A000, &Decoder::decode_udf32 },
  { 0xF800D000, 0xF000D000, &Decoder::decode_bl  },
};

void Decoder::decode_bl()
{
  unsigned int s = (HI16() >> 10) & 0x1;
  unsigned int j1 = (LO16() >> 13) & 0x1;
  unsigned int j2 = (LO16() >> 13) & 0x1;
  unsigned int i1 = !(j1 ^ s);
  unsigned int i2 = !(j2 ^ s);
  uint32_t imm11 = (LO16() & 0x7FF);
  uint32_t imm10 = (HI16() & 0x3FF);

  m_di.imm |= imm11 << 1;
  m_di.imm |= imm10 << 12;
  m_di.imm |= i2 << 22;
  m_di.imm |= i1 << 23;
  m_di.imm |= s << 24;
  m_di.imm = ((int32_t)(m_di.imm << 7)) >> 7;
  m_di.op = BL;
}

void Decoder::decode_msr()
{
  m_di.op = MSR;
  m_di.rn = REG4(16);
  m_di.specific = LO16() & 0xFF;
}

void Decoder::decode_dsb()
{
  m_di.op = DSB;
}

void Decoder::decode_dmb()
{
  m_di.op = DMB;
}

void Decoder::decode_isb()
{
  m_di.op = ISB;
}

void Decoder::decode_mrs()
{
  m_di.op = MRS;
  m_di.rd = REG4(16);
  m_di.specific = LO16() & 0xFF;
}

void Decoder::decode_udf32()
{
  m_di.op = UDF;
}

void Decoder::decode_inst32()
{
  int i;
  int tabsize = sizeof(m_decode32_table) / sizeof(m_decode32_table[0]);

  for (i = 0; i < tabsize; i++) {
    uint16_t mask = m_decode32_table[i].mask;
    uint16_t match = m_decode32_table[i].match;

    if ((m_inst & mask) == match) {
      (this->*(m_decode32_table[i].decode))();
      break;
    }
  }
}

void Decoder::decode_inst()
{
  int i;
  int tabsize = sizeof(m_decode_table) / sizeof(m_decode_table[0]);

  for (i = 0; i < tabsize; i++) {
    uint16_t mask = m_decode_table[i].mask;
    uint16_t match = m_decode_table[i].match;

    if ((m_inst & mask) == match) {
      (this->*(m_decode_table[i].decode))();
      break;
    }
  }
}

